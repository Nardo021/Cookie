#include "SkinChangerInventory.hpp"

#include "SkinChanger.hpp"

#include <algorithm>
#include <atomic>
#include <vector>

#include <CS2/SDK/Cstrike15/CCSInventoryManager.hpp>
#include <CS2/SDK/Cstrike15/CCSPlayerInventory.hpp>
#include <CS2/SDK/Econ/CEconItem.hpp>
#include <CS2/SDK/Econ/CEconItemDefinition.hpp>
#include <CS2/SDK/Econ/CEconItemSchema.hpp>
#include <CS2/SDK/Econ/CEconItemSystem.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/GCSDK/GCSDKTypes/EconItemConstants.hpp>
#include <CS2/SDK/GCSDK/GCSDKTypes/ESOCacheEvent.hpp>
#include <CS2/SDK/Interface/CGameEntitySystem.hpp>
#include <CS2/SDK/Interface/CSource2Client.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Interface/INetworkClientService.hpp>
#include <CS2/SDK/Interface/IGameEvent.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Types/CHandle.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace SkinChangerInventory
{
	namespace
	{
		std::vector<uint64_t> s_addedItemIds;
		std::atomic<bool> s_pendingNetworkFullUpdate{ false };

		auto GetLoadoutSlotForDefIndex( int defIndex ) noexcept -> int
		{
			auto* source2 = SDK::Interfaces::Source2Client();
			if ( !source2 )
				return -1;

			auto* schema = source2->GetEconItemSystem()->GetEconItemSchema();
			if ( !schema )
				return -1;

			const auto definition = schema->GetSortedItemDefinitionMap().FindByKey( defIndex );
			if ( !definition )
				return -1;

			return static_cast<int>( definition.value()->LoadoutSlot() );
		}

		auto FindManagedItem( int defIndex ) noexcept -> CEconItem*
		{
			auto* inventory = CCSPlayerInventory::Get();
			if ( !inventory )
				return nullptr;

			for ( const uint64_t itemId : s_addedItemIds )
			{
				if ( auto* item = inventory->GetSOCDataForItem( itemId ) )
				{
					if ( item->m_unDefIndex == static_cast<uint16_t>( defIndex ) )
						return item;
				}
			}

			return nullptr;
		}

		auto CreateSkinItem( int defIndex , const SkinChanger::SkinConfig& skin ) noexcept -> CEconItem*
		{
			auto* inventory = CCSPlayerInventory::Get();
			if ( !inventory )
				return nullptr;

			auto* item = CEconItem::Create();
			if ( !item )
				return nullptr;

			const auto highestIds = inventory->GetHighestIDs();
			item->m_ulID = highestIds.first + 1;
			item->m_unInventory = highestIds.second + 1;
			item->m_unAccountID = static_cast<uint32_t>( inventory->GetOwner().m_id );
			item->m_unDefIndex = static_cast<uint16_t>( defIndex );
			item->m_nQuality = IQ_UNIQUE;
			item->m_nRarity = IR_RARE;

			if ( skin.paintKit > 0 )
				item->SetPaintKit( static_cast<float>( skin.paintKit ) );

			item->SetPaintSeed( static_cast<float>( skin.seed ) );
			item->SetPaintWear( skin.wear );

			if ( skin.statTrak >= 0 )
			{
				item->SetStatTrak( skin.statTrak );
				item->SetStatTrakType( 0 );
			}

			if ( !inventory->AddEconItem( item ) )
				return nullptr;

			AddEconItemId( item->m_ulID );
			return item;
		}

		auto UpdateSkinItem( CEconItem* item , const SkinChanger::SkinConfig& skin ) noexcept -> void
		{
			if ( !item )
				return;

			if ( skin.paintKit > 0 )
				item->SetPaintKit( static_cast<float>( skin.paintKit ) );

			item->SetPaintSeed( static_cast<float>( skin.seed ) );
			item->SetPaintWear( skin.wear );

			if ( skin.statTrak >= 0 )
			{
				item->SetStatTrak( skin.statTrak );
				item->SetStatTrakType( 0 );
			}
		}

		auto EquipAndRefresh( CEconItem* item , int team , int slot ) noexcept -> bool
		{
			auto* inventoryManager = CCSInventoryManager::Get();
			auto* inventory = CCSPlayerInventory::Get();
			if ( !inventoryManager || !inventory || !item )
				return false;

			if ( !inventoryManager->EquipItemInLoadout( team , slot , item->m_ulID ) )
				return false;

			if ( auto* loadoutItem = inventory->GetItemInLoadout( team , slot ) )
			{
				if ( auto* loadoutSoc = loadoutItem->GetSOCData() )
				{
					inventory->SOUpdated(
						inventory->GetOwner() ,
						reinterpret_cast<CSharedObject*>( loadoutSoc ) ,
						GCSDK::eSOCacheEvent_Incremental );
				}
			}

			return true;
		}

		auto SyncWeaponEntity(
			CCSPlayerInventory* inventory ,
			uint64_t steamId ,
			C_CSWeaponBase* weapon ,
			C_CSGOViewModel* viewModel ) noexcept -> void
		{
			if ( !inventory || !weapon )
				return;

			if ( weapon->GetOriginalOwnerXuid() != steamId )
				return;

			auto* weaponItemView = weapon->m_AttributeManager()->m_Item();
			if ( !weaponItemView )
				return;
			auto* weaponDefinition = weaponItemView->GetStaticData();
			if ( !weaponDefinition )
				return;

			C_EconItemView* loadoutItemView = nullptr;

			if ( weaponDefinition->IsWeapon() && !weaponDefinition->IsKnife( true ) )
			{
				for ( int slot = 0; slot <= 56; ++slot )
				{
					auto* candidate = inventory->GetItemInLoadout( weapon->m_iOriginalTeamNumber() , slot );
					if ( !candidate )
						continue;

					if ( candidate->m_iItemDefinitionIndex() == weaponDefinition->m_nDefIndex() )
					{
						loadoutItemView = candidate;
						break;
					}
				}
			}
			else
			{
				const int team = weapon->m_iOriginalTeamNumber();
				const int slot = GetLoadoutSlotForDefIndex( weaponDefinition->m_nDefIndex() );
				loadoutItemView = inventory->GetItemInLoadout( team , slot );
			}

			if ( !loadoutItemView )
				return;

			if ( !IsManagedItem( loadoutItemView->m_iItemID() ) )
				return;

			auto* loadoutDefinition = loadoutItemView->GetStaticData();
			if ( !loadoutDefinition )
				return;

			const bool isKnife = loadoutDefinition->IsKnife( false );

			weaponItemView->m_bDisallowSOC() = false;
			loadoutItemView->m_bDisallowSOC() = false;

			weaponItemView->m_iItemID() = loadoutItemView->m_iItemID();
			weaponItemView->m_iItemIDHigh() = loadoutItemView->m_iItemIDHigh();
			weaponItemView->m_iItemIDLow() = loadoutItemView->m_iItemIDLow();
			weaponItemView->m_iAccountID() = static_cast<uint32_t>( inventory->GetOwner().m_id );
			weaponItemView->m_iItemDefinitionIndex() = loadoutDefinition->m_nDefIndex();

			auto* weaponSceneNode = weapon->m_pGameSceneNode();
			if ( !weaponSceneNode )
				return;

			const CHandle weaponHandle = weapon->pEntityIdentity()
				? weapon->pEntityIdentity()->Handle()
				: CHandle{};

			if ( isKnife )
			{
				if ( viewModel
					&& ( viewModel->m_hWeapon().GetEntryIndex() == weaponHandle.GetEntryIndex()
						|| viewModel->m_hWeapon() == weaponHandle ) )
				{
					weaponItemView->m_iItemDefinitionIndex() = loadoutDefinition->m_nDefIndex();

					if ( const char* knifeModel = loadoutDefinition->m_pszModelName() )
					{
						C_BaseModelEntity_SetModel( weapon , knifeModel );
						C_BaseModelEntity_SetModel( viewModel , knifeModel );
					}

					if ( auto* vmNode = viewModel->m_pGameSceneNode() )
					{
						CGameSceneNode_SetMeshGroupMask( weaponSceneNode , 2 );
						CGameSceneNode_SetMeshGroupMask( vmNode , 2 );
					}
				}
			}
			else
			{
				if ( const char* model = loadoutDefinition->m_pszModelName() )
					C_BaseModelEntity_SetModel( weapon , model );

				if ( viewModel && viewModel->m_hWeapon() == weaponHandle )
				{
					if ( const char* model = loadoutDefinition->m_pszModelName() )
						C_BaseModelEntity_SetModel( viewModel , model );
				}

				CGameSceneNode_SetMeshGroupMask( weaponSceneNode , 2 );

				if ( viewModel && viewModel->m_hWeapon().GetEntryIndex() == weaponHandle.GetEntryIndex() )
				{
					if ( auto* vmNode = viewModel->m_pGameSceneNode() )
						CGameSceneNode_SetMeshGroupMask( vmNode , 2 );
				}
			}
		}
	}

	auto AddEconItemId( uint64_t itemId ) noexcept -> void
	{
		if ( itemId == 0 )
			return;

		if ( std::find( s_addedItemIds.begin() , s_addedItemIds.end() , itemId ) == s_addedItemIds.end() )
			s_addedItemIds.emplace_back( itemId );
	}

	auto IsManagedItem( uint64_t itemId ) noexcept -> bool
	{
		return std::find( s_addedItemIds.begin() , s_addedItemIds.end() , itemId ) != s_addedItemIds.end();
	}

	auto UsesInventoryPath() noexcept -> bool
	{
		if ( !s_addedItemIds.empty() || SkinChanger::knifeChangerEnabled )
			return true;

		std::lock_guard<std::mutex> lock( SkinChanger::configMutex );
		for ( const auto& entry : SkinChanger::weaponSkins )
		{
			if ( entry.second.enabled && entry.second.paintKit > 0 )
				return true;
		}

		return false;
	}

	auto SyncFromConfig() noexcept -> void
	{
		auto* inventoryManager = CCSInventoryManager::Get();
		auto* inventory = CCSPlayerInventory::Get();
		if ( !inventoryManager || !inventory )
			return;

		std::lock_guard<std::mutex> lock( SkinChanger::configMutex );

		for ( const auto& [defIndex , skin] : SkinChanger::weaponSkins )
		{
			if ( !skin.enabled || skin.paintKit <= 0 )
				continue;

			const int slot = GetLoadoutSlotForDefIndex( defIndex );
			if ( slot < 0 )
				continue;

			CEconItem* item = FindManagedItem( defIndex );
			if ( !item )
				item = CreateSkinItem( defIndex , skin );
			else
				UpdateSkinItem( item , skin );

			if ( !item )
				continue;

			for ( const int team : { 2 , 3 } )
				EquipAndRefresh( item , team , slot );
		}

		if ( SkinChanger::knifeChangerEnabled
			&& SkinChanger::selectedKnifeModel > 0
			&& SkinChanger::selectedKnifeModel < SkinChanger::knifeModelCount )
		{
			const int knifeDef = SkinChanger::knifeModels[SkinChanger::selectedKnifeModel].defIndex;
			if ( knifeDef > 0 )
			{
				SkinChanger::SkinConfig knifeSkin{};
				if ( const auto it = SkinChanger::weaponSkins.find( knifeDef ); it != SkinChanger::weaponSkins.end() )
					knifeSkin = it->second;

				knifeSkin.enabled = true;

				CEconItem* knifeItem = FindManagedItem( knifeDef );
				if ( !knifeItem )
					knifeItem = CreateSkinItem( knifeDef , knifeSkin );
				else if ( knifeSkin.paintKit > 0 )
					UpdateSkinItem( knifeItem , knifeSkin );

				if ( knifeItem )
				{
					const int knifeSlot = GetLoadoutSlotForDefIndex( knifeDef );
					if ( knifeSlot >= 0 )
					{
						for ( const int team : { 2 , 3 } )
							EquipAndRefresh( knifeItem , team , knifeSlot );
					}
				}
			}
		}

		if ( SkinChanger::forceUpdate.load() )
			SkinChanger::forceUpdate.store( false );
	}

	auto OnFrameStageNotify( int frameStage ) noexcept -> void
	{
		if ( frameStage != 6 )
			return;

		auto* inventory = CCSPlayerInventory::Get();
		auto* entitySystem = SDK::Interfaces::GameEntitySystem();
		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !inventory || !entitySystem || !localPawn || !localPawn->IsAlive() )
			return;

		if ( SkinChanger::forceUpdate.load() )
		{
			SyncFromConfig();
			RequestNetworkFullUpdate();
		}

		const uint64_t steamId = inventory->GetOwner().m_id;

		auto* viewModelServices = localPawn->m_pViewModelServices();
		C_CSGOViewModel* viewModel = nullptr;
		if ( viewModelServices )
			viewModel = reinterpret_cast<C_CSGOViewModel*>( viewModelServices->m_hViewModel()->Get() );

		const int highestIndex = entitySystem->GetHighestEntityIndex();
		for ( int i = 64 + 1; i <= highestIndex; ++i )
		{
			auto* entity = entitySystem->GetBaseEntity<C_BaseEntity>( i );
			if ( !entity )
				continue;

			auto* weapon = reinterpret_cast<C_CSWeaponBase*>( entity );
			if ( !weapon->IsBasePlayerWeapon() )
				continue;

			SyncWeaponEntity( inventory , steamId , weapon , viewModel );
		}
	}

	auto OnEquipItemInLoadout( int team , int slot , uint64_t itemId ) noexcept -> void
	{
		if ( !IsManagedItem( itemId ) )
			return;

		auto* inventoryManager = CCSInventoryManager::Get();
		auto* inventory = CCSPlayerInventory::Get();
		if ( !inventoryManager || !inventory )
			return;

		auto* itemViewToEquip = inventory->GetItemViewForItem( itemId );
		auto* itemInLoadout = inventory->GetItemInLoadout( team , slot );
		if ( !itemViewToEquip || !itemInLoadout )
			return;

		auto* loadoutDefinition = itemInLoadout->GetStaticData();
		auto* equipDefinition = itemViewToEquip->GetStaticData();
		if ( !loadoutDefinition || !equipDefinition )
			return;

		const uint64_t defaultItemId = ( static_cast<uint64_t>( 0xF ) << 60 ) | itemViewToEquip->m_iItemDefinitionIndex();
		inventoryManager->EquipItemInLoadout( team , slot , defaultItemId );

		auto* loadoutSoc = itemInLoadout->GetSOCData();
		if ( !loadoutSoc )
			return;

		if ( equipDefinition->IsWeapon()
			&& !equipDefinition->IsKnife( false )
			&& !equipDefinition->IsGlove( false ) )
		{
			inventory->SOUpdated(
				inventory->GetOwner() ,
				reinterpret_cast<CSharedObject*>( loadoutSoc ) ,
				GCSDK::eSOCacheEvent_Incremental );
			return;
		}

		if ( equipDefinition->IsKnife( false ) )
		{
			inventory->SOUpdated(
				inventory->GetOwner() ,
				reinterpret_cast<CSharedObject*>( loadoutSoc ) ,
				GCSDK::eSOCacheEvent_Incremental );
		}
	}

	auto ResetRound() noexcept -> void
	{
		// Inventory items persist across rounds; FSN resync handles visuals.
	}

	auto RequestNetworkFullUpdate() noexcept -> void
	{
		s_pendingNetworkFullUpdate.store( true );
	}

	auto ProcessNetworkFullUpdate() noexcept -> void
	{
		if ( !s_pendingNetworkFullUpdate.load() )
			return;

		auto* networkService = SDK::Interfaces::NetworkClientService();
		if ( !networkService )
			return;

		if ( auto* networkClient = networkService->GetNetworkClient() )
		{
			networkClient->Update();
			s_pendingNetworkFullUpdate.store( false );
		}
	}

	auto OnPlayerDeathKillfeed( IGameEvent* event ) noexcept -> void
	{
		if ( !event || !UsesInventoryPath() )
			return;

		auto* attacker = event->GetPlayerController( "attacker" );
		auto* victim = event->GetPlayerController( "userid" );
		if ( !attacker || !victim || attacker == victim )
			return;

		auto* localController = GetCL_Players()->GetLocalPlayerController();
		if ( !localController || attacker != localController )
			return;

		auto* weapon = GetCL_Weapons()->GetLocalActiveWeapon();
		if ( !weapon )
			return;

		auto* itemView = weapon->m_AttributeManager()->m_Item();
		if ( !itemView )
			return;

		auto* definition = itemView->GetStaticData();
		if ( !definition || !definition->IsKnife( true ) )
			return;

		const char* weaponName = definition->m_pszWeaponName();
		if ( !weaponName || !weaponName[0] )
			return;

		event->SetString( "weapon" , weaponName );
	}

	auto Shutdown() noexcept -> void
	{
		if ( auto* inventory = CCSPlayerInventory::Get() )
		{
			for ( const uint64_t itemId : s_addedItemIds )
			{
				if ( auto* item = inventory->GetSOCDataForItem( itemId ) )
					inventory->RemoveEconItem( item );
			}
		}

		s_addedItemIds.clear();
	}
}

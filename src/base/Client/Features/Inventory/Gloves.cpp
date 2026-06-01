#include "Gloves.hpp"

#include <algorithm>
#include <vector>

#include <Client/Features/Inventory/CEconHelper.hpp>
#include <CS2/SDK/Cstrike15/CCSInventoryManager.hpp>
#include <CS2/SDK/Cstrike15/CCSPlayerInventory.hpp>
#include <CS2/SDK/Econ/CEconItem.hpp>
#include <CS2/SDK/Econ/CEconItemDefinition.hpp>
#include <CS2/SDK/Econ/CEconItemSystem.hpp>
#include <CS2/SDK/GCSDK/CGCClientSharedObjectCache.hpp>
#include <CS2/SDK/GCSDK/GCSDKTypes/EconItemConstants.hpp>
#include <CS2/SDK/GCSDK/GCSDKTypes/ESOCacheEvent.hpp>
#include <CS2/SDK/Interface/CSource2Client.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

#include <GameClient/CL_ItemDefinition.hpp>
#include <GameClient/CL_Players.hpp>

namespace Gloves
{
	Config config;

	namespace
	{
		struct MaterialRecord
		{
			uint32_t ui32UnknownStatic = 0;
			material_magic_number identifier = kMaterialMagicGloves;
			uint32_t ui32Handle = 0;
			uint32_t ui32TypeIndex = 0;
		};

		struct MaterialInfo
		{
			MaterialRecord* pMatRecords = nullptr;
			uint32_t ui32Count = 0;
		};

		struct GloveState
		{
			uint64_t itemId = 0;
			uint64_t itemHighId = 0;
			uint64_t itemLowId = 0;
			int itemDefId = 0;
		};

		std::vector<uint64_t> s_addedItemIds;
		int s_gloveFrame = 0;
		GloveState s_appliedGloves{};
		uint64_t s_activeItemId = 0;
		int s_lastModelIndex = -1;
		int s_lastPaintKit = -1;
		float s_lastWear = -1.f;
		int s_lastSeed = -1;
		int s_lastTeam = -1;

		auto InvalidateGloveMaterial( C_CSGOViewModel* viewModel ) noexcept -> void
		{
			if ( !viewModel )
				return;

			auto* matInfo = reinterpret_cast<MaterialInfo*>( reinterpret_cast<std::byte*>( viewModel ) + 0xF80 );
			if ( !matInfo->pMatRecords )
				return;

			for ( uint32_t i = 0; i < matInfo->ui32Count; ++i )
			{
				if ( matInfo->pMatRecords[i].identifier == kMaterialMagicGloves )
				{
					matInfo->pMatRecords[i].ui32TypeIndex = 0xFFFFFFFFu;
					break;
				}
			}
		}

		auto ResolveGloveDefinitionIndex() noexcept -> int
		{
			if ( config.modelIndex < 0 || config.modelIndex >= static_cast<int>( g_GlovesNames.size() ) )
				return 0;

			return g_GlovesNames[static_cast<size_t>( config.modelIndex )].m_WeaponDefinitionIndex;
		}

		auto ResolveGloveModelName() noexcept -> const char*
		{
			if ( config.modelIndex < 0 || config.modelIndex >= static_cast<int>( g_GlovesNames.size() ) )
				return nullptr;

			return g_GlovesNames[static_cast<size_t>( config.modelIndex )].m_pszDesc;
		}

		auto GetLoadoutSlotForDefIndex( int defIndex ) noexcept -> int
		{
			auto* source2 = SDK::Interfaces::Source2Client();
			if ( !source2 )
				return 41;

			auto* schema = source2->GetEconItemSystem()->GetEconItemSchema();
			if ( !schema )
				return 41;

			const auto definition = schema->GetSortedItemDefinitionMap().FindByKey( defIndex );
			if ( !definition )
				return 41;

			return static_cast<int>( definition->LoadoutSlot() );
		}

		auto ConfigChanged() noexcept -> bool
		{
			return s_lastModelIndex != config.modelIndex
				|| s_lastPaintKit != config.paintKit
				|| s_lastWear != config.wear
				|| s_lastSeed != config.seed
				|| s_lastTeam != config.team;
		}

		auto RememberConfig() noexcept -> void
		{
			s_lastModelIndex = config.modelIndex;
			s_lastPaintKit = config.paintKit;
			s_lastWear = config.wear;
			s_lastSeed = config.seed;
			s_lastTeam = config.team;
		}

		auto FindExistingGloveItem( int defIndex ) noexcept -> CEconItem*
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

		auto CreateGloveItem( int defIndex ) noexcept -> CEconItem*
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
			item->m_nRarity = IR_RARE;

			if ( config.paintKit > 0 )
				item->SetPaintKit( static_cast<float>( config.paintKit ) );

			item->SetPaintSeed( static_cast<float>( config.seed ) );
			item->SetPaintWear( config.wear );

			if ( !inventory->AddEconItem( item ) )
				return nullptr;

			AddEconItemId( item->m_ulID );
			return item;
		}

		auto UpdateExistingGloveItem( CEconItem* item ) noexcept -> void
		{
			if ( !item )
				return;

			if ( config.paintKit > 0 )
				item->SetPaintKit( static_cast<float>( config.paintKit ) );

			item->SetPaintSeed( static_cast<float>( config.seed ) );
			item->SetPaintWear( config.wear );
		}

		auto CaptureAppliedGloves( C_EconItemView* itemView ) noexcept -> void
		{
			if ( !itemView )
				return;

			s_appliedGloves.itemId = itemView->m_iItemID();
			s_appliedGloves.itemHighId = itemView->m_iItemIDHigh();
			s_appliedGloves.itemLowId = itemView->m_iItemIDLow();
			s_appliedGloves.itemDefId = itemView->m_iItemDefinitionIndex();
			s_activeItemId = s_appliedGloves.itemId;
		}

		auto TryApplyRuntimeGlove( CCSPlayerInventory* inventory , C_CSPlayerPawn* localPawn ) noexcept -> void
		{
			if ( !inventory || !localPawn || !localPawn->IsAlive() )
				return;

			auto* viewModelServices = localPawn->m_pViewModelServices();
			if ( !viewModelServices )
				return;

			auto* viewModel = viewModelServices->m_hViewModel().Get<C_CSGOViewModel>();
			if ( !viewModel )
				return;

			auto* glovesItem = &localPawn->m_EconGloves();
			auto* glovesDefinition = glovesItem->GetStaticData();

			if ( const char* modelName = ResolveGloveModelName() )
				C_BaseModelEntity_SetModel( viewModel , modelName );

			if ( s_appliedGloves.itemId == 0 )
				CaptureAppliedGloves( glovesItem );

			OnGlove( inventory , localPawn , viewModel , glovesItem , glovesDefinition );
			localPawn->m_bNeedToReApplyGloves() = true;
		}
	}

	auto AddEconItemId( uint64_t itemId ) noexcept -> void
	{
		if ( itemId == 0 )
			return;

		if ( std::find( s_addedItemIds.begin() , s_addedItemIds.end() , itemId ) == s_addedItemIds.end() )
			s_addedItemIds.emplace_back( itemId );
	}

	auto OnGlove(
		CCSPlayerInventory* inventory ,
		C_CSPlayerPawn* localPawn ,
		C_CSGOViewModel* viewModel ,
		C_EconItemView* glovesItem ,
		CEconItemDefinition* glovesDefinition ) noexcept -> void
	{
		if ( !config.enabled || !localPawn || !localPawn->IsAlive() || !viewModel || !glovesItem || !inventory )
			return;

		(void)glovesDefinition;

		if ( s_appliedGloves.itemId == 0 )
			return;

		if ( s_gloveFrame > 0 )
		{
			InvalidateGloveMaterial( viewModel );
			glovesItem->m_bInitialized() = true;
			localPawn->m_bNeedToReApplyGloves() = true;
			--s_gloveFrame;
		}

		if ( glovesItem->m_iItemID() != s_appliedGloves.itemId )
		{
			s_gloveFrame = 2;

			glovesItem->m_bDisallowSOC() = false;
			glovesItem->m_iItemID() = s_appliedGloves.itemId;
			glovesItem->m_iItemIDHigh() = static_cast<uint32_t>( s_appliedGloves.itemHighId );
			glovesItem->m_iItemIDLow() = static_cast<uint32_t>( s_appliedGloves.itemLowId );
			glovesItem->m_iAccountID() = static_cast<uint32_t>( inventory->GetOwner().m_id );
			glovesItem->m_iItemDefinitionIndex() = static_cast<uint16_t>( s_appliedGloves.itemDefId );
			glovesItem->m_bDisallowSOC() = false;

			if ( auto* sceneNode = viewModel->m_pGameSceneNode() )
				CGameSceneNode_SetMeshGroupMask( sceneNode , 1 );
		}
	}

	auto OnEquipItemInLoadout( int team , int slot , uint64_t itemId ) noexcept -> void
	{
		if ( !config.enabled )
			return;

		const auto it = std::find( s_addedItemIds.begin() , s_addedItemIds.end() , itemId );
		if ( it == s_addedItemIds.end() )
			return;

		auto* inventoryManager = CCSInventoryManager::Get();
		auto* inventory = CCSPlayerInventory::Get();
		if ( !inventoryManager || !inventory )
			return;

		auto* itemViewToEquip = inventory->GetItemViewForItem( itemId );
		if ( !itemViewToEquip )
			return;

		auto* itemInLoadout = inventory->GetItemInLoadout( team , slot );
		if ( !itemInLoadout )
			return;

		auto* loadoutDefinition = itemInLoadout->GetStaticData();
		auto* equipDefinition = itemViewToEquip->GetStaticData();
		if ( !loadoutDefinition || !equipDefinition || !equipDefinition->IsGlove( false ) )
			return;

		const uint64_t defaultItemId = ( static_cast<uint64_t>( 0xF ) << 60 ) | itemViewToEquip->m_iItemDefinitionIndex();
		inventoryManager->EquipItemInLoadout( team , slot , defaultItemId );

		auto* loadoutSoc = itemInLoadout->GetSOCData();
		if ( !loadoutSoc )
			return;

		CaptureAppliedGloves( itemViewToEquip );
		TryApplyRuntimeGlove( inventory , GetCL_Players()->GetLocalPlayerPawn() );
		inventory->SOUpdated( inventory->GetOwner() , reinterpret_cast<CSharedObject*>( loadoutSoc ) , GCSDK::eSOCacheEvent_Incremental );
	}

	auto ApplyFromConfig() noexcept -> bool
	{
		if ( !config.enabled )
			return false;

		const int defIndex = ResolveGloveDefinitionIndex();
		if ( defIndex <= 0 )
			return false;

		auto* inventoryManager = CCSInventoryManager::Get();
		auto* inventory = CCSPlayerInventory::Get();
		if ( !inventoryManager || !inventory )
			return false;

		CEconItem* gloveItem = FindExistingGloveItem( defIndex );
		if ( !gloveItem || ConfigChanged() )
		{
			if ( gloveItem )
			{
				inventory->RemoveEconItem( gloveItem );
				s_addedItemIds.erase(
					std::remove( s_addedItemIds.begin() , s_addedItemIds.end() , gloveItem->m_ulID ) ,
					s_addedItemIds.end() );
				gloveItem = nullptr;
				s_appliedGloves = {};
				s_activeItemId = 0;
			}

			gloveItem = CreateGloveItem( defIndex );
			if ( !gloveItem )
				return false;
		}
		else
		{
			UpdateExistingGloveItem( gloveItem );
		}

		const int loadoutSlot = GetLoadoutSlotForDefIndex( defIndex );
		const int team = config.team > 0 ? config.team : 3;

		if ( !inventoryManager->EquipItemInLoadout( team , loadoutSlot , gloveItem->m_ulID ) )
			return false;

		if ( auto* itemView = inventory->GetItemViewForItem( gloveItem->m_ulID ) )
			CaptureAppliedGloves( itemView );

		if ( auto* loadoutItem = inventory->GetItemInLoadout( team , loadoutSlot ) )
		{
			if ( auto* loadoutSoc = loadoutItem->GetSOCData() )
				inventory->SOUpdated( inventory->GetOwner() , reinterpret_cast<CSharedObject*>( loadoutSoc ) , GCSDK::eSOCacheEvent_Incremental );
		}

		TryApplyRuntimeGlove( inventory , GetCL_Players()->GetLocalPlayerPawn() );
		RememberConfig();
		s_activeItemId = gloveItem->m_ulID;
		return true;
	}

	auto ResetRound() noexcept -> void
	{
		s_gloveFrame = 0;
		s_appliedGloves = {};
		s_activeItemId = 0;
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
		ResetRound();
		s_lastModelIndex = -1;
		s_lastPaintKit = -1;
		s_lastWear = -1.f;
		s_lastSeed = -1;
		s_lastTeam = -1;
	}

	auto OnFrameStageNotify( int frameStage ) noexcept -> void
	{
		if ( frameStage != 6 || !config.enabled )
			return;

		if ( ConfigChanged() )
			ApplyFromConfig();

		auto* inventory = CCSPlayerInventory::Get();
		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !inventory || !localPawn || !localPawn->IsAlive() )
			return;

		auto* viewModelServices = localPawn->m_pViewModelServices();
		if ( !viewModelServices )
			return;

		auto* viewModel = viewModelServices->m_hViewModel().Get<C_CSGOViewModel>();
		if ( !viewModel )
			return;

		auto* glovesItem = &localPawn->m_EconGloves();
		auto* glovesDefinition = glovesItem->GetStaticData();

		if ( s_appliedGloves.itemId == 0 && s_activeItemId != 0 )
		{
			if ( auto* itemView = inventory->GetItemViewForItem( s_activeItemId ) )
				CaptureAppliedGloves( itemView );
		}

		if ( s_appliedGloves.itemId == 0 && config.modelIndex >= 0 && ResolveGloveDefinitionIndex() > 0 )
		{
			ApplyFromConfig();
			return;
		}

		if ( const char* modelName = ResolveGloveModelName() )
			C_BaseModelEntity_SetModel( viewModel , modelName );

		OnGlove( inventory , localPawn , viewModel , glovesItem , glovesDefinition );
	}
}

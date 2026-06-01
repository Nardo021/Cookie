#include "SkinChangerSetModel.hpp"

#include <Client/Features/Inventory/SkinChanger.hpp>
#include <Client/Features/Inventory/SkinChangerInventory.hpp>

#include <cstring>

#include <CS2/SDK/Cstrike15/CCSPlayerInventory.hpp>
#include <CS2/SDK/Econ/CEconItemDefinition.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

#include <GameClient/CL_Players.hpp>

namespace SkinChangerSetModel
{
	auto Resolve( C_BaseModelEntity* entity , const char* modelName ) noexcept -> const char*
	{
		if ( !entity || !modelName )
			return modelName;

		if ( auto* identity = entity->pEntityIdentity() )
		{
			const char* designer = identity->DesingerName().String();
			if ( !designer || std::strstr( designer , "viewmodel" ) == nullptr )
				return modelName;
		}
		else
		{
			return modelName;
		}

		auto* engine = SDK::Interfaces::EngineToClient();
		if ( !engine || !engine->IsInGame() )
			return modelName;

		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !localPawn || !localPawn->IsAlive() )
			return modelName;

		auto* viewModel = reinterpret_cast<C_BaseViewModel*>( entity );
		if ( !viewModel->m_hWeapon().IsValid() )
			return modelName;

		auto* weapon = viewModel->m_hWeapon().Get<C_CSWeaponBase>();
		if ( !weapon )
			return modelName;

		auto* inventory = CCSPlayerInventory::Get();
		if ( !inventory )
			return modelName;

		if ( weapon->GetOriginalOwnerXuid() != inventory->GetOwner().m_id )
			return modelName;

		auto* weaponItemView = weapon->m_AttributeManager()->m_Item();
		if ( !weaponItemView )
			return modelName;
		auto* weaponDefinition = weaponItemView->GetStaticData();
		if ( !weaponDefinition )
			return modelName;

		const int team = weapon->m_iOriginalTeamNumber();
		const int slot = static_cast<int>( weaponDefinition->LoadoutSlot() );
		auto* loadoutItemView = inventory->GetItemInLoadout( team , slot );

		if ( !loadoutItemView )
			return modelName;

		if ( !SkinChangerInventory::IsManagedItem( loadoutItemView->m_iItemID() ) )
			return modelName;

		auto* loadoutDefinition = loadoutItemView->GetStaticData();
		if ( !loadoutDefinition || !loadoutDefinition->m_pszModelName() )
			return modelName;

		return loadoutDefinition->m_pszModelName();
	}
}

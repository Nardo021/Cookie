#include "Hook_EquipItemInLoadout.hpp"

#include <Client/Features/Inventory/Gloves.hpp>
#include <Client/Features/Inventory/SkinChangerInventory.hpp>

auto Hook_EquipItemInLoadout( CCSInventoryManager* pManager , int iTeam , int iSlot , uint64_t iItemID ) -> void
{
	SkinChangerInventory::OnEquipItemInLoadout( iTeam , iSlot , iItemID );
	Gloves::OnEquipItemInLoadout( iTeam , iSlot , iItemID );
	EquipItemInLoadout_o( pManager , iTeam , iSlot , iItemID );
}

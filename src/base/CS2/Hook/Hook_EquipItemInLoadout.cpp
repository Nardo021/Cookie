#include "Hook_EquipItemInLoadout.hpp"

auto Hook_EquipItemInLoadout( CCSInventoryManager* pManager , int iTeam , int iSlot , uint64_t iItemID ) -> void
{
	EquipItemInLoadout_o( pManager , iTeam , iSlot , iItemID );
}

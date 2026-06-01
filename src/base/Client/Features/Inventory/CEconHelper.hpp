#pragma once

#include <CS2/SDK/Cstrike15/CCSInventoryManager.hpp>
#include <CS2/SDK/Cstrike15/CCSPlayerInventory.hpp>
#include <CS2/SDK/Econ/CEconItem.hpp>

class CCSPlayerController;

namespace CEconHelper
{
	inline auto GetLocalInventory() -> CCSPlayerInventory*
	{
		auto* manager = CCSInventoryManager::Get();
		if ( !manager )
			return nullptr;

		return manager->GetLocalInventory();
	}

	inline auto CreateItem( uint64_t itemId , uint16_t defIndex ) -> CEconItem*
	{
		auto* item = CEconItem::Create();
		if ( !item )
			return nullptr;

		item->m_ulID = itemId;
		item->m_unDefIndex = defIndex;
		item->m_unAccountID = static_cast<uint32_t>( itemId );
		return item;
	}

	inline auto AddToInventory( CEconItem* item ) -> bool
	{
		auto* inventory = GetLocalInventory();
		return inventory && item && inventory->AddEconItem( item );
	}
}

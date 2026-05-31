#include "Hook_SOCacheSubscribed.hpp"

#include <CS2/SDK/Cstrike15/CCSPlayerInventory.hpp>

auto Hook_SOCacheSubscribed( CCSPlayerInventory* pCSPlayerInventory , GCSDK::SOID_t owner , int64_t unk ) -> void*
{
	return SOCacheSubscribed_o( pCSPlayerInventory , owner , unk );
}

#include "Hook_OnRemoveEntity.hpp"

#include <GameClient/CEntityCache/CEntityCache.hpp>

auto Hook_OnRemoveEntity( CGameEntitySystem* pCGameEntitySystem , CEntityInstance* pInst , CHandle handle ) -> void
{
	GetEntityCache()->OnRemoveEntity( pInst , handle );

	return OnRemoveEntity_o( pCGameEntitySystem , pInst , handle );
}

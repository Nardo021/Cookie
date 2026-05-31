#include "Hook_OnAddEntity.hpp"

#include <GameClient/CEntityCache/CEntityCache.hpp>

auto Hook_OnAddEntity( CGameEntitySystem* pCGameEntitySystem , CEntityInstance* pInst , CHandle handle ) -> void
{
	GetEntityCache()->OnAddEntity( pInst , handle );

	return OnAddEntity_o( pCGameEntitySystem , pInst , handle );
}

#include "Hook_OnRemoveEntity.hpp"

#include <Client/Features/Combat/LagComp.hpp>
#include <GameClient/CEntityCache/CEntityCache.hpp>

auto Hook_OnRemoveEntity( CGameEntitySystem* pCGameEntitySystem , CEntityInstance* pInst , CHandle handle ) -> void
{
	GetEntityCache()->OnRemoveEntity( pInst , handle );

	if ( handle.IsValid() )
		LagComp::RemoveRecordsForHandle( handle.m_Index );

	return OnRemoveEntity_o( pCGameEntitySystem , pInst , handle );
}

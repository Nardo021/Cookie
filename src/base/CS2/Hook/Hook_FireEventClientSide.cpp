#include "Hook_FireEventClientSide.hpp"

auto Hook_FireEventClientSide( IGameEventManager2* pGameEventManager2 , IGameEvent* pGameEvent ) -> bool
{
	return FireEventClientSide_o( pGameEventManager2 , pGameEvent );
}

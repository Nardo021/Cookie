#include "Hook_FireEventClientSide.hpp"

#include <cstring>

#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/Interface/IGameEvent.hpp>

#include <Client/Features/Events/GameEvents.hpp>
#include <Client/Utils/CNotify.hpp>

auto Hook_FireEventClientSide( IGameEventManager2* pGameEventManager2 , IGameEvent* pGameEvent ) -> bool
{
	if ( pGameEvent )
	{
		const char* name = IGameEvent_GetName( pGameEvent );
		if ( name )
		{
			GameEvents::Dispatch( pGameEvent );

			if ( strcmp( name , "player_death" ) == 0 )
				GetNotify()->Push( N_TYPE_INFO , "Player eliminated" );
			else if ( strcmp( name , "bomb_planted" ) == 0 )
				GetNotify()->Push( N_TYPE_WARNING , "Bomb planted" );
			else if ( strcmp( name , "bomb_defused" ) == 0 )
				GetNotify()->Push( N_TYPE_SUCCESS , "Bomb defused" );
		}
	}

	return FireEventClientSide_o( pGameEventManager2 , pGameEvent );
}

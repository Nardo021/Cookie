#include "Hook_LevelInit.hpp"

#include <Client/Features/Visuals/Chams.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/Utils/CNotify.hpp>

auto Hook_LevelInit( void* clientMode , const char* mapName ) -> void*
{
	LagComp::Clear();
	Chams::Init();

	if ( mapName )
		GetNotify()->Push( N_TYPE_INFO , "Map: %s" , mapName );

	return LevelInit_o( clientMode , mapName );
}

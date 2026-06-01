#include "Hook_LevelShutdown.hpp"

#include <Client/Features/Visuals/Chams.hpp>
#include <Client/Features/Combat/LagComp.hpp>

auto Hook_LevelShutdown( void* clientMode ) -> void
{
	LagComp::Clear();
	Chams::Shutdown();
	LevelShutdown_o( clientMode );
}

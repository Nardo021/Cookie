#include "Hook_HandleGameEvents.hpp"

#include <CS2/SDK/Interface/IGameEvent.hpp>

#include <Client/Features/Events/GameEvents.hpp>

auto Hook_HandleGameEvents( void* rcx , IGameEvent* event ) -> void
{
	GameEvents::Dispatch( event );
	HandleGameEvents_o( rcx , event );
}

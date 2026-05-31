#pragma once

#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>

inline auto GetCL_Input( CCSGOInput* input = nullptr ) -> CCSGOInput*
{
	if ( input )
		return input;

	if ( !Game::clientBase )
		return nullptr;

	return Game::Read<CCSGOInput*>( Game::clientBase + Offsets::dwCSGOInput );
}

#include "GameEvents.hpp"

#include <cstring>

#include <CS2/SDK/Interface/IGameEvent.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/SDK.hpp>

#include <Client/Features/Inventory/Gloves.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/Features/Combat/Legit/LegitBot.hpp>
#include <Client/Features/Combat/Rage/RageSubTick.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Inventory/SkinChanger.hpp>
#include <Client/Features/Inventory/SkinChangerInventory.hpp>

#include <GameClient/CL_Players.hpp>

namespace GameEvents
{
	auto OnRoundStart() noexcept -> void
	{
		Gloves::ResetRound();
		LagComp::Clear();
		RageSubTick::ResetTick();
		Ragebot::OnRoundStart();
		LegitBot::OnRoundStart();
		SkinChanger::forceUpdate.store( true );
		SkinChangerInventory::SyncFromConfig();
		SkinChangerInventory::RequestNetworkFullUpdate();
	}

	auto OnPlayerDeath( IGameEvent* event ) noexcept -> void
	{
		if ( !event )
			return;

		SkinChangerInventory::OnPlayerDeathKillfeed( event );
		LegitBot::OnPlayerDeath( event );
		Ragebot::OnPlayerDeath();
		RageSubTick::ResetTick();
	}

	auto Dispatch( IGameEvent* event ) noexcept -> void
	{
		if ( !event )
			return;

		auto* engine = SDK::Interfaces::EngineToClient();
		if ( !engine || !engine->IsInGame() )
			return;

		auto* pawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !pawn || !pawn->IsAlive() )
			return;

		const char* name = IGameEvent_GetName( event );
		if ( !name )
			return;

		if ( strcmp( name , "round_start" ) == 0 )
			OnRoundStart();
		else if ( strcmp( name , "player_death" ) == 0 )
			OnPlayerDeath( event );
	}
}

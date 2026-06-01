#include "EnginePred.hpp"

#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <GameClient/CL_Players.hpp>

namespace EnginePred
{
	namespace
	{
		float s_savedCurrentTime = 0.f;
		float s_savedCurrentTime2 = 0.f;
		float s_savedFrameTime = 0.f;
		float s_savedFrameTime2 = 0.f;
		int32_t s_savedTickCount = 0;
		int s_predictedTick = 0;
		uint32_t s_predictedFlags = 0;
		int s_depth = 0;
	}

	auto Start( CUserCmd* cmd ) noexcept -> void
	{
		(void)cmd;

		auto* controller = GetCL_Players()->GetLocalPlayerController();
		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		auto* globalVars = SDK::Pointers::GlobalVarsBase();

		if ( !controller || !localPawn || !globalVars )
			return;

		if ( s_depth++ > 0 )
			return;

		s_savedCurrentTime = globalVars->m_flCurrentTime();
		s_savedCurrentTime2 = globalVars->m_flCurrentTime2();
		s_savedFrameTime = globalVars->m_flFrameTime();
		s_savedFrameTime2 = globalVars->m_flFrameTime2();
		s_savedTickCount = globalVars->m_nTickCount();

		const float interval = globalVars->m_flIntervalPerSubTick();
		const float predictedTime = static_cast<float>( controller->m_nTickBase() ) * interval;

		globalVars->m_flCurrentTime() = predictedTime;
		globalVars->m_flCurrentTime2() = predictedTime;
		globalVars->m_flFrameTime() = interval;
		globalVars->m_flFrameTime2() = interval;
		globalVars->m_nTickCount() = controller->m_nTickBase();
		s_predictedTick = controller->m_nTickBase();

		if ( auto* predicted = controller->m_hPredictedPawn().Get<C_CSPlayerPawn>() )
			s_predictedFlags = predicted->m_fFlags();
		else
			s_predictedFlags = localPawn->m_fFlags();
	}

	auto End() noexcept -> void
	{
		if ( s_depth <= 0 )
			return;

		if ( --s_depth > 0 )
			return;

		if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
		{
			globalVars->m_flCurrentTime() = s_savedCurrentTime;
			globalVars->m_flCurrentTime2() = s_savedCurrentTime2;
			globalVars->m_flFrameTime() = s_savedFrameTime;
			globalVars->m_flFrameTime2() = s_savedFrameTime2;
			globalVars->m_nTickCount() = s_savedTickCount;
		}
	}

	auto IsActive() noexcept -> bool
	{
		return s_depth > 0;
	}

	auto GetPredictedTick() noexcept -> int
	{
		return s_predictedTick;
	}

	auto GetPredictedFlags() noexcept -> uint32_t
	{
		return s_predictedFlags;
	}

	auto GetPawnFlags( C_CSPlayerPawn* pawn ) noexcept -> uint32_t
	{
		if ( IsActive() )
			return GetPredictedFlags();

		return pawn ? pawn->m_fFlags() : 0;
	}

	auto IsOnGround( C_CSPlayerPawn* pawn ) noexcept -> bool
	{
		return ( GetPawnFlags( pawn ) & FL_ONGROUND ) != 0;
	}
}

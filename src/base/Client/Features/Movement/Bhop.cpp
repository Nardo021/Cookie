#include "Bhop.hpp"

#include <cmath>
#include <cstdint>

#include <buttons.hpp>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Interface/IEngineCvar.hpp>

#include <Client/Core/CConVars.hpp>
#include <Client/Features/Combat/EnginePred.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Game/Trace.hpp>

#include <GameClient/CL_Bypass.hpp>
#include <GameClient/CL_Players.hpp>

namespace Bhop
{
	BhopConfig config;

	namespace
	{
		bool s_wasOnGround = false;

		constexpr float kMoveSpeed = 1.f;
		constexpr float kIntervalPerTick = 1.0f / 64.0f;
		constexpr float kDefaultStandableNormal = 0.7f;
		constexpr float kSubtickGroundPressWhen = 0.99f;

		auto IsServerAutoBhop() noexcept -> bool
		{
			if ( auto* cv = CookieCore::CConVars::sv_autobunnyhopping )
				return cv->value.i1 != 0;

			return false;
		}

		auto ShouldBhop( CUserCmd* cmd ) noexcept -> bool
		{
			if ( !config.enabled || !cmd || IsServerAutoBhop() )
				return false;

			const bool jumpHeld = ( cmd->button_states.buttonstate1 & IN_JUMP ) != 0;
			if ( config.requireSpace )
				return jumpHeld;

			return true;
		}

		auto GetStandableNormal() noexcept -> float
		{
			if ( auto* cv = CookieCore::CConVars::sv_standable_normal )
				return cv->value.fl;

			return kDefaultStandableNormal;
		}

		auto IsStandableGround( float fraction , float normalZ , float standableNormal ) noexcept -> bool
		{
			if ( fraction <= 0.f || fraction >= 1.f )
				return false;

			return normalZ >= standableNormal;
		}

		auto TryJumpBug(
			CUserCmd* cmd ,
			C_CSPlayerPawn* pawn ,
			bool onGround ,
			uint32_t pawnHandle ) noexcept -> bool
		{
			if ( !config.jumpBug || !cmd || !pawn )
				return false;

			if ( ( cmd->button_states.buttonstate1 & IN_JUMP ) == 0 )
				return false;

			const Vector3 velocity = pawn->m_vecAbsVelocity();
			const Vector3 origin = pawn->GetOrigin();

			Vector3 vStart = origin;
			Vector3 vEnd = origin;
			vEnd.m_x += velocity.m_x * kIntervalPerTick;
			vEnd.m_y += velocity.m_y * kIntervalPerTick;
			vEnd.m_z += velocity.m_z * kIntervalPerTick - 2.f;

			float fraction = 1.f;
			float normalZ = 0.f;

			const bool traced = Trace::TraceSegment(
				{ vStart.m_x , vStart.m_y , vStart.m_z } ,
				{ vEnd.m_x , vEnd.m_y , vEnd.m_z } ,
				reinterpret_cast<uintptr_t>( pawn ) ,
				pawnHandle ,
				fraction ,
				normalZ );

			const float standable = GetStandableNormal();
			const bool validGround = traced && IsStandableGround( fraction , normalZ , standable );

			if ( !onGround && !validGround )
				return false;

			const float landWhen = validGround ? fraction : kSubtickGroundPressWhen;
			GetCL_Bypass()->ApplyJumpBug( cmd , landWhen );
			return true;
		}

		auto TryEdgeJump( CUserCmd* cmd , C_CSPlayerPawn* pawn , uint32_t pawnHandle ) noexcept -> void
		{
			if ( !config.edgeJump || !cmd || !pawn )
				return;

			const Vector3 origin = pawn->GetOrigin();
			float fraction = 1.f;

			const bool hit = Trace::TraceGround(
				{ origin.m_x , origin.m_y , origin.m_z + 5.f } ,
				{ origin.m_x , origin.m_y , origin.m_z - 50.f } ,
				reinterpret_cast<uintptr_t>( pawn ) ,
				pawnHandle ,
				fraction );

			if ( !hit || fraction > Trace::kEdgeJumpMaxGroundFraction )
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , true , false );
		}

		auto RunBhop( CUserCmd* cmd , bool onGround , bool active ) noexcept -> void
		{
			if ( !cmd || !active )
				return;

			if ( config.hopChance < 100 )
			{
				const int roll = static_cast<int>( GetTickCount64() % 100 );
				if ( roll >= config.hopChance )
				{
					GetCL_Bypass()->SetButton( cmd , IN_JUMP , false , false );
					s_wasOnGround = onGround;
					return;
				}
			}

			// Online-safe bhop: only manipulate usercmd buttons, never +jump force memory.
			if ( onGround )
			{
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , false , false );

				if ( !config.requireSpace )
					GetCL_Bypass()->SetButton( cmd , IN_JUMP , true , false );
			}
			else if ( !s_wasOnGround )
			{
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , false , false );
			}
			else
			{
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , false , false );
			}

			s_wasOnGround = onGround;
		}

		auto ApplyAutoForward( CUserCmd* cmd , bool onGround ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !cmd || !config.autoForward )
				return;

			const bool keyW = ( GetAsyncKeyState( 'W' ) & 0x8000 ) != 0;
			const bool keyS = ( GetAsyncKeyState( 'S' ) & 0x8000 ) != 0;
			if ( keyW || keyS )
				return;

			GetCL_Bypass()->SetButton( cmd , IN_FORWARD , true );

			auto* base = cmd->cmd.mutable_base();
			if ( onGround || base->forwardmove() == 0.f )
				base->set_forwardmove( kMoveSpeed );
#endif
		}

	}

	auto Process( CCSGOInput* input , CUserCmd* cmd ) noexcept -> void
	{
		if ( !cmd )
			return;

		if ( !config.enabled && !config.edgeJump )
		{
			s_wasOnGround = false;
			return;
		}

		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !localPawn || !localPawn->IsAlive() )
		{
			s_wasOnGround = false;
			return;
		}

		const auto moveType = localPawn->m_MoveType();
		if ( moveType == MOVETYPE_NOCLIP || moveType == MOVETYPE_LADDER || moveType == MOVETYPE_OBSERVER )
		{
			s_wasOnGround = false;
			return;
		}

		const bool onGround = EnginePred::IsOnGround( localPawn );
		const bool bhopActive = ShouldBhop( cmd );

		uint32_t pawnHandle = 0;
		if ( auto* controller = GetCL_Players()->GetLocalPlayerController() )
			pawnHandle = Game::Read<uint32_t>( reinterpret_cast<uintptr_t>( controller ) + Offsets::m_hPlayerPawn );

		if ( config.edgeJump && onGround )
			TryEdgeJump( cmd , localPawn , pawnHandle );

		if ( config.enabled )
		{
			ApplyAutoForward( cmd , onGround );

			if ( !TryJumpBug( cmd , localPawn , onGround , pawnHandle ) )
				RunBhop( cmd , onGround , bhopActive );
		}
		else
			s_wasOnGround = onGround;
	}
}

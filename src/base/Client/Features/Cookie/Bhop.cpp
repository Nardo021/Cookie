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
		bool s_forceJumpHeld = false;

		constexpr float kMoveSpeed = 1.f;
		constexpr float kIntervalPerTick = 1.0f / 64.0f;
		constexpr float kDefaultStandableNormal = 0.7f;
		constexpr float kSubtickGroundPressWhen = 0.99f;
		constexpr float kSubtickAirReleaseWhen = 0.f;
		constexpr uint32_t kForceButtonPress = 65537u;
		constexpr uint32_t kForceButtonRelease = 256u;

		auto ShouldBhop() noexcept -> bool
		{
			if ( !config.enabled )
				return false;

			if ( config.requireSpace )
				return ( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) != 0;

			return true;
		}

		auto ApplyForceJump( bool pressed ) noexcept -> void
		{
			if ( !Game::clientBase )
				return;

			const uintptr_t forceJump = Game::clientBase + cs2_dumper::buttons::jump;

			if ( pressed )
			{
				Game::Write<uint32_t>( forceJump , kForceButtonPress );
				s_forceJumpHeld = true;
			}
			else if ( s_forceJumpHeld )
			{
				Game::Write<uint32_t>( forceJump , kForceButtonRelease );
				s_forceJumpHeld = false;
			}
		}

		auto GetStandableNormal() noexcept -> float
		{
			static float value = kDefaultStandableNormal;
			static bool queried = false;

			if ( !queried )
			{
				queried = true;
				if ( auto* cvar = SDK::Interfaces::EngineCvar() )
				{
					if ( auto* sv = cvar->Find( "sv_standable_normal" ) )
						value = sv->value.fl;
				}
			}

			return value;
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
			ApplyForceJump( false );
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
			{
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , true , true , kSubtickGroundPressWhen );
				ApplyForceJump( true );
			}
		}

		auto RunBhop( CUserCmd* cmd , bool onGround , bool active ) noexcept -> void
		{
			if ( !cmd || !active )
			{
				ApplyForceJump( false );
				return;
			}

			const bool jumpInCmd = ( cmd->button_states.buttonstate1 & IN_JUMP ) != 0;

			if ( config.requireSpace && jumpInCmd && !onGround )
			{
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , false , true , kSubtickAirReleaseWhen );
				ApplyForceJump( false );
				return;
			}

			if ( onGround )
			{
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , true , true , kSubtickGroundPressWhen );
				ApplyForceJump( true );
			}
			else
			{
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , false , true , kSubtickAirReleaseWhen );
				ApplyForceJump( false );
			}
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

		auto AutoStrafe( CCSGOInput* input , CUserCmd* cmd , bool onGround ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !config.autoStrafe || !cmd || onGround )
				return;

			auto* base = cmd->cmd.mutable_base();
			if ( !base )
				return;

			float yawDelta = 0.f;

			if ( input )
			{
				static float s_lastYaw = 0.f;
				const QAngle view = *CCSGOInput_GetViewAngles( input , 0 );
				yawDelta = Math::AngleNormalize( view.m_y - s_lastYaw );
				s_lastYaw = view.m_y;
			}

			const bool keyA = ( GetAsyncKeyState( 'A' ) & 0x8000 ) != 0;
			const bool keyD = ( GetAsyncKeyState( 'D' ) & 0x8000 ) != 0;
			const bool keyW = ( GetAsyncKeyState( 'W' ) & 0x8000 ) != 0;

			GetCL_Bypass()->AddSubtickStrafeStep( cmd , yawDelta , 0.f , 1.f );

			float leftMove = 0.f;
			if ( keyA )
				leftMove = kMoveSpeed;
			else if ( keyD )
				leftMove = -kMoveSpeed;
			else if ( std::fabs( yawDelta ) > 0.01f )
				leftMove = yawDelta > 0.f ? kMoveSpeed : -kMoveSpeed;
			else
			{
				static bool flip = false;
				flip = !flip;
				leftMove = flip ? kMoveSpeed : -kMoveSpeed;
			}

			if ( keyW )
				leftMove = -leftMove;

			base->set_leftmove( leftMove );

			if ( !config.autoForward && !keyW )
				base->set_forwardmove( 0.f );
#endif
		}
	}

	auto Process( CCSGOInput* input , CUserCmd* cmd ) noexcept -> void
	{
		if ( !cmd )
			return;

		if ( !config.enabled && !config.autoStrafe && !config.edgeJump )
			return;

		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !localPawn || !localPawn->IsAlive() )
		{
			ApplyForceJump( false );
			return;
		}

		const auto moveType = localPawn->m_MoveType();
		if ( moveType == MOVETYPE_NOCLIP || moveType == MOVETYPE_LADDER || moveType == MOVETYPE_OBSERVER )
		{
			ApplyForceJump( false );
			return;
		}

		const bool onGround = ( localPawn->m_fFlags() & FL_ONGROUND ) != 0;
		const bool bhopActive = ShouldBhop();

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
		{
			ApplyForceJump( false );
		}

		AutoStrafe( input , cmd , onGround );
	}
}

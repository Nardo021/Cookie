#pragma once

#include <Windows.h>
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

#include <Client/Game/Game.hpp>

#include <GameClient/CL_Bypass.hpp>
#include <GameClient/CL_Players.hpp>

namespace Bhop
{
	// CreateMove + CUserCmd (UC standard). buttons::jump replaces legacy dwForceJump.
	struct BhopConfig
	{
		bool enabled      = false;
		bool autoStrafe   = false;
		bool autoForward  = true;
		bool requireSpace = false; // false = fully automatic, true = hold VK_SPACE
	};

	inline BhopConfig config;

	inline bool s_forceJumpHeld = false;

	inline constexpr float kMoveSpeed            = 1.f;
	inline constexpr float kSubtickGroundPressWhen = 0.99f;
	inline constexpr float kSubtickAirReleaseWhen  = 0.f;

	// Same pulse pattern as Triggerbot +attack / -attack.
	inline constexpr uint32_t kForceButtonPress   = 65537u;
	inline constexpr uint32_t kForceButtonRelease = 256u;

	inline bool ShouldBhop() noexcept
	{
		if ( !config.enabled )
			return false;

		if ( config.requireSpace )
			return ( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) != 0;

		return true;
	}

	inline void SetMoveButtonState( CUserCmd* cmd , uint64_t button , bool pressed ) noexcept
	{
		if ( !cmd )
			return;

		if ( pressed )
		{
			cmd->button_states.buttonstate1 |= button;
			cmd->button_states.buttonstate2 |= button;
			cmd->button_states.buttonstate3 |= button;
		}
		else
		{
			cmd->button_states.buttonstate1 &= ~button;
			cmd->button_states.buttonstate2 &= ~button;
			cmd->button_states.buttonstate3 &= ~button;
		}

#if DISABLE_PROTOBUF == 0
		auto* buttonsPb = cmd->cmd.mutable_base()->mutable_buttons_pb();
		buttonsPb->set_buttonstate1( cmd->button_states.buttonstate1 );
		buttonsPb->set_buttonstate2( cmd->button_states.buttonstate2 );
		buttonsPb->set_buttonstate3( cmd->button_states.buttonstate3 );
#endif
	}

	inline void ApplyForceJump( bool pressed ) noexcept
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

	inline void RunBhop( CUserCmd* cmd , bool onGround , bool active ) noexcept
	{
		if ( !cmd || !active )
		{
			ApplyForceJump( false );
			return;
		}

		const bool jumpInCmd = ( cmd->button_states.buttonstate1 & IN_JUMP ) != 0;

		// Hold-space (UC): strip jump while airborne so the next landing can re-trigger.
		if ( config.requireSpace && jumpInCmd && !onGround )
		{
			GetCL_Bypass()->SetJump( cmd , false , true , kSubtickAirReleaseWhen );
			ApplyForceJump( false );
			return;
		}

		if ( onGround )
		{
			GetCL_Bypass()->SetJump( cmd , true , true , kSubtickGroundPressWhen );
			ApplyForceJump( true );
		}
		else
		{
			GetCL_Bypass()->SetJump( cmd , false , true , kSubtickAirReleaseWhen );
			ApplyForceJump( false );
		}
	}

	inline void ApplyAutoForward( CUserCmd* cmd , bool onGround ) noexcept
	{
#if DISABLE_PROTOBUF == 0
		if ( !cmd || !config.autoForward )
			return;

		const bool keyW = ( GetAsyncKeyState( 'W' ) & 0x8000 ) != 0;
		const bool keyS = ( GetAsyncKeyState( 'S' ) & 0x8000 ) != 0;
		if ( keyW || keyS )
			return;

		SetMoveButtonState( cmd , IN_FORWARD , true );

		auto* base = cmd->cmd.mutable_base();
		if ( onGround || base->forwardmove() == 0.f )
			base->set_forwardmove( kMoveSpeed );
#endif
	}

	inline void AutoStrafe( CCSGOInput* input , CUserCmd* cmd , bool onGround ) noexcept
	{
#if DISABLE_PROTOBUF == 0
		if ( !config.autoStrafe || !cmd || onGround )
			return;

		auto* base = cmd->cmd.mutable_base();
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

	inline void Process( CCSGOInput* input , CUserCmd* cmd ) noexcept
	{
		if ( !cmd )
			return;

		if ( !config.enabled && !config.autoStrafe )
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

		if ( config.enabled )
		{
			ApplyAutoForward( cmd , onGround );
			RunBhop( cmd , onGround , bhopActive );
		}
		else
		{
			ApplyForceJump( false );
		}

		AutoStrafe( input , cmd , onGround );
	}
}

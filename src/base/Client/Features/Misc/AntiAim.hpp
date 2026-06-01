#pragma once

#include <cmath>
#include <cstdint>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>

#include <Client/Features/Movement/Movement.hpp>
#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace AntiAim
{
	enum class Mode : int
	{
		Static = 0 ,
		Spin   = 1 ,
	};

	enum class PitchType : int
	{
		None = 0 ,
		Down ,
		Up ,
		Zero ,
	};

	enum class YawType : int
	{
		None = 0 ,
		Backwards ,
		Forwards ,
	};

	inline constexpr float kPitchDefault      = 89.f;
	inline constexpr float kStaticYawDefault  = 180.f;
	inline constexpr float kSpinSpeedDefault  = 50.f;

	struct Config
	{
		bool  enabled   = false;
		int   mode      = static_cast<int>( Mode::Static );
		int   pitchType = static_cast<int>( PitchType::Down );
		int   yawType   = static_cast<int>( YawType::Backwards );
		float pitch     = kPitchDefault;
		float yaw       = kStaticYawDefault;
		float spinSpeed = kSpinSpeedDefault;
	};

	inline Config config;
	inline float  spinYaw = 0.f;

	inline float s_realYaw     = 0.f;
	inline float s_fakeYaw     = 0.f;
	inline bool  s_fixMovement = false;

	inline float GetMovementRealYaw( CCSGOInput* input ) noexcept
	{
		if ( input )
		{
			const QAngle realAngles = *CCSGOInput_GetViewAngles( input , 0 );
			return realAngles.m_y;
		}

		return s_realYaw;
	}

	inline float ComputeFakeYaw( float realYaw ) noexcept
	{
		if ( config.mode == static_cast<int>( Mode::Static ) )
		{
			float offset = config.yaw;
			switch ( static_cast<YawType>( config.yawType ) )
			{
			case YawType::Backwards: offset = -180.f; break;
			case YawType::Forwards: offset = 180.f; break;
			default: break;
			}
			return Math::AngleNormalize( realYaw + offset );
		}

		spinYaw += config.spinSpeed;
		spinYaw = Math::AngleNormalize( spinYaw );
		return Math::AngleNormalize( realYaw + spinYaw );
	}

	inline float ComputePitch() noexcept
	{
		switch ( static_cast<PitchType>( config.pitchType ) )
		{
		case PitchType::Down: return 89.f;
		case PitchType::Up: return -89.f;
		case PitchType::Zero: return 0.f;
		default: return config.pitch;
		}
	}

	// WASD = camera-relative intent; fall back to cmd move when auto-strafe etc. sets them.
	inline void GetMoveInput( CUserCmd* cmd , float& forward , float& left ) noexcept
	{
		forward = 0.f;
		left = 0.f;

#if DISABLE_PROTOBUF == 0
		if ( !cmd )
			return;

		const float cmdForward = cmd->cmd.base().forwardmove();
		const float cmdLeft = cmd->cmd.base().leftmove();

		const uint64_t buttons = cmd->button_states.buttonstate1;
		if ( buttons & IN_FORWARD )
			forward += 1.f;
		if ( buttons & IN_BACK )
			forward -= 1.f;
		if ( buttons & IN_MOVELEFT )
			left += 1.f;
		if ( buttons & IN_MOVERIGHT )
			left -= 1.f;

		if ( forward == 0.f && left == 0.f )
		{
			forward = cmdForward;
			left = cmdLeft;
			return;
		}

		float magnitude = sqrtf( cmdForward * cmdForward + cmdLeft * cmdLeft );
		if ( magnitude < 0.01f )
			magnitude = 1.f;

		const float dirLen = sqrtf( forward * forward + left * left );
		if ( dirLen > 0.01f )
		{
			forward = ( forward / dirLen ) * magnitude;
			left = ( left / dirLen ) * magnitude;
		}
#endif
	}

	inline void FixMovement( CUserCmd* cmd , float realYaw , float fakeYaw ) noexcept
	{
#if DISABLE_PROTOBUF == 0
		if ( !cmd )
			return;

		float forward = 0.f;
		float left = 0.f;
		GetMoveInput( cmd , forward , left );

		if ( forward == 0.f && left == 0.f )
			return;

		realYaw = Math::AngleNormalize( realYaw );
		fakeYaw = Math::AngleNormalize( fakeYaw );

		const float delta = DEG2RAD( realYaw - fakeYaw );
		const float cosDelta = cosf( delta );
		const float sinDelta = sinf( delta );

		const float fixedForward = forward * cosDelta + left * sinDelta;
		const float fixedLeft = left * cosDelta - forward * sinDelta;

		cmd->cmd.mutable_base()->set_forwardmove( fixedForward );
		cmd->cmd.mutable_base()->set_leftmove( fixedLeft );
#endif
	}

	inline void ApplyFakeViewAngles( CUserCmd* cmd , float pitch , float fakeYaw ) noexcept
	{
#if DISABLE_PROTOBUF == 0
		if ( !cmd )
			return;

		auto* viewAngles = cmd->cmd.mutable_base()->mutable_viewangles();
		viewAngles->set_x( Math::AngleNormalize( pitch ) );
		viewAngles->set_y( Math::AngleNormalize( fakeYaw ) );

		for ( int i = 0; i < cmd->cmd.input_history_size(); ++i )
		{
			auto* history = cmd->cmd.mutable_input_history( i );
			if ( !history )
				continue;

			history->mutable_view_angles()->set_x( Math::AngleNormalize( pitch ) );
			history->mutable_view_angles()->set_y( Math::AngleNormalize( fakeYaw ) );
		}
#endif
	}

	inline void CancelMovementFix() noexcept
	{
		s_fixMovement = false;
	}

	inline bool ShouldRun( CUserCmd* cmd ) noexcept
	{
		return config.enabled && cmd != nullptr;
	}

	inline void Process( CCSGOInput* input , CUserCmd* cmd ) noexcept
	{
#if DISABLE_PROTOBUF == 0
		s_fixMovement = false;

		if ( !ShouldRun( cmd ) || !input )
			return;

		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !localPawn || !localPawn->IsAlive() )
			return;

		const auto moveType = localPawn->m_MoveType();
		if ( moveType == MOVETYPE_NOCLIP || moveType == MOVETYPE_LADDER || moveType == MOVETYPE_OBSERVER )
			return;

		const uint64_t buttons = cmd->button_states.buttonstate1;
		if ( buttons & IN_ATTACK || buttons & IN_USE || buttons & IN_ATTACK2 )
			return;

		if ( GetCL_Weapons()->IsLocalThrowingGrenade() )
			return;

		static int s_lastMode = -1;
		if ( s_lastMode != config.mode )
		{
			spinYaw = 0.f;
			s_lastMode = config.mode;
		}

		const QAngle realAngles = *CCSGOInput_GetViewAngles( input , 0 );
		s_realYaw = realAngles.m_y;
		s_fakeYaw = ComputeFakeYaw( s_realYaw );

		ApplyFakeViewAngles( cmd , ComputePitch() , s_fakeYaw );

		s_fixMovement = true;
#else
		(void)input;
		(void)cmd;
#endif
	}

	inline void FixMovementPost( CCSGOInput* input , CUserCmd* cmd ) noexcept
	{
#if DISABLE_PROTOBUF == 0
		if ( !s_fixMovement || !cmd || !Movement::config.movementFix )
			return;

		if ( input )
			s_realYaw = GetMovementRealYaw( input );

		Movement::MovementFix( cmd , s_realYaw , s_fakeYaw );
#endif
	}

	void InitHook();
	void ShutdownHook();
}

#include "Movement.hpp"

#include <array>
#include <cmath>

#include <buttons.hpp>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/Protobuf/cs_usercmd.pb.h>
#include <CS2/Protobuf/usercmd.pb.h>
#include <CS2/SDK/FunctionListSDK.hpp>

#include <Client/Core/CConVars.hpp>
#include <Client/Features/Misc/AntiAim.hpp>
#include <Client/Features/Movement/Bhop.hpp>
#include <Client/Features/Combat/EnginePred.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Trace.hpp>

#include <Client/Utils/KeyBindUtils.hpp>
#include <GameClient/CL_Bypass.hpp>
#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace Movement
{
	namespace
	{
		constexpr float kPi = 3.14159265358979323846f;
		constexpr float kMoveClamp = 1.f;
		constexpr float kWaterWaist = 2.f;

		auto ClampMove( float value ) noexcept -> float
		{
			if ( value > kMoveClamp )
				return kMoveClamp;
			if ( value < -kMoveClamp )
				return -kMoveClamp;
			return value;
		}

		auto GetPawnFlags( C_CSPlayerPawn* pawn ) noexcept -> uint32_t
		{
			return EnginePred::GetPawnFlags( pawn );
		}

		auto IsOnGround( C_CSPlayerPawn* pawn ) noexcept -> bool
		{
			return EnginePred::IsOnGround( pawn );
		}

		auto ShouldSkipMovement( C_CSPlayerPawn* pawn ) noexcept -> bool
		{
			if ( !pawn || !pawn->IsAlive() )
				return true;

			const auto moveType = pawn->m_MoveType();
			if ( moveType == MOVETYPE_NOCLIP || moveType == MOVETYPE_LADDER || moveType == MOVETYPE_OBSERVER )
				return true;

			return pawn->m_flWaterLevel() >= kWaterWaist;
		}

		auto NormalizeYaw( float yaw ) noexcept -> float
		{
			return Math::AngleNormalize( yaw );
		}

		auto AngleDirections( const QAngle& angles , Vector3& forward , Vector3& right , Vector3& up ) noexcept -> void
		{
			Math::AngleVectors( angles , forward , right , up );
		}

		auto FlattenDirection( Vector3& forward , Vector3& right , Vector3& up ) noexcept -> void
		{
			forward.m_z = 0.f;
			right.m_z = 0.f;
			up.m_x = 0.f;
			up.m_y = 0.f;

			if ( forward.Length2D() > 0.001f )
				forward.Normalize();
			if ( right.Length2D() > 0.001f )
				right.Normalize();
			if ( std::fabs( up.m_z ) > 0.001f )
				up.m_z = up.m_z > 0.f ? 1.f : -1.f;
			else
				up.m_z = 1.f;
		}

		auto ApplyMatrixMovementFix( CUserCmd* cmd , const QAngle& realView ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !cmd )
				return;

			auto* base = cmd->cmd.mutable_base();
			if ( !base )
				return;

			QAngle wishAngle{};
			wishAngle.m_x = base->viewangles().x();
			wishAngle.m_y = base->viewangles().y();
			wishAngle.m_z = 0.f;

			const int reverse = wishAngle.m_x > 89.f ? -1 : 1;
			if ( wishAngle.m_x > 89.f )
				wishAngle.m_x = 89.f;
			if ( wishAngle.m_x < -89.f )
				wishAngle.m_x = -89.f;
			wishAngle.m_y = NormalizeYaw( wishAngle.m_y );

			Vector3 viewFwd , viewRight , viewUp;
			Vector3 cmdFwd , cmdRight , cmdUp;
			AngleDirections( wishAngle , viewFwd , viewRight , viewUp );
			AngleDirections( realView , cmdFwd , cmdRight , cmdUp );

			const auto normalize2d = []( Vector3 v ) -> Vector3
			{
				const float len = v.Length2D();
				if ( len > 0.001f )
				{
					v.m_x /= len;
					v.m_y /= len;
				}
				v.m_z = 0.f;
				return v;
			};

			const Vector3 normViewFwd = normalize2d( viewFwd );
			const Vector3 normViewRight = normalize2d( viewRight );
			const Vector3 normViewUp{ 0.f , 0.f , std::fabs( viewUp.m_z ) > 0.001f ? ( viewUp.m_z > 0.f ? 1.f : -1.f ) : 1.f };

			const Vector3 normCmdFwd = normalize2d( cmdFwd );
			const Vector3 normCmdRight = normalize2d( cmdRight );
			const Vector3 normCmdUp{ 0.f , 0.f , std::fabs( cmdUp.m_z ) > 0.001f ? ( cmdUp.m_z > 0.f ? 1.f : -1.f ) : 1.f };

			const float forwardMove = base->forwardmove();
			const float sideMove = base->leftmove();
			const float upMove = base->upmove();

			const float v22 = normViewFwd.m_x * forwardMove;
			const float v26 = normViewFwd.m_y * forwardMove;
			const float v28 = normViewFwd.m_z * forwardMove;
			const float v24 = normViewRight.m_x * sideMove;
			const float v23 = normViewRight.m_y * sideMove;
			const float v25 = normViewRight.m_z * sideMove;
			const float v30 = normViewUp.m_x * upMove;
			const float v27 = normViewUp.m_z * upMove;
			const float v29 = normViewUp.m_y * upMove;

			auto project = [&]( const Vector3& axis ) -> float
			{
				return ( ( axis.m_x * v24 ) + ( axis.m_y * v23 ) + ( axis.m_z * v25 ) )
					+ ( ( axis.m_x * v22 ) + ( axis.m_y * v26 ) + ( axis.m_z * v28 ) )
					+ ( ( axis.m_y * v30 ) + ( axis.m_x * v29 ) + ( axis.m_z * v27 ) );
			};

			float fixedForward = project( normCmdFwd );
			float fixedSide = project( normCmdRight );
			const float fixedUp = project( normCmdUp );

			fixedForward = reverse * fixedForward;
			fixedForward = ClampMove( fixedForward );
			fixedSide = ClampMove( fixedSide );

			base->set_forwardmove( fixedForward );
			base->set_leftmove( fixedSide );
			base->set_upmove( ClampMove( fixedUp ) );
#endif
		}

		auto RotateMovement( CUserCmd* cmd , float targetYaw , float& forwardMove , float& sideMove ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !cmd )
				return;

			auto* base = cmd->cmd.mutable_base();
			if ( !base )
				return;

			const float viewYaw = base->viewangles().y();
			const float rotation = ( viewYaw - targetYaw ) * ( kPi / 180.f );
			const float cosRot = std::cos( rotation );
			const float sinRot = std::sin( rotation );

			const float newForward = ( cosRot * forwardMove ) - ( sinRot * sideMove );
			const float newSide = ( sinRot * forwardMove ) + ( cosRot * sideMove );

			forwardMove = ClampMove( newForward );
			sideMove = ClampMove( -newSide );

			GetCL_Bypass()->SetButton( cmd , IN_FORWARD , false );
			GetCL_Bypass()->SetButton( cmd , IN_BACK , false );
			GetCL_Bypass()->SetButton( cmd , IN_MOVELEFT , false );
			GetCL_Bypass()->SetButton( cmd , IN_MOVERIGHT , false );

			if ( forwardMove > 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_FORWARD , true );
			else if ( forwardMove < 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_BACK , true );

			if ( sideMove > 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_MOVELEFT , true );
			else if ( sideMove < 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_MOVERIGHT , true );

			base->set_forwardmove( forwardMove );
			base->set_leftmove( sideMove );
#endif
		}

		auto TraceRingDown( C_CSPlayerPawn* pawn , uint32_t pawnHandle , const Vector3& origin ) noexcept -> bool
		{
			if ( !pawn || !Trace::ready )
				return false;

			constexpr float maxRadius = kPi * 2.f;
			constexpr float step = maxRadius / 128.f;
			constexpr float ringRadius = 23.f;
			constexpr float traceDepth = 8192.f;

			for ( float angle = 0.f; angle < maxRadius; angle += step )
			{
				const Game::Vector3 start{
					( ringRadius * std::cos( angle ) ) + origin.m_x ,
					( ringRadius * std::sin( angle ) ) + origin.m_y ,
					origin.m_z
				};
				const Game::Vector3 end{ start.x , start.y , start.z - traceDepth };

				float fraction = 1.f;
				if ( !Trace::RunTrace(
					start ,
					end ,
					reinterpret_cast<uintptr_t>( pawn ) ,
					pawnHandle ,
					Trace::kGroundFilterProfile ,
					fraction ) )
					continue;

				if ( fraction > 0.f && fraction < 1.f )
					return true;
			}

			return false;
		}

		auto EdgeBugActive() noexcept -> bool
		{
			if ( !config.edgeBug )
				return false;

			if ( !config.edgeBugUseKey )
				return true;

			return KeyBindUtils::IsActive( static_cast<unsigned int>( config.edgeBugKey ) );
		}

		auto SetEdgeBugCvar( bool active ) noexcept -> void
		{
			if ( auto* cv = CookieCore::CConVars::sv_min_jump_landing_sound )
				cv->value.fl = active ? 63464578.f : 260.f;
		}

		auto ApplyEdgeBugDuck( CUserCmd* cmd , C_CSPlayerPawn* pawn ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !cmd || !pawn )
				return;

			auto* base = cmd->cmd.mutable_base();
			if ( !base )
				return;

			base->set_forwardmove( 0.f );
			base->set_leftmove( 0.f );
			base->set_upmove( 0.f );
			base->set_mousedx( 0 );
			base->set_mousedy( 0 );

			GetCL_Bypass()->SetButton( cmd , IN_DUCK , true , true );
			if ( IsOnGround( pawn ) )
				GetCL_Bypass()->SetButton( cmd , IN_JUMP , false , false );
#endif
		}

		auto EdgeBug( CCSPlayerController* controller , C_CSPlayerPawn* localPawn , CUserCmd* cmd ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !controller || !localPawn || !cmd || !localPawn->IsAlive() )
				return;

			const bool keyActive = EdgeBugActive();
			SetEdgeBugCvar( keyActive );

			if ( !config.edgeBug || !keyActive )
				return;

			if ( !Trace::ready )
				return;

			auto* base = cmd->cmd.mutable_base();
			if ( !base )
				return;

			auto* predictedPawn = controller->m_hPredictedPawn().Get<C_CSPlayerPawn>();
			if ( !predictedPawn )
				return;

			uint32_t pawnHandle = 0;
			if ( auto* localController = GetCL_Players()->GetLocalPlayerController() )
				pawnHandle = static_cast<uint32_t>( localController->m_hPawn().GetEntryIndex() );

			static bool edgeBugging = false;
			static int edgeBugTick = 0;

			const int clientTick = base->client_tick();

			if ( !edgeBugging )
			{
				const uint32_t flags = GetPawnFlags( localPawn );
				const float zVelocity = std::floor( localPawn->m_vecAbsVelocity().m_z );

				if ( zVelocity < -7.f
					&& zVelocity == -7.f
					&& ( flags & FL_ONGROUND ) == 0
					&& localPawn->m_MoveType() != MOVETYPE_NOCLIP )
				{
					edgeBugTick = clientTick + 1;
					edgeBugging = true;
				}
			}
			else
			{
				ApplyEdgeBugDuck( cmd , localPawn );

				const Vector3 origin = localPawn->GetOrigin();
				if ( TraceRingDown( localPawn , pawnHandle , origin ) )
					ApplyEdgeBugDuck( cmd , localPawn );

				if ( clientTick > edgeBugTick )
				{
					edgeBugging = false;
					edgeBugTick = 0;
				}
			}

			if ( predictedPawn->m_bInLanding() )
			{
				const Vector3 origin = localPawn->GetOrigin();
				if ( TraceRingDown( localPawn , pawnHandle , origin ) )
					ApplyEdgeBugDuck( cmd , localPawn );
			}

			if ( clientTick > edgeBugTick )
			{
				edgeBugging = false;
				edgeBugTick = 0;
			}
#endif
		}

		auto AutoStrafe( CUserCmd* cmd , CBaseUserCmdPB* base , C_CSPlayerPawn* pawn , CCSGOInput* input ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( config.strafeMode == StrafeMode::Off || !cmd || !base || !pawn )
				return;

			if ( IsOnGround( pawn ) )
				return;

			static uint64_t lastPressed = 0;
			static uint64_t lastButtons = 0;

			const uint64_t currentButtons = cmd->button_states.buttonstate1;
			const bool strafeAssist = config.strafeAssist;
			const bool viewAngleStrafe = config.strafeMode == StrafeMode::Rage;

			float yaw = NormalizeYaw( base->viewangles().y() );

			const auto checkButton = [&]( const uint64_t button )
			{
				if ( currentButtons & button
					&& ( !( lastButtons & button )
						|| ( button & IN_MOVELEFT && !( lastPressed & IN_MOVERIGHT ) )
						|| ( button & IN_MOVERIGHT && !( lastPressed & IN_MOVELEFT ) )
						|| ( button & IN_FORWARD && !( lastPressed & IN_BACK ) )
						|| ( button & IN_BACK && !( lastPressed & IN_FORWARD ) ) ) )
				{
					if ( strafeAssist )
					{
						if ( button & IN_MOVELEFT )
							lastPressed &= ~IN_MOVERIGHT;
						else if ( button & IN_MOVERIGHT )
							lastPressed &= ~IN_MOVELEFT;
						else if ( button & IN_FORWARD )
							lastPressed &= ~IN_BACK;
						else if ( button & IN_BACK )
							lastPressed &= ~IN_FORWARD;
					}

					lastPressed |= button;
				}
				else if ( !( currentButtons & button ) )
					lastPressed &= ~button;
			};

			checkButton( IN_MOVELEFT );
			checkButton( IN_MOVERIGHT );
			checkButton( IN_FORWARD );
			checkButton( IN_BACK );
			lastButtons = currentButtons;

			const Vector3 velocity = pawn->m_vecAbsVelocity();
			float smoothing = config.strafeSmooth;
			if ( config.strafeMode == StrafeMode::Rage )
				smoothing *= 0.5f;

			float forwardMove = base->forwardmove();
			float sideMove = base->leftmove();

			float offset = 0.f;
			if ( lastPressed & IN_MOVELEFT )
				offset += 90.f;
			if ( lastPressed & IN_MOVERIGHT )
				offset -= 90.f;
			if ( lastPressed & IN_FORWARD )
				offset *= 0.5f;
			else if ( lastPressed & IN_BACK )
				offset = -offset * 0.5f + 180.f;

			yaw += offset;
			forwardMove = 0.f;
			sideMove = 0.f;
			RotateMovement( cmd , NormalizeYaw( yaw ) , forwardMove , sideMove );

			if ( !viewAngleStrafe && offset == 0.f )
				return;

			forwardMove = base->forwardmove();
			sideMove = base->leftmove();
			if ( sideMove != 0.f || forwardMove != 0.f )
				return;

			float velocityAngle = std::atan2f( velocity.m_y , velocity.m_x ) * ( 180.f / kPi );
			if ( velocityAngle < 0.f )
				velocityAngle += 360.f;
			velocityAngle = NormalizeYaw( velocityAngle );

			const float speed = velocity.Length2D();
			const float ideal = std::clamp(
				std::atan2( 15.f , speed > 1.f ? speed : 1.f ) * ( 180.f / kPi ) ,
				0.f ,
				45.f );

			const float correct = ( 100.f - smoothing ) * 0.02f * ( ideal + ideal );
			const float velocityDelta = NormalizeYaw( yaw - velocityAngle );

			forwardMove = 0.f;
			sideMove = 0.f;

			uint32_t sequence = 0;
			if ( input && GetCL_Players()->GetLocalPlayerController() )
				sequence = input->GetSequenceNumber( GetCL_Players()->GetLocalPlayerController() );

			const bool sideSwitch = ( sequence % 2 ) == 0;

			if ( std::fabs( velocityDelta ) > 170.f && speed > 80.f || velocityDelta > correct && speed > 80.f )
			{
				yaw = correct + velocityAngle;
				sideMove = -1.f;
				RotateMovement( cmd , NormalizeYaw( yaw ) , forwardMove , sideMove );
				return;
			}

			if ( -correct <= velocityDelta || speed <= 80.f )
			{
				if ( sideSwitch )
				{
					yaw -= ideal;
					sideMove = -1.f;
				}
				else
				{
					yaw += ideal;
					sideMove = 1.f;
				}
			}
			else
			{
				yaw = velocityAngle - correct;
				sideMove = 1.f;
			}

			RotateMovement( cmd , NormalizeYaw( yaw ) , forwardMove , sideMove );
#endif
		}

		auto MovementCorrectionEntry(
			CBaseUserCmdPB* base ,
			CSGOInputHistoryEntryPB* history ,
			const QAngle& desiredView ,
			float forwardMove ,
			float sideMove ,
			float upMove ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !base || !history || !history->has_view_angles() )
				return;

			Vector3 vecForward , vecRight , vecUp;
			QAngle desired = desiredView;
			desired.m_z = 0.f;
			AngleDirections( desired , vecForward , vecRight , vecUp );
			FlattenDirection( vecForward , vecRight , vecUp );

			Vector3 vecOldForward , vecOldRight , vecOldUp;
			QAngle historyView{};
			historyView.m_x = history->view_angles().x();
			historyView.m_y = history->view_angles().y();
			historyView.m_z = 0.f;
			AngleDirections( historyView , vecOldForward , vecOldRight , vecOldUp );
			FlattenDirection( vecOldForward , vecOldRight , vecOldUp );

			const float pitchForward = vecForward.m_x * forwardMove;
			const float yawForward = vecForward.m_y * forwardMove;
			const float pitchSide = vecRight.m_x * sideMove;
			const float yawSide = vecRight.m_y * sideMove;
			const float rollUp = vecUp.m_z * upMove;

			const float correctedForward =
				vecOldForward.m_x * pitchSide + vecOldForward.m_y * yawSide
				+ vecOldForward.m_x * pitchForward + vecOldForward.m_y * yawForward
				+ vecOldForward.m_z * rollUp;

			const float correctedSide =
				vecOldRight.m_x * pitchSide + vecOldRight.m_y * yawSide
				+ vecOldRight.m_x * pitchForward + vecOldRight.m_y * yawForward
				+ vecOldRight.m_z * rollUp;

			const float correctedUp =
				vecOldUp.m_x * yawSide + vecOldUp.m_y * pitchSide
				+ vecOldUp.m_x * yawForward + vecOldUp.m_y * pitchForward
				+ vecOldUp.m_z * rollUp;

			base->set_forwardmove( correctedForward );
			base->set_leftmove( correctedSide );
			base->set_upmove( correctedUp );
#endif
		}

		auto ClampHistoryAngles( CSGOInputHistoryEntryPB* history ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !history || !history->has_view_angles() )
				return;

			auto* viewAngles = history->mutable_view_angles();
			float pitch = viewAngles->x();
			float yaw = viewAngles->y();

			if ( pitch > 89.f )
				pitch = 89.f;
			if ( pitch < -89.f )
				pitch = -89.f;
			yaw = NormalizeYaw( yaw );

			viewAngles->set_x( pitch );
			viewAngles->set_y( yaw );
			viewAngles->set_z( 0.f );
#endif
		}

		auto SyncMoveButtons( CUserCmd* cmd , CBaseUserCmdPB* base ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !cmd || !base )
				return;

			GetCL_Bypass()->SetButton( cmd , IN_FORWARD , false );
			GetCL_Bypass()->SetButton( cmd , IN_BACK , false );
			GetCL_Bypass()->SetButton( cmd , IN_MOVELEFT , false );
			GetCL_Bypass()->SetButton( cmd , IN_MOVERIGHT , false );

			if ( base->forwardmove() > 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_FORWARD , true );
			else if ( base->forwardmove() < 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_BACK , true );

			if ( base->leftmove() > 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_MOVELEFT , true );
			else if ( base->leftmove() < 0.f )
				GetCL_Bypass()->SetButton( cmd , IN_MOVERIGHT , true );
#endif
		}

		auto ApplyHistoryMouseDelta(
			CBaseUserCmdPB* base ,
			const QAngle& beforeClamp ,
			const QAngle& afterClamp ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !base )
				return;

			const float deltaX = std::remainderf( afterClamp.m_x - beforeClamp.m_x , 360.f );
			const float deltaY = std::remainderf( afterClamp.m_y - beforeClamp.m_y , 360.f );

			float pitchCvar = 0.022f;
			float yawCvar = 0.022f;
			float sensitivity = 1.f;

			if ( auto* cv = CookieCore::CConVars::m_pitch )
				pitchCvar = cv->value.fl;
			if ( auto* cv = CookieCore::CConVars::m_yaw )
				yawCvar = cv->value.fl;
			if ( auto* cv = CookieCore::CConVars::sensitivity )
				sensitivity = cv->value.fl;

			if ( sensitivity <= 0.f )
				sensitivity = 1.f;
			if ( pitchCvar <= 0.f )
				pitchCvar = 0.022f;
			if ( yawCvar <= 0.f )
				yawCvar = 0.022f;

			base->set_mousedx( static_cast<int>( deltaX / ( sensitivity * pitchCvar ) ) );
			base->set_mousedy( static_cast<int>( -deltaY / ( sensitivity * yawCvar ) ) );
#endif
		}

		auto ProcessInputHistory( CUserCmd* cmd ) noexcept -> void
		{
#if DISABLE_PROTOBUF == 0
			if ( !cmd )
				return;

			auto* base = cmd->cmd.mutable_base();
			if ( !base )
				return;

			QAngle desiredView{};
			desiredView.m_x = base->viewangles().x();
			desiredView.m_y = base->viewangles().y();
			desiredView.m_z = 0.f;

			const float originalForward = base->forwardmove();
			const float originalSide = base->leftmove();
			const float originalUp = base->upmove();

			for ( int i = 0; i < cmd->cmd.input_history_size(); ++i )
			{
				auto* history = cmd->cmd.mutable_input_history( i );
				if ( !history || !history->has_view_angles() )
					continue;

				QAngle beforeClamp{};
				beforeClamp.m_x = history->view_angles().x();
				beforeClamp.m_y = history->view_angles().y();
				beforeClamp.m_z = 0.f;

				if ( config.movementCorrection )
				{
					MovementCorrectionEntry(
						base ,
						history ,
						desiredView ,
						originalForward ,
						originalSide ,
						originalUp );
				}

				if ( !config.validateAngles )
					continue;

				ClampHistoryAngles( history );

				QAngle afterClamp{};
				afterClamp.m_x = history->view_angles().x();
				afterClamp.m_y = history->view_angles().y();
				afterClamp.m_z = 0.f;

				SyncMoveButtons( cmd , base );

				if ( beforeClamp.m_x != 0.f || beforeClamp.m_y != 0.f || afterClamp.m_x != 0.f || afterClamp.m_y != 0.f )
					ApplyHistoryMouseDelta( base , beforeClamp , afterClamp );
			}

			base->set_forwardmove( ClampMove( base->forwardmove() ) );
			base->set_leftmove( ClampMove( base->leftmove() ) );
			base->set_upmove( ClampMove( base->upmove() ) );
#endif
		}
	}

	auto MovementFix( CUserCmd* cmd , float realYaw , float fakeYaw ) noexcept -> void
	{
		if ( !config.movementFix || !cmd )
			return;

		AntiAim::FixMovement( cmd , realYaw , fakeYaw );
	}

	auto Shutdown() noexcept -> void
	{
		if ( auto* cv = CookieCore::CConVars::sv_min_jump_landing_sound )
			cv->value.fl = 260.f;
	}

	auto ApplyAutoStop(
		CUserCmd* cmd ,
		C_CSPlayerPawn* pawn ,
		C_CSWeaponBaseGun* weapon ,
		AutoStopMode mode ,
		bool active ) noexcept -> void
	{
#if DISABLE_PROTOBUF == 0
		if ( !active || !cmd || !pawn || !pawn->IsAlive() )
			return;

		if ( !IsOnGround( pawn ) )
			return;

		auto* vdata = GetCL_Weapons()->GetLocalWeaponVData();
		if ( !vdata )
			return;

		if ( vdata->m_WeaponType().m_Type == CSWeaponType_t::WEAPONTYPE_SNIPER_RIFLE && mode == AutoStopMode::Early )
		{
			// allow early stop while unscoped on snipers
		}

		float maxSpeed = 250.f;
		switch ( vdata->m_WeaponType().m_Type )
		{
		case CSWeaponType_t::WEAPONTYPE_PISTOL: maxSpeed = 240.f; break;
		case CSWeaponType_t::WEAPONTYPE_SUBMACHINEGUN: maxSpeed = 235.f; break;
		case CSWeaponType_t::WEAPONTYPE_SNIPER_RIFLE: maxSpeed = 200.f; break;
		case CSWeaponType_t::WEAPONTYPE_SHOTGUN: maxSpeed = 220.f; break;
		default: break;
		}

		( void )weapon;

		const float maxAccurateSpeed = maxSpeed * 0.33f;
		Vector3 velocity = pawn->m_vecAbsVelocity();
		velocity.m_z = 0.f;
		const float speed = velocity.Length2D();

		GetCL_Bypass()->SetButton( cmd , IN_FORWARD , false );
		GetCL_Bypass()->SetButton( cmd , IN_BACK , false );
		GetCL_Bypass()->SetButton( cmd , IN_MOVELEFT , false );
		GetCL_Bypass()->SetButton( cmd , IN_MOVERIGHT , false );
		GetCL_Bypass()->SetButton( cmd , IN_SPEED , false );

		auto* base = cmd->cmd.mutable_base();
		if ( !base )
			return;

		if ( mode == AutoStopMode::Early )
		{
			base->set_forwardmove( 0.f );
			base->set_leftmove( 0.f );
			return;
		}

		base->set_forwardmove( 0.f );
		base->set_leftmove( 0.f );

		if ( speed <= maxAccurateSpeed )
			return;

		QAngle velAngle{};
		Math::VectorAngles( velocity , velAngle );
		const float viewYaw = base->viewangles().y();
		const float yawDelta = NormalizeYaw( viewYaw - velAngle.m_y );
		const float rad = yawDelta * ( kPi / 180.f );

		const float stopForward = std::cos( rad ) * -1.f;
		const float stopSide = std::sin( rad ) * -1.f;
		base->set_forwardmove( ClampMove( stopForward ) );
		base->set_leftmove( ClampMove( stopSide ) );
#endif
	}

	auto Process( CCSGOInput* input , CUserCmd* cmd ) noexcept -> void
	{
		if ( !cmd )
			return;

		auto* controller = GetCL_Players()->GetLocalPlayerController();
		auto* pawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !controller || !pawn || !pawn->IsAlive() )
			return;

#if DISABLE_PROTOBUF == 0
		auto* base = cmd->cmd.mutable_base();
		if ( !base )
			return;

		if ( ShouldSkipMovement( pawn ) )
			return;

		EdgeBug( controller , pawn , cmd );
		AutoStrafe( cmd , base , pawn , input );
		Bhop::Process( input , cmd );

		if ( config.movementFix && input )
		{
			const QAngle realView = *CCSGOInput_GetViewAngles( input , 0 );
			ApplyMatrixMovementFix( cmd , realView );
		}

		ProcessInputHistory( cmd );
#else
		( void )input;
#endif
	}
}

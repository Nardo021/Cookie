#include "ThirdPerson.hpp"

#include <algorithm>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>
#include <CS2/SDK/Update/CViewSetup.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

#include <Client/Core/CConVars.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Game/Trace.hpp>
#include <CS2/Hook/Hook_GetRenderFov.hpp>

#include <GameClient/CL_Input.hpp>
#include <GameClient/CL_Players.hpp>

namespace ThirdPerson
{
	namespace
	{
		float s_smoothProgress = 0.f;
		bool  s_wasSmoothCamera = false;

		auto SmoothStepBezier( float t ) noexcept -> float
		{
			return t * t * ( 3.f - 2.f * t );
		}

		auto ApplyEngineThirdPersonConvars( float idealDistance ) noexcept -> void
		{
			if ( CookieCore::CConVars::cam_idealdist )
				CookieCore::CConVars::cam_idealdist->value.fl = idealDistance;

			if ( CookieCore::CConVars::c_thirdpersonshoulder )
				CookieCore::CConVars::c_thirdpersonshoulder->value.i1 = true;
			if ( CookieCore::CConVars::c_thirdpersonshoulderaimdist )
				CookieCore::CConVars::c_thirdpersonshoulderaimdist->value.fl = 0.f;
			if ( CookieCore::CConVars::c_thirdpersonshoulderdist )
				CookieCore::CConVars::c_thirdpersonshoulderdist->value.fl = 0.f;
			if ( CookieCore::CConVars::c_thirdpersonshoulderheight )
				CookieCore::CConVars::c_thirdpersonshoulderheight->value.fl = 0.f;
			if ( CookieCore::CConVars::c_thirdpersonshoulderoffset )
				CookieCore::CConVars::c_thirdpersonshoulderoffset->value.fl = 0.f;
		}

		auto ResolveCameraDistance( float targetDistance ) noexcept -> float
		{
			targetDistance = std::clamp( targetDistance , 40.f , 400.f );

			if ( !config.smoothCamera )
			{
				s_smoothProgress = 1.f;
				s_wasSmoothCamera = false;
				return targetDistance;
			}

			if ( !s_wasSmoothCamera )
				s_smoothProgress = 0.f;
			s_wasSmoothCamera = true;

			float frameTime = 1.f / 64.f;
			if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
				frameTime = globalVars->m_flFrameTime();

			const float speed = std::clamp( config.smoothSpeed , 1.f , 20.f );
			const float minProgress = 40.f / targetDistance;
			s_smoothProgress = std::clamp( s_smoothProgress + frameTime * speed , minProgress , 1.f );

			return targetDistance * SmoothStepBezier( s_smoothProgress );
		}
	}

	auto ApplyInput( CCSGOInput* input ) noexcept -> void
	{
		input = GetCL_Input( input );
		if ( !input )
			return;

		if ( !config.enabled )
		{
			s_smoothProgress = 0.f;
			input->m_bInThirdPerson() = false;
			return;
		}

		input->m_bInThirdPerson() = true;
	}

	auto ApplyView( CViewSetup* setup ) noexcept -> void
	{
		if ( !setup || !config.enabled )
			return;

		auto* pawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !pawn || !pawn->IsAlive() )
			return;

		auto* input = GetCL_Input();
		if ( !input )
			return;

		uint32_t localHandle = 0;
		if ( auto* controller = GetCL_Players()->GetLocalPlayerController() )
			localHandle = Game::Read<uint32_t>( reinterpret_cast<uintptr_t>( controller ) + Offsets::m_hPlayerPawn );

		const Vector3 eyePos = GetCL_Players()->GetLocalEyeOrigin();
		QAngle viewAngles = *CCSGOInput_GetViewAngles( input , 0 );

		QAngle camAngles = viewAngles;
		camAngles.m_x = -camAngles.m_x;

		const float distance = ResolveCameraDistance( config.distance );
		ApplyEngineThirdPersonConvars( distance );

		Vector3 camPos = Math::CalculateCameraPosition( eyePos , -distance , camAngles );

		float fraction = 1.f;
		if ( Trace::RunTrace(
			{ eyePos.m_x , eyePos.m_y , eyePos.m_z } ,
			{ camPos.m_x , camPos.m_y , camPos.m_z } ,
			reinterpret_cast<uintptr_t>( pawn ) ,
			localHandle ,
			Trace::kShotFilterProfile ,
			fraction ) && fraction < 1.f )
		{
			const float scale = fraction * 0.9f;
			camPos.m_x = eyePos.m_x + ( camPos.m_x - eyePos.m_x ) * scale;
			camPos.m_y = eyePos.m_y + ( camPos.m_y - eyePos.m_y ) * scale;
			camPos.m_z = eyePos.m_z + ( camPos.m_z - eyePos.m_z ) * scale;
		}

		QAngle lookAngles = Math::CalcAngle( camPos , eyePos );
		Math::NormalizeAngles( lookAngles );
		Math::ClampAngles( lookAngles );

		setup->m_vecOrigin() = camPos;
		setup->m_angView() = lookAngles;

		pawn->SetBodyGroup();

		if ( const uintptr_t cameraServices = Game::Read<uintptr_t>(
			reinterpret_cast<uintptr_t>( pawn ) + Offsets::m_pCameraServices ) )
		{
			const uint32_t fov = static_cast<uint32_t>( std::clamp( config.fov , 60 , 140 ) );
			Game::Write( cameraServices + Offsets::m_iFOV , fov );
			Game::Write( cameraServices + Offsets::m_iFOVStart , fov );
		}
	}

	auto Shutdown() noexcept -> void
	{
		config.enabled = false;
		s_smoothProgress = 0.f;
		s_wasSmoothCamera = false;
		g_fovOverride = 0.f;

		if ( auto* input = GetCL_Input() )
			input->m_bInThirdPerson() = false;
	}
}

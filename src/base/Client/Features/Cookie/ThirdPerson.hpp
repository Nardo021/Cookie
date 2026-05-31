#pragma once

#include <cstdint>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Update/GameTrace.hpp>
#include <CS2/SDK/Update/Offsets.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/SDK.hpp>

#include <GameClient/CL_Players.hpp>

#include "SkinChanger.hpp"

namespace ThirdPerson
{
	inline void ApplyCameraFov( C_CSPlayerPawn* pawn )
	{
		if ( !pawn )
			return;

		uintptr_t cameraServices = Game::Read<uintptr_t>(
			reinterpret_cast<uintptr_t>( pawn ) + Offsets::m_pCameraServices );
		if ( !cameraServices )
			return;

		int fov = SkinChanger::thirdPersonFov;
		if ( fov < 60 )
			fov = 60;
		else if ( fov > 140 )
			fov = 140;

		const uint32_t fovU = static_cast<uint32_t>( fov );
		Game::Write( cameraServices + Offsets::m_iFOV , fovU );
		Game::Write( cameraServices + Offsets::m_iFOVStart , fovU );
	}

	inline void Apply( void* pSetup ) noexcept
	{
		if ( !pSetup || !SkinChanger::thirdPerson || !Game::clientBase )
			return;

		auto* pawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !pawn || !pawn->IsAlive() )
			return;

		auto* input = SkinChanger::GetCSGOInput();
		if ( !input )
			return;

		const Vector3 eyePos = GetCL_Players()->GetLocalEyeOrigin();
		QAngle viewAngles = *CCSGOInput_GetViewAngles( input , 0 );

		QAngle camAngles = viewAngles;
		camAngles.m_x = -camAngles.m_x;

		float distance = static_cast<float>( SkinChanger::thirdPersonDistance );
		if ( distance < 40.f )
			distance = 40.f;
		else if ( distance > 400.f )
			distance = 400.f;

		Vector3 camPos = Math::CalculateCameraPosition( eyePos , -distance , camAngles );

		Ray_t ray{};
		CGameTrace trace{};
		CTraceFilter filter( 0x1C3003 , pawn , 4 , 15 );

		if ( IGamePhysicsQuery_TraceShape(
				SDK::Pointers::CVPhys2World() ,
				ray ,
				eyePos ,
				camPos ,
				&filter ,
				&trace ) )
		{
			if ( trace.pHitEntity != nullptr )
				camPos = trace.vecPosition;
		}

		QAngle lookAngles = Math::CalcAngle( camPos , eyePos );
		Math::NormalizeAngles( lookAngles );
		Math::ClampAngles( lookAngles );

		const auto setupAddr = reinterpret_cast<uintptr_t>( pSetup );
		*reinterpret_cast<Vector3*>( setupAddr + g_CViewSetup_vecOrigin ) = camPos;
		*reinterpret_cast<QAngle*>( setupAddr + g_CViewSetup_angView ) = lookAngles;

		pawn->SetBodyGroup();
		ApplyCameraFov( pawn );
	}
}

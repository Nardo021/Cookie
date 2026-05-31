#include "ThirdPerson.hpp"

#include <algorithm>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CViewSetup.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Game/Trace.hpp>

#include <GameClient/CL_Input.hpp>
#include <GameClient/CL_Players.hpp>

auto ThirdPerson::ApplyInput( CCSGOInput* input ) noexcept -> void
{
	input = GetCL_Input( input );
	if ( !input )
		return;

	input->m_bInThirdPerson() = config.enabled;
}

auto ThirdPerson::ApplyView( CViewSetup* setup ) noexcept -> void
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

	const float distance = std::clamp( config.distance , 40.f , 400.f );
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

auto ThirdPerson::Shutdown() noexcept -> void
{
	config.enabled = false;

	if ( auto* input = GetCL_Input() )
		input->m_bInThirdPerson() = false;
}

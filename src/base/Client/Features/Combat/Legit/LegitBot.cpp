#include "LegitBot.hpp"

#include <algorithm>
#include <cstring>

#include <CS2/SDK/Interface/IGameEvent.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>
#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Math/QAngle.hpp>

#include <Client/Features/Combat/EnginePred.hpp>
#include <Client/Features/Combat/Legit/LegitScan.hpp>
#include <Client/Core/SpreadHooks.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Combat/Legit/Triggerbot.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace LegitBot
{
	namespace
	{
		int s_delayUntilTick = 0;
	}

	auto SyncToAimbot() noexcept -> void
	{
		Aimbot::config.enabled = config.enabled;
		Aimbot::config.fovType = config.fovType;
		Aimbot::config.fov = config.fov;
		Aimbot::config.screenFov = config.screenFov;
		Aimbot::config.targetHitbox = config.targetHitbox;
		Aimbot::config.aimKey = config.aimKey;
		Aimbot::config.autoShoot = config.autoShoot;
		Aimbot::config.silentAim = config.silentAim;
		Aimbot::config.teamCheck = config.teamCheck;
		Aimbot::config.visCheck = config.visCheck;
		Aimbot::config.penetration = config.penetration;
		Aimbot::config.recoilControl = config.recoilControl;
		Aimbot::config.smooth = config.smooth;
		Aimbot::config.punchRandomization = config.punchRandomization;
		Aimbot::config.targetSelection = config.targetSelection;
		Aimbot::config.fovVisualize = config.fovVisualize;
		memcpy( Aimbot::config.fovVisualizeColor , config.fovVisualizeColor , sizeof( config.fovVisualizeColor ) );
		Aimbot::config.hitboxHead = config.hitboxHead;
		Aimbot::config.hitboxNeck = config.hitboxNeck;
		Aimbot::config.hitboxChest = config.hitboxChest;
		Aimbot::config.hitboxUpperChest = config.hitboxUpperChest;
		Aimbot::config.hitboxPelvis = config.hitboxPelvis;
		Aimbot::config.hitboxStomach = config.hitboxStomach;
		Aimbot::config.hitboxLegs = config.hitboxLegs;
		Aimbot::config.hitboxArms = config.hitboxArms;
		Aimbot::config.rcsShotsEnable = config.recoilControl;
	}

	auto SyncFromAimbot() noexcept -> void
	{
		config.enabled = Aimbot::config.enabled;
		config.fovType = Aimbot::config.fovType;
		config.fov = Aimbot::config.fov;
		config.screenFov = Aimbot::config.screenFov;
		config.targetHitbox = Aimbot::config.targetHitbox;
		config.aimKey = Aimbot::config.aimKey;
		config.autoShoot = Aimbot::config.autoShoot;
		config.silentAim = Aimbot::config.silentAim;
		config.teamCheck = Aimbot::config.teamCheck;
		config.visCheck = Aimbot::config.visCheck;
		config.penetration = Aimbot::config.penetration;
		config.recoilControl = Aimbot::config.recoilControl;
		config.smooth = Aimbot::config.smooth;
		config.punchRandomization = Aimbot::config.punchRandomization;
		config.targetSelection = Aimbot::config.targetSelection;
		config.fovVisualize = Aimbot::config.fovVisualize;
		memcpy( config.fovVisualizeColor , Aimbot::config.fovVisualizeColor , sizeof( config.fovVisualizeColor ) );
		config.hitboxHead = Aimbot::config.hitboxHead;
		config.hitboxNeck = Aimbot::config.hitboxNeck;
		config.hitboxChest = Aimbot::config.hitboxChest;
		config.hitboxUpperChest = Aimbot::config.hitboxUpperChest;
		config.hitboxPelvis = Aimbot::config.hitboxPelvis;
		config.hitboxStomach = Aimbot::config.hitboxStomach;
		config.hitboxLegs = Aimbot::config.hitboxLegs;
		config.hitboxArms = Aimbot::config.hitboxArms;
	}

	auto ApplyWeaponSettings() noexcept -> void
	{
		if ( !config.usePerWeapon )
			return;

		const auto& ws = WeaponConfig::GetLegitSettingsForActiveWeapon();
		config.fov = static_cast<float>( ws.fov );
		config.aimKey = ws.aimKey;
		config.smooth = ws.smooth;
		config.targetSelection = ws.targetSelection;
		config.recoilControl = ws.rcs;
		config.punchRandomization = ws.punchRandomization;
		config.visCheck = ws.visCheck;
		config.hitboxHead = ws.hitboxHead;
		config.hitboxNeck = ws.hitboxNeck;
		config.hitboxChest = ws.hitboxChest;
		config.hitboxUpperChest = ws.hitboxUpperChest;
		config.hitboxPelvis = ws.hitboxPelvis;
		config.hitboxStomach = ws.hitboxStomach;
		config.hitboxLegs = ws.hitboxLegs;
		config.hitboxArms = ws.hitboxArms;
		config.triggerOverride = ws.triggerEnabled;
		config.triggerHitchance = ws.triggerHitchance;
		config.removeSpread = ws.removeSpread;
		Aimbot::config.rcsShots = ws.rcsShots;
		Aimbot::config.rcsShotsEnable = ws.rcsShotsEnable && ws.rcs;
		Aimbot::config.rcsSmoothX = ws.rcsSmoothX;
		Aimbot::config.rcsSmoothY = ws.rcsSmoothY;
		Aimbot::config.punchRandomX = ws.punchRandomX;
		Aimbot::config.punchRandomY = ws.punchRandomY;
	}

	auto PassesConditions( C_CSPlayerPawn* pawn ) noexcept -> bool
	{
		if ( !pawn )
			return false;

		if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
		{
			if ( ( config.conditions & static_cast<unsigned int>( Condition::DelayShot ) ) != 0
				&& s_killDelayTick >= globalVars->m_nTickCount() )
				return false;

			if ( config.delayAim && s_delayUntilTick > globalVars->m_nTickCount() )
				return false;
		}

		if ( ( config.conditions & static_cast<unsigned int>( Condition::InAir ) ) != 0 )
		{
			if ( !EnginePred::IsOnGround( pawn ) )
				return false;
		}

		if ( ( config.conditions & static_cast<unsigned int>( Condition::Flashed ) ) != 0 )
		{
			if ( pawn->m_flFlashDuration() > 0.f )
				return false;
		}

		if ( ( config.conditions & static_cast<unsigned int>( Condition::InSmoke ) ) != 0 )
		{
			const ::Vector3 eye = GetCL_Players()->GetLocalEyeOrigin();
			if ( eye.IsZero() )
				return false;

			const auto viewAngles = Game::Read<Game::QAngle>( Game::clientBase + Offsets::dwViewAngles );
			::QAngle sdkView( viewAngles.pitch , viewAngles.yaw , 0.f );
			::Vector3 forward{};
			Math::AngleVectors( sdkView , forward , nullptr , nullptr );
			forward.Normalize();

			if ( LegitScan::IsSmokeBetween( eye , eye + forward * 128.f ) )
				return false;
		}

		return true;
	}

	auto OnRoundStart() noexcept -> void
	{
		s_killDelayTick = 0;
		s_delayUntilTick = 0;
	}

	auto OnPlayerDeath( IGameEvent* event ) noexcept -> void
	{
		if ( !event )
			return;

		auto* localController = GetCL_Players()->GetLocalPlayerController();
		if ( !localController )
			return;

		auto* attacker = event->GetPlayerController( "attacker" );
		if ( !attacker || attacker != localController )
			return;

		if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
		{
			const int delayTicks = config.delayAim && config.delayAimMs > 0
				? std::max( 1 , config.delayAimMs / 15 )
				: 8;
			s_killDelayTick = globalVars->m_nTickCount() + delayTicks;
			if ( config.delayAim && config.delayAimMs > 0 )
				s_delayUntilTick = globalVars->m_nTickCount() + delayTicks;
		}
	}

	auto OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd ) -> void
	{
		if ( Ragebot::config.enabled )
			return;

		if ( !config.enabled )
		{
			Aimbot::config.enabled = false;
			return;
		}

		ApplyWeaponSettings();

		auto* pawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !PassesConditions( pawn ) )
		{
			Aimbot::config.enabled = false;
			return;
		}

		if ( config.noScope )
		{
			if ( auto* vdata = GetCL_Weapons()->GetLocalWeaponVData() )
			{
				if ( vdata->m_WeaponType().m_Type == CSWeaponType_t::WEAPONTYPE_SNIPER_RIFLE && pawn && !pawn->m_bIsScoped() )
					Aimbot::config.silentAim = true;
			}
		}

		SyncToAimbot();
		Aimbot::config.smooth = config.smooth;
		Aimbot::config.punchRandomization = config.punchRandomization;

		const bool useNoSpread = NoSpread::config.enabled
			|| ( config.usePerWeapon && config.removeSpread );
		const bool savedNoSpread = NoSpread::config.enabled;
		if ( useNoSpread )
			NoSpread::config.enabled = true;

		const bool savedTriggerEnabled = Triggerbot::config.enabled;
		const bool savedTriggerVis = Triggerbot::config.visCheck;
		const int savedTriggerHc = Triggerbot::config.hitchance;

		if ( config.usePerWeapon && config.triggerOverride )
			Triggerbot::config.enabled = true;

		Triggerbot::config.visCheck = config.visCheck;
		Triggerbot::config.hitchance = config.triggerHitchance;

		Aimbot::OnCreateMove( input , slot , cmd );

		Triggerbot::config.enabled = savedTriggerEnabled;
		Triggerbot::config.visCheck = savedTriggerVis;
		Triggerbot::config.hitchance = savedTriggerHc;

		if ( useNoSpread && !savedNoSpread )
			NoSpread::config.enabled = false;

		SyncFromAimbot();
	}
}

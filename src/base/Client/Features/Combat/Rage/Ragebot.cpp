#include "Ragebot.hpp"

#include <buttons.hpp>

#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>
#include <CS2/SDK/SDK.hpp>

#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Combat/EnginePred.hpp>
#include <Client/Features/Movement/Movement.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Features/Combat/Rage/RageScan.hpp>
#include <Client/Features/Combat/Rage/RageSubTick.hpp>
#include <Client/Features/Combat/Legit/WeaponConfig.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Utils/KeyBindUtils.hpp>

#include <GameClient/CL_Bypass.hpp>
#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace Ragebot
{
	namespace
	{
		int s_delayUntilTick = 0;

		auto CanShoot( C_CSPlayerPawn* localPawn , C_CSWeaponBaseGun* weapon ) noexcept -> bool
		{
			if ( !localPawn || !weapon )
				return false;

			if ( weapon->m_iClip1() <= 0 )
				return false;

			auto* globalVars = SDK::Pointers::GlobalVarsBase();
			if ( !globalVars )
				return true;

			const int nextAttackTick = weapon->m_nNextPrimaryAttackTick().m_Value;
			return nextAttackTick <= static_cast<int>( globalVars->m_nTickCount() );
		}

		auto ApplyAimpunch( Game::QAngle& angles , C_CSPlayerPawn* localPawn ) noexcept -> void
		{
			if ( !localPawn )
				return;

			const auto& punchCache = localPawn->m_aimPunchCache();
			const int punchCount = punchCache.Count();
			if ( punchCount <= 0 )
				return;

			const ::QAngle& punch = punchCache[punchCount - 1];
			angles.pitch += punch.m_x * 2.f;
			angles.yaw += punch.m_y * 2.f;
			Aimbot::ClampAngles( angles );
		}

		auto ApplyWeaponOverrides( RageScan::ScanSettings& settings ) noexcept -> void
		{
			if ( !config.usePerWeapon )
				return;

			const auto& ws = WeaponConfig::GetRageSettingsForActiveWeapon();
			settings.minDamage = ws.minDamage;
			settings.hitchance = ws.hitchance;
			settings.penetration = ws.penetration;
			settings.safePoint = ws.safePoint;
			config.rapidFire = ws.rapidFire;
			config.autoScope = ws.autoScope;
			config.autoStop = ws.autoStop;
			config.earlyAutoStop = ws.earlyAutoStop;
			config.safePoint = ws.safePoint;
			config.targetSelect = static_cast<RageScan::TargetSelect>( ws.targetSelect );
			config.hitboxHead = ws.hitboxHead;
			config.hitboxNeck = ws.hitboxNeck;
			config.hitboxChest = ws.hitboxChest;
			config.hitboxUpperChest = ws.hitboxUpperChest;
			config.hitboxPelvis = ws.hitboxPelvis;
			config.hitboxStomach = ws.hitboxStomach;
			config.hitboxArms = ws.hitboxArms;
			config.hitboxLegs = ws.hitboxLegs;
			config.hitboxFeet = ws.hitboxFeet;
		}
	}

	auto OnRoundStart() noexcept -> void
	{
		s_delayUntilTick = 0;
		RageScan::ResetTarget();
	}

	auto OnPlayerDeath() noexcept -> void
	{
		RageScan::ResetTarget();
	}

	auto ResetTarget() noexcept -> void
	{
		RageScan::ResetTarget();
	}

	auto BuildScanSettings() noexcept -> RageScan::ScanSettings
	{
		RageScan::ScanSettings settings{};
		settings.minDamage = config.minDamage;
		settings.hitchance = config.hitchance;
		settings.multipointScale = config.multipointScale;
		settings.penetration = config.penetration;
		settings.safePoint = config.safePoint;
		settings.hitboxHead = config.hitboxHead;
		settings.hitboxNeck = config.hitboxNeck;
		settings.hitboxChest = config.hitboxChest;
		settings.hitboxPelvis = config.hitboxPelvis;
		settings.hitboxStomach = config.hitboxStomach;
		settings.hitboxArms = config.hitboxArms;
		settings.hitboxLegs = config.hitboxLegs;
		settings.hitboxUpperChest = config.hitboxUpperChest;
		settings.hitboxFeet = config.hitboxFeet;
		settings.hitscanMode = config.hitscanMode;
		settings.scanMode = config.scanMode;
		settings.backtrack = config.backtrack;

		if ( config.adaptiveWeapon )
			RageScan::ApplyAdaptiveWeapon( settings );

		ApplyWeaponOverrides( settings );

		return settings;
	}

	auto OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd ) -> void
	{
		(void)slot;
		blockAntiAim = false;
		wantsAutoStop = false;
		RageSubTick::ResetTick();

		if ( !config.enabled || !Game::clientBase || !input || !cmd )
			return;

		static KeyBindUtils::KeyBindSlot s_activationSlot{};
		if ( config.activationUseKey && config.activationKey != 0
			&& !KeyBindUtils::IsActive(
				static_cast<unsigned int>( config.activationKey ) ,
				true ,
				config.activationKeyHold ,
				s_activationSlot ) )
			return;

		if ( Aimbot::IsMenuBlockingCombat() )
			return;

		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		auto* localController = GetCL_Players()->GetLocalPlayerController();
		if ( !localPawn || !localController || !localPawn->IsAlive() )
			return;

		if ( config.delayAim && s_delayUntilTick > 0 )
		{
			if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
			{
				if ( globalVars->m_nTickCount() < s_delayUntilTick )
				{
					if ( config.autoShoot )
						GetCL_Bypass()->SetDontAttack( cmd , true );
					return;
				}
			}
		}

		uint32_t localHandle = Game::Read<uint32_t>(
			reinterpret_cast<uintptr_t>( localController ) + Offsets::m_hPlayerPawn );

		EnginePred::Start( cmd );

		const ::Vector3 sdkEye = GetCL_Players()->GetLocalEyeOrigin();
		if ( sdkEye.IsZero() )
		{
			EnginePred::End();
			return;
		}

		const auto viewAngles = Game::Read<Game::QAngle>( Game::clientBase + Offsets::dwViewAngles );
		const RageScan::ScanSettings settings = BuildScanSettings();

		RageScan::PointResult target{};
		if ( !RageScan::ScanBestTarget(
			sdkEye ,
			viewAngles ,
			settings ,
			config.teamCheck ,
			config.targetSelect ,
			target ) )
		{
			if ( config.autoShoot )
				GetCL_Bypass()->SetDontAttack( cmd , true );
			EnginePred::End();
			return;
		}

		wantsAutoStop = config.autoStop;
		if ( config.earlyAutoStop && wantsAutoStop )
		{
			Movement::ApplyAutoStop(
				cmd ,
				localPawn ,
				GetCL_Weapons()->GetLocalActiveWeapon() ,
				Movement::AutoStopMode::Early ,
				true );
		}

		auto* targetPawn = reinterpret_cast<C_CSPlayerPawn*>( target.pawn );
		if ( !RageScan::PassesHitchance(
			target.angle ,
			target.pos ,
			cmd ,
			targetPawn ,
			target.pawn ,
			reinterpret_cast<uintptr_t>( localPawn ) ,
			localHandle ,
			settings.hitchance ) )
		{
			if ( config.autoShoot )
				GetCL_Bypass()->SetDontAttack( cmd , true );
			EnginePred::End();
			return;
		}

		blockAntiAim = true;

		if ( config.autoScope && !config.rapidFire )
		{
			if ( auto* vdata = GetCL_Weapons()->GetLocalWeaponVData() )
			{
				if ( vdata->m_WeaponType().m_Type == CSWeaponType_t::WEAPONTYPE_SNIPER_RIFLE && !localPawn->m_bIsScoped() )
					GetCL_Bypass()->SetButton( cmd , IN_SECOND_ATTACK , true );
			}
		}

		if ( wantsAutoStop )
		{
			const Movement::AutoStopMode mode = config.stopMode == RageScan::StopMode::Early
				? Movement::AutoStopMode::Early
				: Movement::AutoStopMode::Slow;

			Movement::ApplyAutoStop(
				cmd ,
				localPawn ,
				GetCL_Weapons()->GetLocalActiveWeapon() ,
				mode ,
				true );
		}

		Game::QAngle shootAngles( target.angle.pitch , target.angle.yaw , 0.f );
		ApplyAimpunch( shootAngles , localPawn );
		Aimbot::ClampAngles( shootAngles );

		auto* activeWeapon = GetCL_Weapons()->GetLocalActiveWeapon();
		if ( config.autoShoot && !CanShoot( localPawn , activeWeapon ) )
		{
			GetCL_Bypass()->SetDontAttack( cmd , true );
			EnginePred::End();
			return;
		}

		if ( target.usedBacktrack )
			RageSubTick::SetBacktrack( target.backtrackTick , target.backtrackFraction );

		if ( NoSpread::config.enabled
			|| ( config.usePerWeapon && WeaponConfig::GetRageSettingsForActiveWeapon().removeSpread ) )
		{
			::QAngle compensated( shootAngles.pitch , shootAngles.yaw , 0.f );
			::QAngle desired( shootAngles.pitch , shootAngles.yaw , 0.f );
			if ( NoSpread::CompensateAngles( desired , cmd , compensated ) )
				shootAngles = Game::QAngle( compensated.m_x , compensated.m_y , 0.f );
		}

		::QAngle sdkAngles( shootAngles.pitch , shootAngles.yaw , 0.f );

		if ( config.silentAim || config.rapidFire )
		{
			if ( config.rapidFire )
				RageSubTick::SetRapidFire( sdkAngles );
			else
				RageSubTick::SetSilentAim( sdkAngles );
		}

		GetCL_Bypass()->SetViewAngles(
			&sdkAngles ,
			input ,
			cmd ,
			!config.silentAim ,
			config.silentAim || config.rapidFire );

		if ( config.autoShoot && !config.rapidFire )
			GetCL_Bypass()->SetAttack( cmd , true );

		if ( config.delayAim && config.delayAimMs > 0 )
		{
			if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
				s_delayUntilTick = globalVars->m_nTickCount() + ( config.delayAimMs / 15 );
		}

		EnginePred::End();
	}
}

#pragma once

#include <cstdint>
#include <cmath>
#include <cfloat>

#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <Client/Features/Combat/Rage/RageScan.hpp>

namespace Ragebot
{
	struct Config
	{
		bool  enabled = false;
		int   activationKey = 0;
		bool  activationUseKey = false;
		bool  activationKeyHold = true;
		bool  activationShowInBinds = true;
		int   minDamage = 1;
		int   hitchance = 50;
		int   multipointScale = 70;
		bool  autoStop = true;
		bool  earlyAutoStop = true;
		bool  penetration = true;
		bool  safePoint = false;
		bool  adaptiveWeapon = true;
		bool  hitboxHead = true;
		bool  hitboxNeck = false;
		bool  hitboxChest = true;
		bool  hitboxUpperChest = false;
		bool  hitboxPelvis = false;
		bool  hitboxStomach = false;
		bool  hitboxArms = false;
		bool  hitboxLegs = false;
		bool  hitboxFeet = false;
		bool  teamCheck = true;
		bool  autoShoot = true;
		bool  silentAim = false;
		bool  backtrack = true;
		bool  rapidFire = false;
		bool  autoScope = true;
		bool  delayAim = false;
		int   delayAimMs = 0;
		bool  usePerWeapon = true;
		int   perWeaponClass = 0;
		RageScan::HitscanMode hitscanMode = RageScan::HitscanMode::Normal;
		RageScan::ScanMode      scanMode = RageScan::ScanMode::Adaptive;
		RageScan::StopMode      stopMode = RageScan::StopMode::Slow;
		RageScan::TargetSelect  targetSelect = RageScan::TargetSelect::Damage;
	};

	inline Config config;
	inline bool   blockAntiAim = false;
	inline bool   wantsAutoStop = false;

	auto OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd ) -> void;
	auto BuildScanSettings() noexcept -> RageScan::ScanSettings;
	auto OnRoundStart() noexcept -> void;
	auto OnPlayerDeath() noexcept -> void;
	auto ResetTarget() noexcept -> void;

} // namespace Ragebot

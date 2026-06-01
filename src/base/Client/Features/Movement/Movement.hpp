#pragma once

#include <Windows.h>
#include <cstdint>

#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

class CCSGOInput;
class CUserCmd;
class C_CSPlayerPawn;
class C_CSWeaponBaseGun;
class CCSPlayerController;

namespace Movement
{
	enum class StrafeMode : int
	{
		Off = 0 ,
		Legit = 1 ,
		Rage = 2 ,
	};

	enum class AutoStopMode : int
	{
		Slow = 0 ,
		Early = 1 ,
	};

	struct Config
	{
		bool       movementFix = true;
		bool       movementCorrection = true;
		bool       validateAngles = true;
		bool       edgeBug = false;
		bool       edgeBugUseKey = true;
		int        edgeBugKey = VK_XBUTTON1;
		float      strafeSmooth = 50.f;
		bool       strafeAssist = true;
		StrafeMode strafeMode = StrafeMode::Off;
	};

	inline Config config;

	auto Process( CCSGOInput* input , CUserCmd* cmd ) noexcept -> void;
	auto MovementFix( CUserCmd* cmd , float realYaw , float fakeYaw ) noexcept -> void;
	auto Shutdown() noexcept -> void;
	auto ApplyAutoStop(
		CUserCmd* cmd ,
		C_CSPlayerPawn* pawn ,
		C_CSWeaponBaseGun* weapon ,
		AutoStopMode mode ,
		bool active ) noexcept -> void;

} // namespace Movement

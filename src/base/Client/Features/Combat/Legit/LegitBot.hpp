#pragma once

#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/SDK/Interface/IGameEvent.hpp>

#include <Client/Features/Combat/Legit/Aimbot.hpp>

namespace LegitBot
{
	enum class Condition : unsigned int
	{
		None = 0 ,
		InAir = 1 << 0 ,
		Flashed = 1 << 1 ,
		InSmoke = 1 << 2 ,
		DelayShot = 1 << 3 ,
	};

	enum class TargetSelection : int
	{
		FovWeighted = 0 ,
		Crosshair = 1 ,
		Distance = 2 ,
	};

	struct Config
	{
		bool  enabled = true;
		int   fovType = 0;
		float fov = 5.f;
		float screenFov = 100.f;
		int   targetHitbox = 0;
		int   aimKey = VK_LBUTTON;
		bool  autoShoot = false;
		bool  silentAim = true;
		bool  teamCheck = true;
		bool  visCheck = true;
		bool  penetration = false;
		bool  recoilControl = false;
		int   smooth = 0;
		bool  noScope = false;
		bool  delayAim = false;
		int   delayAimMs = 0;
		unsigned int conditions = 0;
		bool  punchRandomization = false;
		bool  usePerWeapon = true;
		int   targetSelection = static_cast<int>( TargetSelection::FovWeighted );
		bool  hitboxHead = true;
		bool  hitboxNeck = false;
		bool  hitboxChest = true;
		bool  hitboxUpperChest = false;
		bool  hitboxPelvis = false;
		bool  hitboxStomach = false;
		bool  hitboxLegs = false;
		bool  hitboxArms = false;
		bool  triggerOverride = false;
		int   triggerHitchance = 0;
		bool  removeSpread = false;
		bool  fovVisualize = false;
		float fovVisualizeColor[4] = { 1.f , 0.35f , 0.35f , 0.35f };
		int   perWeaponClass = 0;
	};

	inline Config config;
	inline int s_killDelayTick = 0;
	inline bool s_triggerFromWeapon = false;

	auto OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd ) -> void;
	auto OnRoundStart() noexcept -> void;
	auto OnPlayerDeath( IGameEvent* event ) noexcept -> void;
	auto SyncToAimbot() noexcept -> void;
	auto SyncFromAimbot() noexcept -> void;
	auto ApplyWeaponSettings() noexcept -> void;
	auto PassesConditions( C_CSPlayerPawn* pawn ) noexcept -> bool;

} // namespace LegitBot

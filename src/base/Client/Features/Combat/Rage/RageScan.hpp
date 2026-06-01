#pragma once

#include <cstdint>
#include <vector>

#include <CS2/SDK/Math/Vector3.hpp>
#include <Client/Game/Game.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

class C_CSPlayerPawn;
class C_CSWeaponBaseGun;

namespace RageScan
{
	enum class HitscanMode : int
	{
		Normal = 0 ,
		Lethal = 1 ,
		Lethal2 = 3 ,
		Prefer = 4 ,
	};

	enum class ScanMode : int
	{
		Single = 0 ,
		Adaptive = 1 ,
	};

	enum class StopMode : int
	{
		Slow = 0 ,
		Early = 1 ,
	};

	enum class TargetSelect : int
	{
		Damage = 0 ,
		Fov = 1 ,
		Distance = 2 ,
	};

	struct PointResult
	{
		uintptr_t pawn = 0;
		Game::Vector3 pos{};
		Game::QAngle  angle{};
		float     damage = 0.f;
		int       hitbox = 0;
		uint32_t  studioHitbox = 0;
		float     fov = 0.f;
		float     distance = 0.f;
		bool      safe = false;
		bool      lethal = false;
		bool      usedBacktrack = false;
		int       backtrackTick = 0;
		float     backtrackFraction = 0.f;
		float     backtrackSimTime = 0.f;
	};

	struct ScanSettings
	{
		int          minDamage = 1;
		int          hitchance = 50;
		int          multipointScale = 70;
		bool         penetration = true;
		bool         safePoint = false;
		bool         backtrack = true;
		bool         hitboxHead = true;
		bool         hitboxNeck = false;
		bool         hitboxChest = true;
		bool         hitboxPelvis = false;
		bool         hitboxStomach = false;
		bool         hitboxArms = false;
		bool         hitboxLegs = false;
		bool         hitboxUpperChest = false;
		bool         hitboxFeet = false;
		HitscanMode  hitscanMode = HitscanMode::Normal;
		ScanMode     scanMode = ScanMode::Adaptive;
	};

	auto ApplyAdaptiveWeapon( ScanSettings& settings ) noexcept -> void;
		const ::Vector3& eye ,
		const ::Vector3& point ,
		uint32_t studioHitbox ,
		int targetHealth ,
		C_CSPlayerPawn* targetPawn ,
		uintptr_t targetAddr ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		const ScanSettings& settings ,
		PointResult& out ) noexcept -> bool;

	auto ScanBestTarget(
		const ::Vector3& eye ,
		const Game::QAngle& viewAngles ,
		const ScanSettings& settings ,
		bool teamCheck ,
		TargetSelect targetSelect ,
		PointResult& best ) noexcept -> bool;

	auto PassesHitchance(
		const Game::QAngle& aimAngles ,
		const Game::Vector3& targetPoint ,
		CUserCmd* cmd ,
		C_CSPlayerPawn* targetPawn ,
		uintptr_t targetAddr ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		int hitchancePercent ) noexcept -> bool;

	auto ResetTarget() noexcept -> void;
	auto GetLastTarget() noexcept -> uintptr_t;

} // namespace RageScan

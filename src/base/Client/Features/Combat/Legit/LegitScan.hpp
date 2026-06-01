#pragma once

#include <cstdint>
#include <vector>

#include <Client/Game/Game.hpp>
#include <CS2/SDK/Math/Vector3.hpp>

class CUserCmd;
class C_CSPlayerPawn;
class C_CSWeaponBaseGun;

namespace LegitScan
{
	struct HitboxFilter
	{
		bool head = true;
		bool neck = false;
		bool chest = true;
		bool upperChest = false;
		bool pelvis = false;
		bool stomach = false;
		bool legs = false;
		bool arms = false;
		int  legacyFallback = 0;
	};

	auto CollectHitboxes( const HitboxFilter& filter , std::vector<uint32_t>& out ) noexcept -> void;

	auto IsValidTarget( C_CSPlayerPawn* pawn ) noexcept -> bool;

	auto GetHitboxPosition( C_CSPlayerPawn* pawn , uint32_t studioIndex ) noexcept -> ::Vector3;

	auto CanShoot( C_CSPlayerPawn* localPawn , C_CSWeaponBaseGun* weapon ) noexcept -> bool;

	auto IsSmokeBetween( const ::Vector3& start , const ::Vector3& end ) noexcept -> bool;

	auto CalcDrawFovRadius( float fovDegrees , float screenHeight ) noexcept -> float;

	auto PassesHitchance(
		const Game::QAngle& viewAngles ,
		const Game::Vector3& targetPoint ,
		CUserCmd* cmd ,
		uintptr_t targetAddr ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		int hitchancePercent ) noexcept -> bool;

} // namespace LegitScan

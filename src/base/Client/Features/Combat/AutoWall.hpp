#pragma once

#include <CS2/SDK/Math/Vector3.hpp>

class C_CSPlayerPawn;
class CCSWeaponBaseVData;

namespace AutoWall
{
	enum HitGroup : int
	{
		HITGROUP_GENERIC = 0 ,
		HITGROUP_HEAD = 1 ,
		HITGROUP_CHEST = 2 ,
		HITGROUP_STOMACH = 3 ,
		HITGROUP_LEFTARM = 4 ,
		HITGROUP_RIGHTARM = 5 ,
		HITGROUP_LEFTLEG = 6 ,
		HITGROUP_RIGHTLEG = 7 ,
		HITGROUP_NECK = 8 ,
	};

	struct PenetrationResult
	{
		bool canHit = false;
		float damage = 0.f;
	};

	auto ScaleDamage(
		float& damage ,
		int hitGroup ,
		C_CSPlayerPawn* target ,
		CCSWeaponBaseVData* weaponVData ) noexcept -> void;

	auto FireBullet(
		const Vector3& localPos ,
		const Vector3& targetPos ,
		C_CSPlayerPawn* localPawn ,
		C_CSPlayerPawn* targetPawn ,
		CCSWeaponBaseVData* weaponVData ,
		float& outDamage ,
		bool& outValid ) noexcept -> bool;

	auto CanPenetrate(
		const Vector3& localPos ,
		const Vector3& targetPos ,
		C_CSPlayerPawn* targetPawn ,
		CCSWeaponBaseVData* weaponVData ) noexcept -> PenetrationResult;
}

#pragma once

#include <Windows.h>
#include <cstdint>

class CCSWeaponBaseVData;

namespace WeaponConfig
{
	enum class WeaponClass : int
	{
		Default = 0 ,
		Pistol ,
		HeavyPistol ,
		AssaultRifle ,
		Auto ,
		Scout ,
		Awp ,
		Count ,
	};

	inline constexpr const char* kWeaponClassNames[] = {
		"Default" ,
		"Pistol" ,
		"Heavy Pistol" ,
		"Assault Rifle" ,
		"Auto" ,
		"Scout" ,
		"AWP" ,
	};

	inline constexpr int kWeaponClassCount = static_cast<int>( WeaponClass::Count );

	struct LegitSettings
	{
		int   fov = 5;
		int   aimKey = VK_LBUTTON;
		int   smooth = 0;
		int   targetSelection = 0;
		bool  rcs = false;
		bool  rcsShotsEnable = false;
		int   rcsShots = 0;
		float rcsSmoothX = 0.f;
		float rcsSmoothY = 0.f;
		bool  punchRandomization = false;
		float punchRandomX = 0.f;
		float punchRandomY = 0.f;
		bool  visCheck = true;
		bool  hitboxHead = true;
		bool  hitboxNeck = false;
		bool  hitboxChest = true;
		bool  hitboxUpperChest = false;
		bool  hitboxStomach = false;
		bool  hitboxLegs = false;
		bool  hitboxPelvis = false;
		bool  hitboxArms = false;
		bool  triggerEnabled = false;
		int   triggerHitchance = 0;
		bool  removeSpread = false;
	};

	struct RageSettings
	{
		int  minDamage = 1;
		int  hitchance = 50;
		int  targetSelect = 0;
		bool rapidFire = false;
		bool autoScope = true;
		bool penetration = true;
		bool autoStop = true;
		bool safePoint = false;
		bool earlyAutoStop = true;
		bool hitboxHead = true;
		bool hitboxNeck = false;
		bool hitboxChest = true;
		bool hitboxUpperChest = false;
		bool hitboxPelvis = false;
		bool hitboxStomach = false;
		bool hitboxArms = false;
		bool hitboxLegs = false;
		bool hitboxFeet = false;
		bool removeSpread = false;
	};

	inline LegitSettings s_legitDefault{};
	inline LegitSettings s_legitPistol{};
	inline LegitSettings s_legitHeavyPistol{};
	inline LegitSettings s_legitAssaultRifle{};
	inline LegitSettings s_legitAuto{};
	inline LegitSettings s_legitScout{};
	inline LegitSettings s_legitAwp{};

	inline RageSettings s_rageDefault{};
	inline RageSettings s_ragePistol{};
	inline RageSettings s_rageHeavyPistol{};
	inline RageSettings s_rageAssaultRifle{};
	inline RageSettings s_rageAuto{};
	inline RageSettings s_rageScout{};
	inline RageSettings s_rageAwp{};

	auto ClassifyWeapon( CCSWeaponBaseVData* vdata , int defIndex = -1 ) noexcept -> WeaponClass;
	auto GetLegitSettings( WeaponClass cls ) noexcept -> LegitSettings&;
	auto GetRageSettings( WeaponClass cls ) noexcept -> RageSettings&;
	auto GetLegitSettingsForActiveWeapon() noexcept -> LegitSettings&;
	auto GetRageSettingsForActiveWeapon() noexcept -> RageSettings&;

} // namespace WeaponConfig

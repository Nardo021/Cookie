#include "WeaponConfig.hpp"

#include <algorithm>

#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

#include <GameClient/CL_Weapons.hpp>

namespace WeaponConfig
{
	auto ClassifyWeapon( CCSWeaponBaseVData* vdata , int defIndex ) noexcept -> WeaponClass
	{
		if ( defIndex < 0 )
			defIndex = GetCL_Weapons()->GetLocalWeaponDefinitionIndex();

		switch ( defIndex )
		{
		case 9: return WeaponClass::Awp;
		case 40: return WeaponClass::Scout;
		case 1:
		case 64: return WeaponClass::HeavyPistol;
		default: break;
		}

		if ( !vdata )
			return WeaponClass::Default;

		switch ( vdata->m_WeaponType().m_Type )
		{
		case CSWeaponType_t::WEAPONTYPE_PISTOL:
			return WeaponClass::Pistol;
		case CSWeaponType_t::WEAPONTYPE_MACHINEGUN:
		case CSWeaponType_t::WEAPONTYPE_SUBMACHINEGUN:
			return WeaponClass::Auto;
		case CSWeaponType_t::WEAPONTYPE_RIFLE:
		case CSWeaponType_t::WEAPONTYPE_SHOTGUN:
			return WeaponClass::AssaultRifle;
		case CSWeaponType_t::WEAPONTYPE_SNIPER_RIFLE:
			return WeaponClass::Scout;
		default:
			return WeaponClass::Default;
		}
	}

	auto GetLegitSettings( WeaponClass cls ) noexcept -> LegitSettings&
	{
		switch ( cls )
		{
		case WeaponClass::Pistol: return s_legitPistol;
		case WeaponClass::HeavyPistol: return s_legitHeavyPistol;
		case WeaponClass::AssaultRifle: return s_legitAssaultRifle;
		case WeaponClass::Auto: return s_legitAuto;
		case WeaponClass::Scout: return s_legitScout;
		case WeaponClass::Awp: return s_legitAwp;
		default: return s_legitDefault;
		}
	}

	auto GetRageSettings( WeaponClass cls ) noexcept -> RageSettings&
	{
		switch ( cls )
		{
		case WeaponClass::Pistol: return s_ragePistol;
		case WeaponClass::HeavyPistol: return s_rageHeavyPistol;
		case WeaponClass::AssaultRifle: return s_rageAssaultRifle;
		case WeaponClass::Auto: return s_rageAuto;
		case WeaponClass::Scout: return s_rageScout;
		case WeaponClass::Awp: return s_rageAwp;
		default: return s_rageDefault;
		}
	}

	auto GetLegitSettingsForActiveWeapon() noexcept -> LegitSettings&
	{
		return GetLegitSettings( ClassifyWeapon( GetCL_Weapons()->GetLocalWeaponVData() ) );
	}

	auto GetRageSettingsForActiveWeapon() noexcept -> RageSettings&
	{
		return GetRageSettings( ClassifyWeapon( GetCL_Weapons()->GetLocalWeaponVData() ) );
	}
}

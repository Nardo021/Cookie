#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Combat/Legit/LegitBot.hpp>
#include <Client/Features/Combat/Legit/Triggerbot.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Combat/WeaponConfig.hpp>

namespace SyntheticTabs
{
	auto RenderRageTab() noexcept -> void
	{
		using namespace SyntheticUI;

		gui->begin_group();
		{
			gui->begin_child( "Ragebot" );
			{
				if ( Checkbox( "Enable Ragebot" , &Ragebot::config.enabled ) && Ragebot::config.enabled )
				{
					Aimbot::config.enabled = false;
					Triggerbot::config.enabled = false;
					LegitBot::config.enabled = false;
					LegitBot::SyncFromAimbot();
				}

				Separator();

				static const auto hitboxApi = Strings( { "V1 Bones" , "V2 Native" } );
				Combo( "Hitbox API" , reinterpret_cast<int*>( &HitboxData::config.mode ) , hitboxApi );
				if ( HitboxData::config.mode == HitboxData::Mode::V2_Native && !HitboxData::IsNativeReady() )
				{
					draw->render_text(
						GetWindowDrawList() ,
						set->c_font.inter_medium[0] ,
						GetWindowPos() + SCALE( 0 , GetCursorPosY() ) ,
						GetWindowPos() + GetWindowSize() ,
						gui->get_clr( clr->c_other_clr.accent_clr ) ,
						"V2 native unavailable — using V1 fallback." ,
						nullptr ,
						nullptr ,
						ImVec2( 0.f , 0.f ) );
				}

				if ( Ragebot::config.enabled )
				{
					Separator();
					SliderInt( "Min Damage" , &Ragebot::config.minDamage , 1 , 100 , 1 , "%d" );
					SliderInt( "Hitchance" , &Ragebot::config.hitchance , 0 , 100 , 1 , "%d%%" );
					SliderInt( "Multipoint Scale" , &Ragebot::config.multipointScale , 0 , 100 , 1 , "%d%%" );
					Checkbox( "Auto Stop" , &Ragebot::config.autoStop );
					Checkbox( "Early Auto Stop" , &Ragebot::config.earlyAutoStop );
					Checkbox( "Penetration" , &Ragebot::config.penetration );
					Checkbox( "Safe Point" , &Ragebot::config.safePoint );
					Checkbox( "Adaptive Weapon" , &Ragebot::config.adaptiveWeapon );
					Checkbox( "Auto Shoot" , &Ragebot::config.autoShoot );
					Checkbox( "Silent Aim" , &Ragebot::config.silentAim );
					Checkbox( "No Spread" , &NoSpread::config.enabled );
					Checkbox( "Rapid Fire" , &Ragebot::config.rapidFire );
					Checkbox( "Auto Scope" , &Ragebot::config.autoScope );
					Checkbox( "Delay Aim" , &Ragebot::config.delayAim );
					if ( Ragebot::config.delayAim )
						SliderInt( "Delay Aim (ms)" , &Ragebot::config.delayAimMs , 0 , 500 , 1 , "%d" );
					Checkbox( "Backtrack" , &Ragebot::config.backtrack );
					Checkbox( "Backtrack Debug" , &LagComp::debugConfig.drawBacktrack );
					if ( LagComp::debugConfig.drawBacktrack )
						widget->color_edit( "Backtrack Color" , LagComp::debugConfig.color );
					Checkbox( "Team Check" , &Ragebot::config.teamCheck );

					Separator();
					static const auto hitscanModes = Strings( { "Normal" , "Lethal" , "Lethal+" , "Prefer Lethal" } );
					Combo( "Hitscan Mode" , reinterpret_cast<int*>( &Ragebot::config.hitscanMode ) , hitscanModes );

					static const auto scanModes = Strings( { "Single Point" , "Adaptive Multipoint" } );
					Combo( "Scan Mode" , reinterpret_cast<int*>( &Ragebot::config.scanMode ) , scanModes );

					static const auto stopModes = Strings( { "Slow (Counter-Strafe)" , "Early (Hard Stop)" } );
					Combo( "Stop Mode" , reinterpret_cast<int*>( &Ragebot::config.stopMode ) , stopModes );

					static const auto targetModes = Strings( { "Highest Damage" , "Lowest FOV" , "Lowest Distance" } );
					Combo( "Target Select" , reinterpret_cast<int*>( &Ragebot::config.targetSelect ) , targetModes );

					Separator();
					Checkbox( "Head" , &Ragebot::config.hitboxHead );
					Checkbox( "Neck" , &Ragebot::config.hitboxNeck );
					Checkbox( "Chest" , &Ragebot::config.hitboxChest );
					Checkbox( "Upper Chest" , &Ragebot::config.hitboxUpperChest );
					Checkbox( "Pelvis" , &Ragebot::config.hitboxPelvis );
					Checkbox( "Stomach" , &Ragebot::config.hitboxStomach );
					Checkbox( "Arms" , &Ragebot::config.hitboxArms );
					Checkbox( "Legs" , &Ragebot::config.hitboxLegs );
					Checkbox( "Feet" , &Ragebot::config.hitboxFeet );
				}
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Per-Weapon" );
			{
				Checkbox( "Use Per-Weapon Settings" , &Ragebot::config.usePerWeapon );
				if ( Ragebot::config.usePerWeapon )
				{
					static std::vector<std::string> classNames;
					if ( classNames.empty() )
					{
						for ( int i = 0; i < WeaponConfig::kWeaponClassCount; ++i )
							classNames.emplace_back( WeaponConfig::kWeaponClassNames[i] );
					}
					Combo( "Weapon Class" , &Ragebot::config.perWeaponClass , classNames );

					WeaponConfig::RageSettings& ws = WeaponConfig::GetRageSettings(
						static_cast<WeaponConfig::WeaponClass>( Ragebot::config.perWeaponClass ) );
					SliderInt( "Class Min Damage" , &ws.minDamage , 1 , 100 , 1 , "%d" );
					SliderInt( "Class Hitchance" , &ws.hitchance , 0 , 100 , 1 , "%d%%" );
					Checkbox( "Class Auto Stop" , &ws.autoStop );
					Checkbox( "Class Early Auto Stop" , &ws.earlyAutoStop );
					Checkbox( "Class Penetration" , &ws.penetration );
					Checkbox( "Class Safe Point" , &ws.safePoint );
				}
			}
			gui->end_child();
		}
		gui->end_group();
	}
}

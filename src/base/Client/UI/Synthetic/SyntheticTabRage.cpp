#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Combat/Legit/LegitBot.hpp>
#include <Client/Features/Combat/Legit/Triggerbot.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Combat/Legit/WeaponConfig.hpp>
#include <Client/UI/Synthetic/SyntheticBinds.hpp>

namespace SyntheticTabs
{
	auto RenderRageTab() noexcept -> void
	{
		using namespace SyntheticUI;

		const SyntheticBinds::KeyBindUiState rageBind{
			&Ragebot::config.activationKeyHold ,
			&Ragebot::config.activationUseKey ,
			&Ragebot::config.activationShowInBinds ,
		};

		gui->begin_group();
		{
			gui->begin_child( "Ragebot" );
			{
				if ( UiCheckboxWithKey( "Enable Ragebot" , &Ragebot::config.enabled , &Ragebot::config.activationKey , rageBind )
					&& Ragebot::config.enabled )
				{
					Aimbot::config.enabled = false;
					Triggerbot::config.enabled = false;
					LegitBot::config.enabled = false;
					LegitBot::SyncFromAimbot();
				}

				UiSeparator();

				static const auto hitboxApi = ItemStrings( { "V1 Bones" , "V2 Native" } );
				UiCombo( "Hitbox API" , reinterpret_cast<int*>( &HitboxData::config.mode ) , hitboxApi );
				widget->set_tooltip(
					"Hitbox API" ,
					"V1 uses studio bones; V2 uses native hitbox data when patterns are available." );
				if ( HitboxData::config.mode == HitboxData::Mode::V2_Native && !HitboxData::IsNativeReady() )
				{
					draw->render_text(
						GetWindowDrawList() ,
						set->c_font.inter_medium[0] ,
						GetWindowPos() + SCALE( 0 , GetCursorPosY() ) ,
						GetWindowPos() + GetWindowSize() ,
						gui->get_clr( clr->c_other_clr.accent_clr ) ,
						"V2 native unavailable 鈥?using V1 fallback." ,
						nullptr ,
						nullptr ,
						ImVec2( 0.f , 0.f ) );
				}

				if ( Ragebot::config.enabled )
				{
					UiSeparator();
					UiSliderInt( "Min Damage" , &Ragebot::config.minDamage , 1 , 100 , 1 , "%d" );
					UiSliderInt( "Hitchance" , &Ragebot::config.hitchance , 0 , 100 , 1 , "%d%%" );
					UiSliderInt( "Multipoint Scale" , &Ragebot::config.multipointScale , 0 , 100 , 1 , "%d%%" );
					UiCheckbox( "Auto Stop" , &Ragebot::config.autoStop );
					UiCheckbox( "Early Auto Stop" , &Ragebot::config.earlyAutoStop );
					UiCheckbox( "Penetration" , &Ragebot::config.penetration );
					UiCheckbox( "Safe Point" , &Ragebot::config.safePoint );
					UiCheckbox( "Adaptive Weapon" , &Ragebot::config.adaptiveWeapon );
					UiCheckbox( "Auto Shoot" , &Ragebot::config.autoShoot );
					UiCheckbox( "Silent Aim" , &Ragebot::config.silentAim );
					UiCheckbox( "No Spread" , &NoSpread::config.enabled );
					UiCheckbox( "Rapid Fire" , &Ragebot::config.rapidFire );
					UiCheckbox( "Auto Scope" , &Ragebot::config.autoScope );
					UiCheckbox( "Delay Aim" , &Ragebot::config.delayAim );
					if ( Ragebot::config.delayAim )
						UiSliderInt( "Delay Aim (ms)" , &Ragebot::config.delayAimMs , 0 , 500 , 1 , "%d" );
					UiCheckbox( "Backtrack" , &Ragebot::config.backtrack );
					UiCheckbox( "Backtrack Debug" , &LagComp::debugConfig.drawBacktrack );
					if ( LagComp::debugConfig.drawBacktrack )
						widget->color_edit( "Backtrack Color" , LagComp::debugConfig.color );
					UiCheckbox( "Team Check" , &Ragebot::config.teamCheck );

					UiSeparator();
					static const auto hitscanModes = ItemStrings( { "Normal" , "Lethal" , "Lethal+" , "Prefer Lethal" } );
					UiCombo( "Hitscan Mode" , reinterpret_cast<int*>( &Ragebot::config.hitscanMode ) , hitscanModes );

					static const auto scanModes = ItemStrings( { "Single Point" , "Adaptive Multipoint" } );
					UiCombo( "Scan Mode" , reinterpret_cast<int*>( &Ragebot::config.scanMode ) , scanModes );

					static const auto stopModes = ItemStrings( { "Slow (Counter-Strafe)" , "Early (Hard Stop)" } );
					UiCombo( "Stop Mode" , reinterpret_cast<int*>( &Ragebot::config.stopMode ) , stopModes );

					static const auto targetModes = ItemStrings( { "Highest Damage" , "Lowest FOV" , "Lowest Distance" } );
					UiCombo( "Target Select" , reinterpret_cast<int*>( &Ragebot::config.targetSelect ) , targetModes );

					UiSeparator();
					UiCheckbox( "Head" , &Ragebot::config.hitboxHead );
					UiCheckbox( "Neck" , &Ragebot::config.hitboxNeck );
					UiCheckbox( "Chest" , &Ragebot::config.hitboxChest );
					UiCheckbox( "Upper Chest" , &Ragebot::config.hitboxUpperChest );
					UiCheckbox( "Pelvis" , &Ragebot::config.hitboxPelvis );
					UiCheckbox( "Stomach" , &Ragebot::config.hitboxStomach );
					UiCheckbox( "Arms" , &Ragebot::config.hitboxArms );
					UiCheckbox( "Legs" , &Ragebot::config.hitboxLegs );
					UiCheckbox( "Feet" , &Ragebot::config.hitboxFeet );
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
				UiCheckbox( "Use Per-Weapon Settings" , &Ragebot::config.usePerWeapon );
				if ( Ragebot::config.usePerWeapon )
				{
					static std::vector<std::string> classNames;
					if ( classNames.empty() )
					{
						for ( int i = 0; i < WeaponConfig::kWeaponClassCount; ++i )
							classNames.emplace_back( WeaponConfig::kWeaponClassNames[i] );
					}
					UiCombo( "Weapon Class" , &Ragebot::config.perWeaponClass , classNames );

					WeaponConfig::RageSettings& ws = WeaponConfig::GetRageSettings(
						static_cast<WeaponConfig::WeaponClass>( Ragebot::config.perWeaponClass ) );
					UiSliderInt( "Class Min Damage" , &ws.minDamage , 1 , 100 , 1 , "%d" );
					UiSliderInt( "Class Hitchance" , &ws.hitchance , 0 , 100 , 1 , "%d%%" );
					UiCheckbox( "Class Auto Stop" , &ws.autoStop );
					UiCheckbox( "Class Early Auto Stop" , &ws.earlyAutoStop );
					UiCheckbox( "Class Penetration" , &ws.penetration );
					UiCheckbox( "Class Safe Point" , &ws.safePoint );
				}
			}
			gui->end_child();
		}
		gui->end_group();
	}
}


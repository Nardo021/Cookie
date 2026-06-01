#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Combat/Legit/LegitBot.hpp>
#include <Client/Features/Combat/Legit/Triggerbot.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Combat/Legit/WeaponConfig.hpp>
#include <Client/UI/Synthetic/SyntheticBinds.hpp>

namespace SyntheticTabs
{
	using namespace SyntheticUI;

	namespace
	{
		auto SyncLegitConditions( unsigned int& conditions ) noexcept -> void
		{
			bool inAir = ( conditions & static_cast<unsigned int>( LegitBot::Condition::InAir ) ) != 0;
			bool flashed = ( conditions & static_cast<unsigned int>( LegitBot::Condition::Flashed ) ) != 0;
			bool inSmoke = ( conditions & static_cast<unsigned int>( LegitBot::Condition::InSmoke ) ) != 0;
			bool delayShot = ( conditions & static_cast<unsigned int>( LegitBot::Condition::DelayShot ) ) != 0;

			if ( UiCheckbox( "Only On Ground" , &inAir ) )
				conditions = inAir
					? ( conditions | static_cast<unsigned int>( LegitBot::Condition::InAir ) )
					: ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::InAir ) );
			if ( UiCheckbox( "Not Flashed" , &flashed ) )
				conditions = flashed
					? ( conditions | static_cast<unsigned int>( LegitBot::Condition::Flashed ) )
					: ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::Flashed ) );
			if ( UiCheckbox( "Not In Smoke" , &inSmoke ) )
				conditions = inSmoke
					? ( conditions | static_cast<unsigned int>( LegitBot::Condition::InSmoke ) )
					: ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::InSmoke ) );
			if ( UiCheckbox( "Delay After Kill" , &delayShot ) )
				conditions = delayShot
					? ( conditions | static_cast<unsigned int>( LegitBot::Condition::DelayShot ) )
					: ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::DelayShot ) );
		}
	}

	auto RenderLegitTab() noexcept -> void
	{
		using namespace SyntheticUI;

		if ( Ragebot::config.enabled )
		{
			gui->begin_child( "Legit" );
			draw->render_text(
				GetWindowDrawList() ,
				set->c_font.inter_medium[0] ,
				GetWindowPos() ,
				GetWindowPos() + GetWindowSize() ,
				gui->get_clr( clr->c_text.text ) ,
				"Disable Ragebot to edit Legit settings." ,
				nullptr ,
				nullptr ,
				ImVec2( 0.f , 0.f ) );
			gui->end_child();
			return;
		}

		const SyntheticBinds::KeyBindUiState aimBind{
			&Aimbot::config.aimKeyHold ,
			&Aimbot::config.aimUseKey ,
			&Aimbot::config.aimShowInBinds ,
		};
		const SyntheticBinds::KeyBindUiState triggerBind{
			&Triggerbot::config.keyHold ,
			&Triggerbot::config.useKey ,
			&Triggerbot::config.showInBinds ,
		};

		gui->begin_group();
		{
			gui->begin_child( "Aimbot" );
			{
				if ( UiCheckboxWithKey( "Enable Aimbot" , &Aimbot::config.enabled , &Aimbot::config.aimKey , aimBind )
					&& Aimbot::config.enabled )
					Ragebot::config.enabled = false;
				LegitBot::SyncFromAimbot();

				UiSeparator();
				UiCheckbox( "Auto Shoot" , &Aimbot::config.autoShoot );
				UiCheckbox( "Silent Aim" , &Aimbot::config.silentAim );
				UiCheckbox( "No Spread" , &NoSpread::config.enabled );
				UiCheckbox( "Recoil Control" , &Aimbot::config.recoilControl );
				UiSliderInt( "Smooth" , &Aimbot::config.smooth , 0 , 20 , 1 , "%d" );
				UiCheckbox( "Punch Randomization" , &Aimbot::config.punchRandomization );
				UiCheckbox( "FOV Visualize" , &Aimbot::config.fovVisualize );
				if ( Aimbot::config.fovVisualize )
					widget->color_edit( "FOV Color" , Aimbot::config.fovVisualizeColor );

				UiSeparator();
				static const auto fovTypes = ItemStrings( { "Angle (Degrees)" , "Screen (Pixels)" } );
				UiCombo( "FOV Mode" , &Aimbot::config.fovType , fovTypes );
				widget->set_tooltip( "FOV Mode" , "Angle FOV is view-angle based; Screen FOV is pixel radius on screen." );
				if ( Aimbot::config.fovType == 0 )
					UiSliderFloat( "FOV" , &Aimbot::config.fov , 1.f , 180.f , 0.5f , "%.1f deg" );
				else
					UiSliderFloat( "FOV" , &Aimbot::config.screenFov , 10.f , 1000.f , 1.f , "%.0f px" );

				static const auto bones = ItemStrings( { "Head" , "Neck" , "Chest" , "Pelvis" } );
				UiCombo( "Hitbox" , &Aimbot::config.targetHitbox , bones );

				static const auto targetModes = ItemStrings( { "FOV + Distance" , "Crosshair" , "Distance" } );
				UiCombo( "Target Select" , &Aimbot::config.targetSelection , targetModes );

				UiSeparator();
				UiCheckbox( "Head##Legit" , &Aimbot::config.hitboxHead );
				UiCheckbox( "Neck##Legit" , &Aimbot::config.hitboxNeck );
				UiCheckbox( "Chest##Legit" , &Aimbot::config.hitboxChest );
				UiCheckbox( "Upper Chest##Legit" , &Aimbot::config.hitboxUpperChest );
				UiCheckbox( "Stomach##Legit" , &Aimbot::config.hitboxStomach );
				UiCheckbox( "Pelvis##Legit" , &Aimbot::config.hitboxPelvis );
				UiCheckbox( "Legs##Legit" , &Aimbot::config.hitboxLegs );
				UiCheckbox( "Arms##Legit" , &Aimbot::config.hitboxArms );
				if ( Aimbot::config.punchRandomization )
				{
					UiSliderFloat( "Punch Rand X" , &Aimbot::config.punchRandomX , 0.f , 1.f , 0.02f , "%.2f" );
					UiSliderFloat( "Punch Rand Y" , &Aimbot::config.punchRandomY , 0.f , 1.f , 0.02f , "%.2f" );
				}
				UiCheckbox( "Team Check" , &Aimbot::config.teamCheck );
				UiCheckbox( "Visible Only" , &Aimbot::config.visCheck );
				UiCheckbox( "Penetration" , &Aimbot::config.penetration );
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Trigger & Weapon" );
			{
				UiCheckboxWithKey( "Enable Triggerbot" , &Triggerbot::config.enabled , &Triggerbot::config.key , triggerBind );
				UiCheckbox( "Team Check (Trigger)" , &Triggerbot::config.teamCheck );
				UiCheckbox( "Visible Only (Trigger)" , &Triggerbot::config.visCheck );
				UiSliderInt( "Trigger Hitchance" , &Triggerbot::config.hitchance , 0 , 100 , 1 , "%d%%" );
				UiSliderInt( "Delay (ms)" , &Triggerbot::config.delayMs , 0 , 200 , 1 , "%d" );

				UiSeparator();
				UiCheckbox( "No Scope Silent" , &LegitBot::config.noScope );
				UiCheckbox( "Delay Aim" , &LegitBot::config.delayAim );
				if ( LegitBot::config.delayAim )
					UiSliderInt( "Delay Aim (ms)" , &LegitBot::config.delayAimMs , 0 , 500 , 1 , "%d" );

				unsigned int conditions = LegitBot::config.conditions;
				SyncLegitConditions( conditions );
				LegitBot::config.conditions = conditions;

				UiSeparator();
				UiCheckbox( "Use Per-Weapon Settings" , &LegitBot::config.usePerWeapon );
				if ( LegitBot::config.usePerWeapon )
				{
					static std::vector<std::string> classNames;
					if ( classNames.empty() )
					{
						for ( int i = 0; i < WeaponConfig::kWeaponClassCount; ++i )
							classNames.emplace_back( WeaponConfig::kWeaponClassNames[i] );
					}
					UiCombo( "Weapon Class" , &LegitBot::config.perWeaponClass , classNames );

					WeaponConfig::LegitSettings& ws = WeaponConfig::GetLegitSettings(
						static_cast<WeaponConfig::WeaponClass>( LegitBot::config.perWeaponClass ) );
					UiSliderInt( "Class FOV" , &ws.fov , 1 , 30 , 1 , "%d" );
					UiSliderInt( "Class Smooth" , &ws.smooth , 0 , 20 , 1 , "%d" );
					static const auto classTargetModes = ItemStrings( { "FOV + Distance" , "Crosshair" , "Distance" } );
					UiCombo( "Class Target Select" , &ws.targetSelection , classTargetModes );
					UiCheckbox( "Class RCS" , &ws.rcs );
					UiCheckbox( "Class Trigger Override" , &ws.triggerEnabled );
					if ( ws.triggerEnabled )
						UiSliderInt( "Class Trigger HC" , &ws.triggerHitchance , 0 , 100 , 1 , "%d%%" );
				}
			}
			gui->end_child();
		}
		gui->end_group();

		LegitBot::SyncFromAimbot();
	}
}


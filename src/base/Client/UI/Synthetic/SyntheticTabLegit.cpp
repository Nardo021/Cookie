#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Combat/Legit/LegitBot.hpp>
#include <Client/Features/Combat/Legit/Triggerbot.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Combat/WeaponConfig.hpp>

namespace SyntheticTabs
{
	namespace
	{
		auto SyncLegitConditions( unsigned int& conditions ) noexcept -> void
		{
			bool inAir = ( conditions & static_cast<unsigned int>( LegitBot::Condition::InAir ) ) != 0;
			bool flashed = ( conditions & static_cast<unsigned int>( LegitBot::Condition::Flashed ) ) != 0;
			bool inSmoke = ( conditions & static_cast<unsigned int>( LegitBot::Condition::InSmoke ) ) != 0;
			bool delayShot = ( conditions & static_cast<unsigned int>( LegitBot::Condition::DelayShot ) ) != 0;

			if ( Checkbox( "Only On Ground" , &inAir ) )
				conditions = inAir
					? ( conditions | static_cast<unsigned int>( LegitBot::Condition::InAir ) )
					: ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::InAir ) );
			if ( Checkbox( "Not Flashed" , &flashed ) )
				conditions = flashed
					? ( conditions | static_cast<unsigned int>( LegitBot::Condition::Flashed ) )
					: ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::Flashed ) );
			if ( Checkbox( "Not In Smoke" , &inSmoke ) )
				conditions = inSmoke
					? ( conditions | static_cast<unsigned int>( LegitBot::Condition::InSmoke ) )
					: ( conditions & ~static_cast<unsigned int>( LegitBot::Condition::InSmoke ) );
			if ( Checkbox( "Delay After Kill" , &delayShot ) )
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

		gui->begin_group();
		{
			gui->begin_child( "Aimbot" );
			{
				if ( Checkbox( "Enable Aimbot" , &Aimbot::config.enabled ) && Aimbot::config.enabled )
					Ragebot::config.enabled = false;
				LegitBot::SyncFromAimbot();

				Separator();
				KeyBind( "Aim Key" , &Aimbot::config.aimKey );
				Checkbox( "Auto Shoot" , &Aimbot::config.autoShoot );
				Checkbox( "Silent Aim" , &Aimbot::config.silentAim );
				Checkbox( "No Spread" , &NoSpread::config.enabled );
				Checkbox( "Recoil Control" , &Aimbot::config.recoilControl );
				SliderInt( "Smooth" , &Aimbot::config.smooth , 0 , 20 , 1 , "%d" );
				Checkbox( "Punch Randomization" , &Aimbot::config.punchRandomization );
				Checkbox( "FOV Visualize" , &Aimbot::config.fovVisualize );
				if ( Aimbot::config.fovVisualize )
					widget->color_edit( "FOV Color" , Aimbot::config.fovVisualizeColor );

				Separator();
				static const auto fovTypes = Strings( { "Angle (Degrees)" , "Screen (Pixels)" } );
				Combo( "FOV Mode" , &Aimbot::config.fovType , fovTypes );
				if ( Aimbot::config.fovType == 0 )
					SliderFloat( "FOV" , &Aimbot::config.fov , 1.f , 180.f , 0.5f , "%.1f deg" );
				else
					SliderFloat( "FOV" , &Aimbot::config.screenFov , 10.f , 1000.f , 1.f , "%.0f px" );

				static const auto bones = Strings( { "Head" , "Neck" , "Chest" , "Pelvis" } );
				Combo( "Hitbox" , &Aimbot::config.targetHitbox , bones );

				static const auto targetModes = Strings( { "FOV + Distance" , "Crosshair" , "Distance" } );
				Combo( "Target Select" , &Aimbot::config.targetSelection , targetModes );

				Separator();
				Checkbox( "Head##Legit" , &Aimbot::config.hitboxHead );
				Checkbox( "Neck##Legit" , &Aimbot::config.hitboxNeck );
				Checkbox( "Chest##Legit" , &Aimbot::config.hitboxChest );
				Checkbox( "Upper Chest##Legit" , &Aimbot::config.hitboxUpperChest );
				Checkbox( "Stomach##Legit" , &Aimbot::config.hitboxStomach );
				Checkbox( "Pelvis##Legit" , &Aimbot::config.hitboxPelvis );
				Checkbox( "Legs##Legit" , &Aimbot::config.hitboxLegs );
				Checkbox( "Arms##Legit" , &Aimbot::config.hitboxArms );
				if ( Aimbot::config.punchRandomization )
				{
					SliderFloat( "Punch Rand X" , &Aimbot::config.punchRandomX , 0.f , 1.f , 0.02f , "%.2f" );
					SliderFloat( "Punch Rand Y" , &Aimbot::config.punchRandomY , 0.f , 1.f , 0.02f , "%.2f" );
				}
				Checkbox( "Team Check" , &Aimbot::config.teamCheck );
				Checkbox( "Visible Only" , &Aimbot::config.visCheck );
				Checkbox( "Penetration" , &Aimbot::config.penetration );
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Trigger & Weapon" );
			{
				Checkbox( "Enable Triggerbot" , &Triggerbot::config.enabled );
				KeyBind( "Trigger Key" , &Triggerbot::config.key );
				Checkbox( "Trigger Use Key" , &Triggerbot::config.useKey );
				Checkbox( "Team Check (Trigger)" , &Triggerbot::config.teamCheck );
				Checkbox( "Visible Only (Trigger)" , &Triggerbot::config.visCheck );
				SliderInt( "Trigger Hitchance" , &Triggerbot::config.hitchance , 0 , 100 , 1 , "%d%%" );
				SliderInt( "Delay (ms)" , &Triggerbot::config.delayMs , 0 , 200 , 1 , "%d" );

				Separator();
				Checkbox( "No Scope Silent" , &LegitBot::config.noScope );
				Checkbox( "Delay Aim" , &LegitBot::config.delayAim );
				if ( LegitBot::config.delayAim )
					SliderInt( "Delay Aim (ms)" , &LegitBot::config.delayAimMs , 0 , 500 , 1 , "%d" );

				unsigned int conditions = LegitBot::config.conditions;
				SyncLegitConditions( conditions );
				LegitBot::config.conditions = conditions;

				Separator();
				Checkbox( "Use Per-Weapon Settings" , &LegitBot::config.usePerWeapon );
				if ( LegitBot::config.usePerWeapon )
				{
					static std::vector<std::string> classNames;
					if ( classNames.empty() )
					{
						for ( int i = 0; i < WeaponConfig::kWeaponClassCount; ++i )
							classNames.emplace_back( WeaponConfig::kWeaponClassNames[i] );
					}
					Combo( "Weapon Class" , &LegitBot::config.perWeaponClass , classNames );

					WeaponConfig::LegitSettings& ws = WeaponConfig::GetLegitSettings(
						static_cast<WeaponConfig::WeaponClass>( LegitBot::config.perWeaponClass ) );
					SliderInt( "Class FOV" , &ws.fov , 1 , 30 , 1 , "%d" );
					SliderInt( "Class Smooth" , &ws.smooth , 0 , 20 , 1 , "%d" );
					static const auto classTargetModes = Strings( { "FOV + Distance" , "Crosshair" , "Distance" } );
					Combo( "Class Target Select" , &ws.targetSelection , classTargetModes );
					Checkbox( "Class RCS" , &ws.rcs );
					Checkbox( "Class Trigger Override" , &ws.triggerEnabled );
					if ( ws.triggerEnabled )
						SliderInt( "Class Trigger HC" , &ws.triggerHitchance , 0 , 100 , 1 , "%d%%" );
				}
			}
			gui->end_child();
		}
		gui->end_group();

		LegitBot::SyncFromAimbot();
	}
}

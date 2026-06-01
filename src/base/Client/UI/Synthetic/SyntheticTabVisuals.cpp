#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <CS2/Hook/Hook_DrawObject.hpp>

#include <Client/Features/Movement/ThirdPerson.hpp>
#include <Client/Features/Visuals/Chams.hpp>
#include <Client/Features/Visuals/Esp.hpp>
#include <Client/Features/Visuals/EspOverlay.hpp>
#include <Client/Features/Visuals/Tracers.hpp>
#include <Client/Features/Visuals/WorldFov.hpp>
#include <Client/Features/Visuals/WorldVisuals.hpp>
#include <Client/UI/Synthetic/SyntheticEspPreview.hpp>
#include <CS2/Hook/Hook_SetViewModelFov.hpp>

namespace SyntheticTabs
{
	auto RenderVisualsTab() noexcept -> void
	{
		using namespace SyntheticUI;

		gui->begin_group();
		{
			gui->begin_child( "Player ESP" );
			{
				Checkbox( "Enable ESP" , &ESP::config.enabled );
				if ( ESP::config.enabled )
				{
					Checkbox( "Box" , &ESP::config.bBox );
					if ( ESP::config.bBox )
					{
						static const auto boxModes = Strings( { "Normal" , "Corners" } );
						Combo( "Box Style" , &ESP::config.boxMode , boxModes );
					}
					widget->color_edit( "Box Color" , ESP::config.boxColor );
					Checkbox( "Skeleton" , &ESP::config.bSkeleton );
					widget->color_edit( "Skeleton Color" , ESP::config.skeletonColor );
					Checkbox( "Health Bar" , &ESP::config.bHealthBar );
					Checkbox( "Name" , &ESP::config.bName );
					Checkbox( "Distance" , &ESP::config.bDistance );
					ColorCheckbox( "Glow ESP" , &ESP::config.bGlow , ESP::config.glowColor );
					Checkbox( "Team Check" , &ESP::config.teamCheck );
					SliderFloat( "Max Distance" , &ESP::config.maxDistance , 100.f , 10000.f , 50.f , "%.0f" );
					Checkbox( "Bomb Timer" , &ESP::config.bBombTimer );
					Checkbox( "Spectator List" , &ESP::config.bSpectators );
				}
			}
			gui->end_child();

			gui->begin_child( "ESP Overlay" );
			{
				Checkbox( "Enable Overlay" , &EspOverlay::config.enabled );
				if ( EspOverlay::config.enabled )
				{
					Checkbox( "Use Components" , &EspOverlay::config.useComponents );
					Checkbox( "Box" , &EspOverlay::config.showBox );
					Checkbox( "Health Bar" , &EspOverlay::config.showHealthBar );
					Checkbox( "Name" , &EspOverlay::config.showName );
					Checkbox( "Distance" , &EspOverlay::config.showDistance );
					Checkbox( "Weapon Icon" , &EspOverlay::config.showWeaponIcon );
					Checkbox( "Ammo Bar" , &EspOverlay::config.showAmmoBar );
					Checkbox( "Helmet Flag" , &EspOverlay::config.showHelmetFlag );
					Checkbox( "Defuser Flag" , &EspOverlay::config.showKitFlag );
					widget->color_edit( "Box Color" , EspOverlay::config.boxColor );
					widget->color_edit( "Health Bar Color" , EspOverlay::config.healthBarColor );
					widget->color_edit( "Text Color" , EspOverlay::config.textColor );
					SliderFloat( "Box Thickness" , &EspOverlay::config.boxThickness , 1.f , 4.f , 0.1f , "%.1f" );
					SliderFloat( "Bar Thickness" , &EspOverlay::config.barThickness , 1.f , 6.f , 0.1f , "%.1f" );
				}
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Chams" );
			{
				const bool chamsAvailable = DrawObject_o != nullptr && Chams::IsReady();
				if ( !chamsAvailable )
				{
					draw->render_text(
						GetWindowDrawList() ,
						set->c_font.inter_medium[0] ,
						GetWindowPos() ,
						GetWindowPos() + GetWindowSize() ,
						gui->get_clr( clr->c_other_clr.accent_clr ) ,
						"Chams unavailable" ,
						nullptr ,
						nullptr ,
						ImVec2( 0.f , 0.f ) );
				}

				Checkbox( "Enable Chams" , &Chams::config.enabled );
				Checkbox( "Enemy Chams" , &Chams::config.enemy );
				Checkbox( "Local Chams" , &Chams::config.local );
				Checkbox( "Teammate Chams" , &Chams::config.teammate );
				Checkbox( "Hide Teammate Vanilla" , &Chams::config.hideTeammateVanilla );
				Checkbox( "Skip Occlude Pass" , &Chams::config.skipOccludePass );
				Checkbox( "Weapon Chams" , &Chams::config.weapon );
				Checkbox( "Hands / ViewModel" , &Chams::config.hands );
				static const auto chamsMaterials = Strings( { "Flat" , "Glow" , "White" , "Default" , "Illum" } );
				Combo( "Chams Material" , reinterpret_cast<int*>( &Chams::config.materialStyle ) , chamsMaterials );
				Checkbox( "Ignore Z" , &Chams::config.ignoreZ );
				widget->color_edit( "Chams Color" , Chams::config.color );
				if ( Chams::config.ignoreZ )
					widget->color_edit( "Ignore Z Color" , Chams::config.ignoreZColor );
			}
			gui->end_child();

			gui->begin_child( "World & Camera" );
			{
				Checkbox( "Night Mode" , &WorldVisuals::config.nightMode );
				if ( WorldVisuals::config.nightMode )
					SliderFloat( "Ambient Boost" , &WorldVisuals::config.nightAmbient , 0.1f , 1.5f , 0.05f , "%.2f" );
				Checkbox( "World Modulate" , &WorldVisuals::config.worldModulate );
				if ( WorldVisuals::config.worldModulate )
					widget->color_edit( "Modulate Tint" , WorldVisuals::config.modulateColor );
				Checkbox( "No Shadows" , &WorldVisuals::config.noShadow );

				Separator();
				Checkbox( "World FOV Override" , &WorldFov::config.enabled );
				if ( WorldFov::config.enabled )
					SliderFloat( "World FOV" , &WorldFov::config.amount , 60.f , 140.f , 1.f , "%.0f" );
				Checkbox( "ViewModel FOV Override" , &g_viewModelFovConfig.enabled );
				if ( g_viewModelFovConfig.enabled )
					SliderFloat( "ViewModel FOV" , &g_viewModelFovConfig.amount , 54.f , 90.f , 1.f , "%.0f" );

				Checkbox( "Bullet Tracers" , &BulletTracer::config.enabled );
				if ( BulletTracer::config.enabled )
				{
					SliderFloat( "Trail Life" , &BulletTracer::config.trailLife , 0.5f , 5.f , 0.1f , "%.1fs" );
					SliderFloat( "Thickness" , &BulletTracer::config.thickness , 1.f , 6.f , 0.1f , "%.1f" );
				}
				Checkbox( "Third Person" , &ThirdPerson::config.enabled );
				if ( ThirdPerson::config.enabled )
				{
					Checkbox( "Smooth Camera" , &ThirdPerson::config.smoothCamera );
					Checkbox( "No Interp" , &ThirdPerson::config.noInterp );
					SliderFloat( "Distance" , &ThirdPerson::config.distance , 40.f , 400.f , 5.f , "%.0f" );
					SliderInt( "Third Person FOV" , &ThirdPerson::config.fov , 60 , 140 , 1 , "%d" );
				}
			}
			gui->end_child();

			gui->begin_child( "ESP Layout Preview" );
			{
				const float previewH = SCALE( 200.f );
				const ImVec2 previewSize( GetContentRegionAvail().x , previewH );
				const ImVec2 previewOrigin = GetCursorScreenPos();
				ImGui::Dummy( previewSize );
				SyntheticEspPreview::RenderPanel( previewOrigin , previewSize );
			}
			gui->end_child();
		}
		gui->end_group();
	}
}

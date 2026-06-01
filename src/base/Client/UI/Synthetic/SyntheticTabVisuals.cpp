#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <CS2/Hook/Hook_DrawObject.hpp>

#include <Client/Features/Visuals/ThirdPerson.hpp>
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
				UiCheckbox( "Enable ESP" , &ESP::config.enabled );
				if ( ESP::config.enabled )
				{
					UiCheckbox( "Box##ESP" , &ESP::config.bBox );
					if ( ESP::config.bBox )
					{
						static const auto boxModes = ItemStrings( { "Normal" , "Corners" } );
						UiCombo( "Box Style" , &ESP::config.boxMode , boxModes );
					}
					widget->color_edit( "Box Color" , ESP::config.boxColor );
					UiCheckbox( "Skeleton" , &ESP::config.bSkeleton );
					widget->color_edit( "Skeleton Color" , ESP::config.skeletonColor );
					UiCheckbox( "Health Bar##ESP" , &ESP::config.bHealthBar );
					UiCheckbox( "Name##ESP" , &ESP::config.bName );
					UiCheckbox( "Distance##ESP" , &ESP::config.bDistance );
					UiColorCheckbox( "Glow ESP" , &ESP::config.bGlow , ESP::config.glowColor );
					UiCheckbox( "Team Check" , &ESP::config.teamCheck );
					UiSliderFloat( "Max Distance" , &ESP::config.maxDistance , 100.f , 10000.f , 50.f , "%.0f" );
					UiCheckbox( "Bomb Timer" , &ESP::config.bBombTimer );
					UiCheckbox( "Spectator List" , &ESP::config.bSpectators );
				}
			}
			gui->end_child();

			gui->begin_child( "ESP Overlay" );
			{
				UiCheckbox( "Enable Overlay" , &EspOverlay::config.enabled );
				if ( EspOverlay::config.enabled )
				{
					UiCheckbox( "Use Components" , &EspOverlay::config.useComponents );
					UiCheckbox( "Box##Overlay" , &EspOverlay::config.showBox );
					UiCheckbox( "Health Bar##Overlay" , &EspOverlay::config.showHealthBar );
					UiCheckbox( "Name##Overlay" , &EspOverlay::config.showName );
					UiCheckbox( "Distance##Overlay" , &EspOverlay::config.showDistance );
					UiCheckbox( "Weapon Icon" , &EspOverlay::config.showWeaponIcon );
					UiCheckbox( "Ammo Bar" , &EspOverlay::config.showAmmoBar );
					UiCheckbox( "Helmet Flag" , &EspOverlay::config.showHelmetFlag );
					UiCheckbox( "Defuser Flag" , &EspOverlay::config.showKitFlag );
					widget->color_edit( "Box Color##Overlay" , EspOverlay::config.boxColor );
					widget->color_edit( "Health Bar Color##Overlay" , EspOverlay::config.healthBarColor );
					widget->color_edit( "Text Color##Overlay" , EspOverlay::config.textColor );
					UiSliderFloat( "Box Thickness" , &EspOverlay::config.boxThickness , 1.f , 4.f , 0.1f , "%.1f" );
					UiSliderFloat( "Bar Thickness" , &EspOverlay::config.barThickness , 1.f , 6.f , 0.1f , "%.1f" );
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

				UiCheckbox( "Enable Chams" , &Chams::config.enabled );
				UiCheckbox( "Enemy Chams" , &Chams::config.enemy );
				UiCheckbox( "Local Chams" , &Chams::config.local );
				UiCheckbox( "Teammate Chams" , &Chams::config.teammate );
				UiCheckbox( "Hide Teammate Vanilla" , &Chams::config.hideTeammateVanilla );
				UiCheckbox( "Skip Occlude Pass" , &Chams::config.skipOccludePass );
				UiCheckbox( "Weapon Chams" , &Chams::config.weapon );
				UiCheckbox( "Hands / ViewModel" , &Chams::config.hands );
				static const auto chamsMaterials = ItemStrings( { "Flat" , "Glow" , "White" , "Default" , "Illum" } );
				UiCombo( "Chams Material" , reinterpret_cast<int*>( &Chams::config.materialStyle ) , chamsMaterials );
				UiCheckbox( "Ignore Z" , &Chams::config.ignoreZ );
				widget->color_edit( "Chams Color" , Chams::config.color );
				if ( Chams::config.ignoreZ )
					widget->color_edit( "Ignore Z Color" , Chams::config.ignoreZColor );
			}
			gui->end_child();

			gui->begin_child( "World & Camera" );
			{
				UiCheckbox( "Night Mode" , &WorldVisuals::config.nightMode );
				if ( WorldVisuals::config.nightMode )
					UiSliderFloat( "Ambient Boost" , &WorldVisuals::config.nightAmbient , 0.1f , 1.5f , 0.05f , "%.2f" );
				UiCheckbox( "World Modulate" , &WorldVisuals::config.worldModulate );
				if ( WorldVisuals::config.worldModulate )
					widget->color_edit( "Modulate Tint" , WorldVisuals::config.modulateColor );
				UiCheckbox( "No Shadows" , &WorldVisuals::config.noShadow );

				UiSeparator();
				UiCheckbox( "World FOV Override" , &WorldFov::config.enabled );
				if ( WorldFov::config.enabled )
					UiSliderFloat( "World FOV" , &WorldFov::config.amount , 60.f , 140.f , 1.f , "%.0f" );
				UiCheckbox( "ViewModel FOV Override" , &g_viewModelFovConfig.enabled );
				if ( g_viewModelFovConfig.enabled )
					UiSliderFloat( "ViewModel FOV" , &g_viewModelFovConfig.amount , 54.f , 90.f , 1.f , "%.0f" );

				UiCheckbox( "Bullet Tracers" , &BulletTracer::config.enabled );
				if ( BulletTracer::config.enabled )
				{
					UiSliderFloat( "Trail Life" , &BulletTracer::config.trailLife , 0.5f , 5.f , 0.1f , "%.1fs" );
					UiSliderFloat( "Thickness" , &BulletTracer::config.thickness , 1.f , 6.f , 0.1f , "%.1f" );
				}
				UiCheckbox( "Third Person" , &ThirdPerson::config.enabled );
				if ( ThirdPerson::config.enabled )
				{
					UiCheckbox( "Smooth Camera" , &ThirdPerson::config.smoothCamera );
					UiCheckbox( "No Interp" , &ThirdPerson::config.noInterp );
					UiSliderFloat( "Distance" , &ThirdPerson::config.distance , 40.f , 400.f , 5.f , "%.0f" );
					UiSliderInt( "Third Person FOV" , &ThirdPerson::config.fov , 60 , 140 , 1 , "%d" );
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


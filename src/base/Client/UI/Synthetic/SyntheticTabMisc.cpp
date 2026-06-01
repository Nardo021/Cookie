#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/Features/Misc/PlantBomb.hpp>
#include <Client/UI/Menu/MenuEffects.hpp>
#include <Client/UI/Menu/MenuSettings.hpp>

namespace SyntheticTabs
{
	auto RenderMiscTab() noexcept -> void
	{
		using namespace SyntheticUI;

		gui->begin_group();
		{
			gui->begin_child( "Gameplay" );
			{
				Checkbox( "Plant Bomb Anywhere" , &PlantBomb::config.enabled );
			}
			gui->end_child();

			gui->begin_child( "Menu Effects" );
			{
				Checkbox( "Watermark (menu closed)" , &MenuEffects::config.watermark );
				Checkbox( "Synthetic Watermark" , &var->c_watermark.watermark );
				static const auto wmPos = Strings( { "Top Left" , "Top Right" , "Bottom Left" , "Bottom Right" } );
				Combo( "Watermark Position" , &var->c_watermark.watermark_position , wmPos );
				Checkbox( "Menu Particles" , &MenuEffects::config.particles );
				Checkbox( "Menu Background Image" , &MenuEffects::config.menuBackgroundImage );
				if ( MenuEffects::config.menuBackgroundImage )
					SliderFloat( "Background Alpha" , &MenuEffects::config.menuBackgroundAlpha , 0.f , 1.f , 0.05f , "%.2f" );
				Checkbox( "Blur Placeholder" , &MenuEffects::config.blurPlaceholder );
				SliderInt( "Max Particles" , &MenuEffects::config.maxParticles , 20 , 200 , 5 , "%d" );
				SliderFloat( "Particle Link Distance" , &MenuEffects::config.particleLinkDistance , 40.f , 240.f , 5.f , "%.0f" );
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Menu & DPI" );
			{
				SliderInt( "Menu Toggle VK" , &MenuSettings::menuToggleKey , 0 , 255 , 1 , "%d" );

				Separator();

				SliderInt( "DPI" , &var->c_dpi.dpi_saved , 100 , 200 , 1 , "%d%%" );
				if ( var->c_dpi.dpi != var->c_dpi.dpi_saved / 100.f && IsMouseReleased( ImGuiMouseButton_Left ) )
				{
					var->c_dpi.dpi = var->c_dpi.dpi_saved / 100.f;
					var->c_dpi.dpi_changed = true;
				}
			}
			gui->end_child();
		}
		gui->end_group();
	}
}

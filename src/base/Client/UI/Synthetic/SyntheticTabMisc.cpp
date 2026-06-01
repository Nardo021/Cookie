#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/Features/Misc/PlantBomb.hpp>
#include <Client/UI/Menu/MenuSettings.hpp>
#include <Client/UI/Synthetic/SyntheticMenu.hpp>

namespace SyntheticTabs
{
	auto RenderMiscTab() noexcept -> void
	{
		using namespace SyntheticUI;

		gui->begin_group();
		{
			gui->begin_child( "Gameplay" );
			{
				UiCheckbox( "Plant Bomb Anywhere" , &PlantBomb::config.enabled );
			}
			gui->end_child();

			gui->begin_child( "HUD & Notify" );
			{
				UiCheckbox( "HUD (Server / FPS / Ping / Time)" , &var->c_watermark.watermark );
				static const auto wmPos = ItemStrings( { "Top Left" , "Top Right" , "Bottom Left" , "Bottom Right" } );
				if ( UiCombo( "HUD Default Corner" , &var->c_watermark.watermark_position , wmPos ) )
					var->c_watermark.use_custom_position = false;
				static const auto notifyPos = ItemStrings( { "Top Left" , "Top Right" , "Bottom Left" , "Bottom Right" } );
				UiCombo( "Notify Position" , &var->c_notify.notify_position , notifyPos );
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Menu & DPI" );
			{
				UiSliderInt( "Menu Toggle VK" , &MenuSettings::menuToggleKey , 0 , 255 , 1 , "%d" );

				UiSeparator();

				UiSliderInt( "DPI" , &var->c_dpi.dpi_saved , 100 , 200 , 1 , "%d%%" );
				if ( var->c_dpi.dpi != var->c_dpi.dpi_saved / 100.f && IsMouseReleased( ImGuiMouseButton_Left ) )
				{
					var->c_dpi.dpi = var->c_dpi.dpi_saved / 100.f;
					var->c_dpi.dpi_changed = true;
					MenuSettings::menuDpiPercent = var->c_dpi.dpi_saved;
				}

				if ( IsMouseReleased( ImGuiMouseButton_Left ) )
					SyntheticMenu::SyncUiSettingsToMenu();
			}
			gui->end_child();
		}
		gui->end_group();
	}
}

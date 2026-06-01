#include <Client/UI/Synthetic/SyntheticConfig.hpp>
#include <Client/UI/Synthetic/SyntheticTabs.hpp>
#include <Client/UI/Synthetic/SyntheticWatermark.hpp>
#include <Client/UI/Menu/MenuEffects.hpp>
#include <Version.hpp>

#include "framework/settings/functions.h"
#include "framework/shader/blur.hpp"
#include "framework/data/font.h"
#include "framework/data/texture.h"

void c_gui::render()
{
	{
		notify->setup_notify();

		draw->add_image(GetBackgroundDrawList(), set->c_texture.bg, { 0, 0 }, { 1920, 1080 }, { 0, 0 }, { 1, 1 }, gui->get_clr(clr->c_other_clr.white_clr));

		gui->set_next_window_size(SCALE(set->c_window.window_size));

		gui->begin({ "Cookie" }, { 0 }, set->c_window.window_flags);
		{
			const ImVec2 pos = GetWindowPos();
			const ImVec2 size = GetWindowSize();

			ImDrawList* draw_list = GetWindowDrawList();
			ImGuiStyle* style = &GetStyle();

			{
				style->WindowBorderSize = SCALE(set->c_window.border_size);
				style->WindowRounding = SCALE(set->c_window.rounding);
				style->WindowPadding = SCALE(set->c_window.padding);

				style->ScrollbarSize = SCALE(set->c_window.scrollbar_size);
				style->ItemSpacing = SCALE(set->c_window.item_spacing);
			}

			draw_background_blur(draw_list, g_pSwapChain, g_pd3dDevice, g_pd3dDeviceContext, GetWindowPos(), GetWindowPos() + GetWindowSize(), style->WindowRounding);
			
			draw->add_rect_filled(draw_list, { pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, gui->get_clr(clr->c_window.general_layout), SCALE(set->c_window.general_rounding));
			draw->add_rect(draw_list, { pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, gui->get_clr(clr->c_window.general_stroke), SCALE(set->c_window.general_rounding));

			draw->add_rect_filled(draw_list, { pos.x + SCALE(110), pos.y + SCALE(15) }, { pos.x + (size.x - SCALE(15)), pos.y + (size.y - SCALE(15)) }, gui->get_clr(clr->c_window.layout), SCALE(set->c_window.rounding));
			draw->add_rect(draw_list, { pos.x + SCALE(110), pos.y + SCALE(15) }, { pos.x + (size.x - SCALE(15)), pos.y + (size.y - SCALE(15)) }, gui->get_clr(clr->c_window.stroke), SCALE(set->c_window.rounding));

			if ( MenuEffects::config.particles || MenuEffects::config.menuBackgroundImage || MenuEffects::config.blurPlaceholder )
			{
				const ImVec2 panelOrigin( pos.x + SCALE( 110 ) , pos.y + SCALE( 15 ) );
				const ImVec2 panelSize( size.x - SCALE( 125 ) , size.y - SCALE( 30 ) );
				MenuEffects::RenderMenuBackground( panelSize , &panelOrigin );
			}

			draw->rect_filled_multi_color(draw_list, { pos.x + size.x / 2, pos.y }, { pos.x + size.x, pos.y + 1 }, gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f));
			draw->rect_filled_multi_color(draw_list, { pos.x, pos.y }, { pos.x + size.x / 2, pos.y + 1 }, gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f));

			draw->rect_filled_multi_color(draw_list, { pos.x + size.x / 2, pos.y + size.y - 1 }, { pos.x + size.x, pos.y + size.y }, gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f));
			draw->rect_filled_multi_color(draw_list, { pos.x, pos.y + size.y - 1 }, { pos.x + size.x / 2, pos.y + size.y }, gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.2f), gui->get_clr(clr->c_other_clr.accent_clr, 0.f));

			draw->render_text(draw_list, set->c_font.icon[2], { pos.x, pos.y }, { pos.x + SCALE(110), pos.y + size.y }, gui->get_clr(clr->c_other_clr.accent_clr), "B", 0, 0, { 0.5, 0.5 });

			gui->push_font(set->c_font.name);

			const int vtx_start_one = draw_list->VtxBuffer.Size;
			gui->rotate_start();
			draw->render_text_with_spacing("Cookie", SCALE(3.f), pos, pos + ImVec2(SCALE(110), size.y / 2), ImColor(255, 255, 255), true);
			gui->rotate_end(gui->deg_to_rad(-90));
			const int vtx_end_one = draw_list->VtxBuffer.Size;
			draw->set_linear_color_alpha(draw_list, vtx_start_one, vtx_end_one, pos + ImVec2(0, size.y / 6), pos + ImVec2(0, size.y / 2), gui->get_clr(clr->c_other_clr.accent_clr, 0.f), gui->get_clr(clr->c_other_clr.accent_clr));

			const int vtx_start_two = draw_list->VtxBuffer.Size;
			draw->render_text_with_spacing(COOKIE_VER_LABEL, SCALE(3.f), pos + ImVec2(0, size.y / 2), pos + ImVec2(SCALE(110), size.y), ImColor(255, 255, 255), true);
			const int vtx_end_two = draw_list->VtxBuffer.Size;
			draw->set_linear_color_alpha(draw_list, vtx_start_two, vtx_end_two, pos + ImVec2(0, size.y / 2), pos + ImVec2(0, size.y / 2 + size.y / 3), gui->get_clr(clr->c_other_clr.accent_clr), gui->get_clr(clr->c_other_clr.accent_clr, 0.f));

			gui->pop_font();

			gui->set_cursor_pos(SCALE(110, 15));

			var->c_selection.selection_alpha = ImClamp(var->c_selection.selection_alpha + (4.f * ImGui::GetIO().DeltaTime * (var->c_selection.selection == var->c_selection.selection_active ? 1.f : -1.f)), 0.f, 1.f);
			if (var->c_selection.selection_alpha == 0.f && var->c_selection.selection_add == 0.f) var->c_selection.selection_active = var->c_selection.selection;

			gui->push_style_var(ImGuiStyleVar_Alpha, var->c_selection.selection_alpha * style->Alpha);

			gui->begin_content("content", GetContentRegionAvail() - SCALE(15, 15), { 15, 15 }, { 15, 15 });
			{
				switch ( var->c_selection.selection_active )
				{
				case 0: SyntheticTabs::RenderRageTab(); break;
				case 1: SyntheticTabs::RenderLegitTab(); break;
				case 2: SyntheticTabs::RenderMovementTab(); break;
				case 3: SyntheticTabs::RenderVisualsTab(); break;
				case 4: SyntheticTabs::RenderSkinsTab(); break;
				case 5: SyntheticConfig::RenderConfigTab(); break;
				case 6: SyntheticTabs::RenderMiscTab(); break;
				default: break;
				}
			}
			gui->end_content();

			gui->pop_style_var(2);

		}
		gui->end();

		gui->push_style_var(ImGuiStyleVar_WindowPadding, SCALE(15, 15));
		gui->push_style_var(ImGuiStyleVar_ItemSpacing, SCALE(4, 0));
		{
			gui->set_next_window_pos(SCALE(20, 20));
			gui->begin({ "SELECTION" }, { 0 }, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
			{
				const ImVec2 pos = GetWindowPos();
				const ImVec2 size = GetWindowSize();

				ImDrawList* draw_list = GetBackgroundDrawList();

				gui->set_cursor_pos_x(SCALE(54));
				gui->begin_group();
				{
					widget->selection(var->c_selection.selection_icon[0].data(), ImVec2(74, 76), 0, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[1].data(), ImVec2(74, 76), 1, var->c_selection.selection);
				}
				gui->end_group();

				gui->set_cursor_pos_y(get_cursor_pos_y() - SCALE(5));
				gui->begin_group();
				{
					widget->selection(var->c_selection.selection_icon[2].data(), ImVec2(74, 76), 2, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[3].data(), ImVec2(74, 76), 3, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[4].data(), ImVec2(74, 76), 4, var->c_selection.selection);
				}
				gui->end_group();

				gui->set_cursor_pos(get_cursor_pos() + SCALE(39, -5));
				gui->begin_group();
				{
					widget->selection(var->c_selection.selection_icon[5].data(), ImVec2(74, 76), 5, var->c_selection.selection);

					gui->sameline();

					widget->selection(var->c_selection.selection_icon[6].data(), ImVec2(74, 76), 6, var->c_selection.selection);
				}
				gui->end_group();

			}
			gui->end();
		}
		gui->pop_style_var(2);

		SyntheticWatermark::Render();

	}
}
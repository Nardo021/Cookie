#include "SyntheticBinds.hpp"

#include <vector>

#include <framework/settings/functions.h>

namespace SyntheticBinds
{
	namespace
	{
		struct Entry
		{
			const char*    label = nullptr;
			bool*          enabled = nullptr;
			int*           key = nullptr;
			KeyBindUiState ui{};
		};

		std::vector<Entry> s_entries;

	}

	auto BeginFrame() noexcept -> void
	{
		s_entries.clear();
	}

	auto Register(
		const char* label ,
		bool* enabled ,
		int* key ,
		KeyBindUiState ui ) noexcept -> void
	{
		s_entries.push_back( { label , enabled , key , ui } );
	}

	auto RenderOverlay() noexcept -> void
	{
		if ( s_entries.empty() )
			return;

		int visible = 0;
		for ( const Entry& e : s_entries )
		{
			if ( *e.ui.showInBinds && *e.enabled && *e.ui.useKey && *e.key != 0 )
				++visible;
		}

		if ( visible == 0 )
			return;

		const ImVec2 display = ImGui::GetIO().DisplaySize;
		const float panelW = SCALE( 220.f );
		const float rowH = SCALE( 22.f );
		const float headerH = SCALE( 28.f );
		const float panelH = headerH + rowH * static_cast<float>( visible ) + SCALE( 12.f );

		gui->set_next_window_pos( ImVec2( display.x - panelW - SCALE( 24.f ) , SCALE( 24.f ) ) , ImGuiCond_Always );
		gui->set_next_window_size( ImVec2( panelW , panelH ) , ImGuiCond_Always );
		gui->push_style_var( ImGuiStyleVar_WindowPadding , SCALE( 10 , 8 ) );
		gui->begin( "Active Binds" , nullptr , ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );
		{
			ImDrawList* dl = GetWindowDrawList();
			const ImVec2 pos = GetWindowPos();
			const ImVec2 size = GetWindowSize();

			draw->add_rect_filled( dl , pos , pos + size , gui->get_clr( clr->c_child.layout ) , SCALE( set->c_child.rounding ) );
			draw->add_rect( dl , pos , pos + size , gui->get_clr( clr->c_child.stroke ) , SCALE( set->c_child.rounding ) , 0 , SCALE( 1.f ) );

			draw->render_text(
				dl ,
				set->c_font.inter_medium[0] ,
				pos + ImVec2( SCALE( 8 ) , SCALE( 4 ) ) ,
				pos + ImVec2( size.x , headerH ) ,
				gui->get_clr( clr->c_text.text_active ) ,
				"Active Binds" ,
				nullptr ,
				nullptr ,
				ImVec2( 0.f , 0.5f ) );

			float y = headerH;
			for ( const Entry& e : s_entries )
			{
				if ( !*e.ui.showInBinds || !*e.enabled || !*e.ui.useKey || *e.key == 0 )
					continue;

				const char* modeText = *e.ui.holdMode ? "Hold" : "Toggle";
				char line[128];
				snprintf( line , sizeof( line ) , "%s  [%s] %s" , e.label , widget->key_name_from_vk( *e.key ) , modeText );

				draw->render_text(
					dl ,
					set->c_font.inter_medium[0] ,
					pos + ImVec2( SCALE( 8 ) , y ) ,
					pos + ImVec2( size.x - SCALE( 8 ) , y + rowH ) ,
					gui->get_clr( clr->c_text.text ) ,
					line ,
					nullptr ,
					nullptr ,
					ImVec2( 0.f , 0.5f ) );

				y += rowH;
			}
		}
		gui->end();
		gui->pop_style_var();
	}
}

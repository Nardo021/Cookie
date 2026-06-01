#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/UI/Synthetic/SyntheticLuaRuntime.hpp>
#include <Client/UI/Synthetic/SyntheticNotifyBridge.hpp>

namespace SyntheticTabs
{
	auto RenderLuaTab() noexcept -> void
	{
		using namespace SyntheticUI;

		SyntheticLua::SyncScriptList();

		const ImVec2 pos = GetWindowPos();
		const ImVec2 size = GetWindowSize();

		draw->add_line(
			GetWindowDrawList() ,
			GetWindowPos() + ImVec2( GetStyle().WindowPadding.x , SCALE( 64 ) ) ,
			GetWindowPos() + ImVec2( GetWindowWidth() - GetStyle().WindowPadding.x , SCALE( 64 ) ) ,
			gui->get_clr( clr->c_child.stroke ) ,
			SCALE( 1.f ) );

		widget->tool_dropdown( "Sort" , &var->c_lua.sort_selection , var->c_lua.sort_list , var->c_lua.sort_list.size() );
		gui->sameline();
		if ( widget->tool_button( "Create" , "A" , SCALE( 90 , 36 ) ) )
			var->c_lua.create = true;

		if ( var->c_lua.create )
		{
			gui->set_next_window_size( SCALE( 310 , 80 ) );
			gui->set_next_window_pos( GetWindowPos() + ( GetWindowSize() / 2.f - SCALE( 310 , 80 ) / 2.f ) );
			gui->push_style_var( ImGuiStyleVar_WindowPadding , ImVec2( 0 , 0 ) );
			gui->begin( "Create Lua" , nullptr , ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground );
			{
				draw->add_rect_filled( GetWindowDrawList() , GetWindowPos() , GetWindowPos() + GetWindowSize() , gui->get_clr( clr->c_child.layout ) , SCALE( set->c_child.rounding ) );
				draw->add_rect( GetWindowDrawList() , GetWindowPos() , GetWindowPos() + GetWindowSize() , gui->get_clr( clr->c_child.stroke ) , SCALE( set->c_child.rounding ) , 0 , SCALE( 1.f ) );

				gui->set_cursor_pos( SCALE( 20 , 20 ) );
				gui->begin_group();
				{
					widget->text_field( "Lua Name" , "M" , var->c_lua.name , sizeof( var->c_lua.name ) , SCALE( 180 , 40 ) );
					gui->sameline();
					ImGui::PushID( "CreateLuaConfirm" );
					if ( widget->button( "Create" , SCALE( 75 , 40 ) ) )
					{
						if ( var->c_lua.name[0] != '\0' )
						{
							if ( SyntheticLua::CreateScript( var->c_lua.name ) )
							{
								var->c_lua.opened = true;
								SyntheticNotifyBridge::Push( N_TYPE_SUCCESS , "Lua script created" );
							}
							else
								SyntheticNotifyBridge::Push( N_TYPE_WARNING , SyntheticLua::GetLastError().c_str() );
							var->c_lua.create = false;
							var->c_lua.name[0] = '\0';
						}
					}
					ImGui::PopID();
				}
				gui->end_group();

				if ( !IsMouseHoveringRect( GetWindowPos() , GetWindowPos() + GetWindowSize() )
					&& ( IsMouseClicked( 0 ) || IsMouseClicked( 1 ) ) )
					var->c_lua.create = false;
			}
			gui->end();
			gui->pop_style_var();
		}

		gui->set_cursor_pos_y( SCALE( 80 ) );
		gui->begin_group();
		{
			const int count = static_cast<int>( var->c_lua.data.size() );
			for ( int i = 0; i < count; ++i )
			{
				const int index = var->c_lua.sort_selection == 0 ? i : ( count - 1 - i );
				if ( widget->lua_selectable( &var->c_lua.data.at( index ) , index ) )
				{
					var->c_lua.editable = var->c_lua.data[index].name;
					var->c_lua.opened = true;
					SyntheticLua::LoadScriptIntoEditor( var->c_lua.editable );
				}
			}
		}
		gui->end_group();

		if ( var->c_lua.opened )
		{
			gui->set_next_window_size( SCALE( 500 , 520 ) );
			gui->set_next_window_pos( pos + ImVec2( size.x + SCALE( 20 ) , 0 ) );
			gui->push_style_var( ImGuiStyleVar_WindowPadding , ImVec2( 0 , 0 ) );
			gui->begin( "Lua Editor" , nullptr , ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground );
			{
				draw->add_rect_filled( GetWindowDrawList() , GetWindowPos() , GetWindowPos() + GetWindowSize() , gui->get_clr( clr->c_window.general_layout ) , SCALE( set->c_window.general_rounding ) );
				draw->add_rect( GetWindowDrawList() , GetWindowPos() , GetWindowPos() + GetWindowSize() , gui->get_clr( clr->c_window.general_stroke ) , SCALE( set->c_window.general_rounding ) , 0 , SCALE( 1.f ) );
				draw->add_line( GetWindowDrawList() , GetWindowPos() + SCALE( 0 , 40 ) , GetWindowPos() + ImVec2( GetWindowSize().x , SCALE( 40 ) ) , gui->get_clr( clr->c_window.general_stroke ) , SCALE( 1.f ) );
				draw->add_line( GetWindowDrawList() , GetWindowPos() + ImVec2( 0 , GetWindowSize().y - SCALE( 40 ) ) , GetWindowPos() + ImVec2( GetWindowSize().x , GetWindowSize().y - SCALE( 40 ) ) , gui->get_clr( clr->c_window.general_stroke ) , SCALE( 1.f ) );

				const std::string title = std::string( "Lua Editor - " ) + var->c_lua.editable;
				draw->render_text( GetWindowDrawList() , set->c_font.inter_medium[0] , GetWindowPos() + SCALE( 40 , 0 ) , GetWindowPos() + ImVec2( GetWindowWidth() , SCALE( 40 ) ) , gui->get_clr( clr->c_text.text_active ) , title.c_str() , nullptr , nullptr , ImVec2( 0.f , 0.5f ) );
				draw->render_text( GetWindowDrawList() , set->c_font.icon[4] , GetWindowPos() + SCALE( 15 , 1 ) , GetWindowPos() + ImVec2( GetWindowWidth() , SCALE( 40 ) ) , gui->get_clr( clr->c_other_clr.accent_clr ) , "B" , nullptr , nullptr , ImVec2( 0.f , 0.5f ) );

				gui->set_cursor_pos( ImVec2( GetWindowWidth() - SCALE( 30 ) , 13 ) );
				if ( widget->lua_tool_button( "E" , "Close Button" ) )
					var->c_lua.opened = false;

				gui->set_cursor_pos( SCALE( 10 , 50 ) );
				text_editor->render( "LuaTextEditor" , SCALE( 480 , 420 ) );

				gui->set_cursor_pos( GetWindowSize() - SCALE( 122 , 34 ) );
				if ( widget->tool_button( "Save" , "" , SCALE( 54 , 28 ) ) )
				{
					const std::string source = text_editor->get_text();
					if ( SyntheticLua::SaveActiveScript( var->c_lua.editable , source ) )
						SyntheticNotifyBridge::Push( N_TYPE_SUCCESS , "Lua script saved" );
					else
						SyntheticNotifyBridge::Push( N_TYPE_WARNING , SyntheticLua::GetLastError().c_str() );
				}

				gui->set_cursor_pos( GetWindowSize() - SCALE( 61 , 34 ) );
				if ( widget->tool_button( "Run" , "" , SCALE( 54 , 28 ) ) )
				{
					const std::string source = text_editor->get_text();
					if ( SyntheticLua::RunSource( source ) )
						SyntheticNotifyBridge::Push( N_TYPE_SUCCESS , "Lua executed" );
					else
						SyntheticNotifyBridge::Push( N_TYPE_WARNING , SyntheticLua::GetLastError().c_str() );
				}
			}
			gui->end();
			gui->pop_style_var();
		}
	}
}

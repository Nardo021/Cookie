#include "SyntheticConfig.hpp"

#include <cstdio>
#include <cstring>

#include <Client/Settings/CSettingsJson.hpp>
#include <Client/UI/Menu/MenuConfig.hpp>
#include <Client/Utils/CNotify.hpp>
#include <Common/Include/Config.hpp>
#include <framework/settings/functions.h>

namespace SyntheticConfig
{
	namespace
	{
		char s_configNameBuf[128] = "default.json";
		int s_selectedConfigIdx = -1;
		bool s_listDirty = true;

		auto EnsureExtension( std::string fileName ) -> std::string
		{
			if ( fileName.find( ".json" ) == std::string::npos )
				fileName += ".json";
			return fileName;
		}
	}

	auto InvalidateConfigList() noexcept -> void
	{
		s_listDirty = true;
	}

	auto SyncConfigListFromDisk() noexcept -> void
	{
		if ( !s_listDirty )
			return;

		GetSettingsJson()->UpdateConfigList();
		var->c_config.data.clear();

		for ( const auto& name : GetSettingsJson()->GetConfigList() )
			var->c_config.data.push_back( { name , "on disk" } );

		if ( var->c_config.active >= static_cast<int>( var->c_config.data.size() ) )
			var->c_config.active = -1;

		s_listDirty = false;
	}

	auto RenderConfigTab() noexcept -> void
	{
		SyncConfigListFromDisk();

		draw->add_line(
			GetWindowDrawList() ,
			GetWindowPos() + ImVec2( GetStyle().WindowPadding.x , SCALE( 64 ) ) ,
			GetWindowPos() + ImVec2( GetWindowWidth() - GetStyle().WindowPadding.x , SCALE( 64 ) ) ,
			gui->get_clr( clr->c_child.stroke ) ,
			SCALE( 1.f ) );

		widget->tool_dropdown( "Sort" , &var->c_config.sort_selection , var->c_config.sort_list , var->c_config.sort_list.size() );
		gui->sameline();
		if ( widget->tool_button( "Refresh" , "R" , SCALE( 90 , 36 ) ) )
			s_listDirty = true;

		gui->set_cursor_pos_y( SCALE( 80 ) );
		widget->text_field( "Config Name" , "M" , s_configNameBuf , sizeof( s_configNameBuf ) , SCALE( GetContentRegionAvail().x , 40 ) );

		const float buttonWidth = ( GetContentRegionAvail().x - GetStyle().ItemSpacing.x ) / 3.f;
		if ( widget->button( "Save" , ImVec2( buttonWidth , SCALE( 35 ) ) ) )
		{
			const std::string fileName = EnsureExtension( s_configNameBuf );
			GetSettingsJson()->SaveConfig( fileName );
			InvalidateConfigList();
		}
		gui->sameline();
		if ( widget->button( "Load" , ImVec2( buttonWidth , SCALE( 35 ) ) ) )
		{
			const std::string fileName = EnsureExtension( s_configNameBuf );
			GetSettingsJson()->LoadConfig( fileName );
			InvalidateConfigList();
		}
		gui->sameline();
		if ( widget->button( "Delete" , ImVec2( buttonWidth , SCALE( 35 ) ) ) )
		{
			const std::string fileName = EnsureExtension( s_configNameBuf );
			GetSettingsJson()->DeleteConfig( fileName );
			InvalidateConfigList();
			SyncConfigListFromDisk();
			GetNotify()->Push( N_TYPE_WARNING , "Config deleted" );
		}

		widget->separator();

		char schemaText[64];
		snprintf( schemaText , sizeof( schemaText ) , "Schema version: %d" , MenuConfig::kSchemaVersion );
		draw->render_text(
			GetWindowDrawList() ,
			set->c_font.inter_medium[0] ,
			GetWindowPos() + SCALE( 0 , GetCursorPosY() ) ,
			GetWindowPos() + GetWindowSize() ,
			gui->get_clr( clr->c_text.text ) ,
			schemaText ,
			nullptr ,
			nullptr ,
			ImVec2( 0.f , 0.f ) );

		gui->set_cursor_pos_y( GetCursorPosY() + SCALE( 24 ) );

		static int s_prevActive = -1;
		gui->begin_group();
		for ( int i = 0; i < static_cast<int>( var->c_config.data.size() ); ++i )
		{
			if ( widget->config_selectable( &var->c_config.data.at( i ) , i , var->c_config.active ) )
			{
				s_selectedConfigIdx = i;
				snprintf( s_configNameBuf , sizeof( s_configNameBuf ) , "%s" , var->c_config.data[i].name.c_str() );
			}
		}
		gui->end_group();

		if ( var->c_config.active != s_prevActive && var->c_config.active >= 0
			&& var->c_config.active < static_cast<int>( var->c_config.data.size() ) )
		{
			s_prevActive = var->c_config.active;
			const std::string fileName = EnsureExtension( var->c_config.data[var->c_config.active].name );
			GetSettingsJson()->LoadConfig( fileName );
			snprintf( s_configNameBuf , sizeof( s_configNameBuf ) , "%s" , fileName.c_str() );
		}

	}
}

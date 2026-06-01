#include "SyntheticWatermark.hpp"

#include <cstdio>
#include <ctime>

#include <Common/Include/Config.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <GameClient/CL_Players.hpp>
#include <Client/UI/Menu/MenuEffects.hpp>
#include <Client/UI/Menu/MenuSettings.hpp>
#include <Client/UI/Synthetic/SyntheticMenu.hpp>
#include <framework/settings/functions.h>

namespace SyntheticWatermark
{
	namespace
	{
		auto GetServerLabel() noexcept -> const char*
		{
			auto* engine = SDK::Interfaces::EngineToClient();
			if ( !engine )
				return "Server";

			const char* level = engine->GetLevelNameShort();
			if ( !level || level[0] == '\0' )
				return "Server";

			return level;
		}

		auto GetPingMs() noexcept -> int
		{
			auto* controller = GetCL_Players()->GetLocalPlayerController();
			if ( !controller )
				return 0;
			return static_cast<int>( controller->m_iPing() );
		}

		auto GetTimeLabel( char* buf , size_t bufSize ) noexcept -> void
		{
			const std::time_t now = std::time( nullptr );
			std::tm localTime{};
#if defined( _WIN32 )
			localtime_s( &localTime , &now );
#else
			localtime_r( &now , &localTime );
#endif
			const int hour12 = ( localTime.tm_hour % 12 == 0 ) ? 12 : ( localTime.tm_hour % 12 );
			const char* ampm = ( localTime.tm_hour < 12 ) ? "AM" : "PM";
			snprintf( buf , bufSize , "%d:%02d%s" , hour12 , localTime.tm_min , ampm );
		}

		auto SyncHudLayoutToMenuSettings() noexcept -> void
		{
			MenuSettings::useCustomHudPosition = var->c_watermark.use_custom_position;
			MenuSettings::hudPositionX = var->c_watermark.custom_position.x;
			MenuSettings::hudPositionY = var->c_watermark.custom_position.y;
		}
	}

	auto Update() noexcept -> void
	{
		const ImGuiIO& io = ImGui::GetIO();

		char fpsBuf[32];
		char pingBuf[32];
		char timeBuf[32];
		snprintf( fpsBuf , sizeof( fpsBuf ) , "%.0f FPS" , io.Framerate );
		snprintf( pingBuf , sizeof( pingBuf ) , "%dms" , GetPingMs() );
		GetTimeLabel( timeBuf , sizeof( timeBuf ) );

		var->c_watermark.watermark_content = {
			GetServerLabel() ,
			fpsBuf ,
			pingBuf ,
			timeBuf ,
		};
	}

	auto Render() noexcept -> void
	{
		if ( !var->c_watermark.watermark || !set->c_font.inter_medium[0] )
			return;

		Update();

		watermark_layout layout{};
		layout.use_custom_pos = var->c_watermark.use_custom_position;
		layout.custom_pos = var->c_watermark.custom_position;
		layout.draggable = true;

		gui->water_mark(
			"##CookieHud" ,
			var->c_watermark.watermark_content ,
			static_cast<watermark_position>( var->c_watermark.watermark_position ) ,
			&var->c_watermark.watermark ,
			&layout );

		var->c_watermark.use_custom_position = layout.use_custom_pos;
		var->c_watermark.custom_position = layout.custom_pos;
		SyncHudLayoutToMenuSettings();

		if ( layout.request_settings_sync )
			SyntheticMenu::SyncUiSettingsToMenu();
	}

	auto RenderOverlay() noexcept -> void
	{
		if ( SyntheticMenu::IsInitialized() && var->c_watermark.watermark )
			Render();
		else if ( MenuEffects::config.watermark )
			MenuEffects::RenderWatermark();
	}
}

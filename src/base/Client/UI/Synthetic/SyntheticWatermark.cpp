#include "SyntheticWatermark.hpp"



#include <cstdio>

#include <ctime>

#include <string>

#include <vector>



#include <Common/Include/Config.hpp>

#include <CS2/SDK/Interface/IEngineToClient.hpp>

#include <CS2/SDK/SDK.hpp>

#include <CS2/SDK/Types/CEntityData.hpp>

#include <GameClient/CL_Players.hpp>

#include <Client/UI/Menu/MenuEffects.hpp>

#include <Client/UI/Menu/MenuSettings.hpp>

#include <Client/UI/Synthetic/SyntheticMenu.hpp>

#include <framework/settings/functions.h>



#include <Client/UI/Synthetic/SyntheticCompat/SyntheticUiGuard.hpp>



namespace SyntheticWatermark

{

	namespace

	{

		struct HudState

		{

			ImVec2 pos{};

			ImVec2 size{};

			bool pos_initialized = false;

		};



		HudState s_hud{};



		auto GetServerLabelOwned() noexcept -> std::string

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



		auto BuildHudLines() noexcept -> std::vector<std::string>

		{

			const ImGuiIO& io = ImGui::GetIO();



			char fpsBuf[32];

			char pingBuf[32];

			char timeBuf[32];

			snprintf( fpsBuf , sizeof( fpsBuf ) , "%.0f FPS" , io.Framerate );

			snprintf( pingBuf , sizeof( pingBuf ) , "%dms" , GetPingMs() );

			GetTimeLabel( timeBuf , sizeof( timeBuf ) );



			return {

				GetServerLabelOwned() ,

				fpsBuf ,

				pingBuf ,

				timeBuf ,

			};

		}



		auto MeasureHudSize( ImFont* font , const std::vector<std::string>& lines ) noexcept -> ImVec2

		{

			if ( !SyntheticUi::FontUsable( font ) )

				return ImVec2{};

			const float padX = SCALE( 10.f );

			const float padY = SCALE( 10.f );

			const float sep = SCALE( 20.f );

			const float dot = SCALE( 3.f );



			float width = 0.f;

			const float lineH = font->FontSize;

			for ( size_t i = 0; i < lines.size(); ++i )

			{

				if ( lines[i].empty() )

					continue;



				const ImVec2 ts = font->CalcTextSizeA( font->FontSize , FLT_MAX , 0.f , lines[i].c_str() );

				width += ts.x;

				if ( i + 1 < lines.size() )

					width += sep + dot;

			}



			return ImVec2( width + padX * 2.f , lineH + padY * 2.f );

		}



		auto ResolveHudPosition(

			const ImVec2& contentSize ,

			watermark_position corner ,

			bool useCustom ,

			ImVec2 customPos ) noexcept -> ImVec2

		{

			if ( useCustom && customPos.x >= 0.f && customPos.y >= 0.f )

				return customPos;



			const ImVec2 display = ImGui::GetIO().DisplaySize;

			const ImVec2 margin = SCALE( ImVec2( 10.f , 10.f ) );



			switch ( corner )

			{

			case mark_top_left:

				return margin;

			case mark_top_right:

				return ImVec2( display.x - contentSize.x - margin.x , margin.y );

			case mark_bottom_left:

				return ImVec2( margin.x , display.y - contentSize.y - margin.y );

			case mark_bottom_right:

			default:

				return ImVec2( display.x - contentSize.x - margin.x , display.y - contentSize.y - margin.y );

			}

		}



		auto DrawHudBackground( ImDrawList* drawList , const ImVec2& pos , const ImVec2& size ) noexcept -> void

		{

			if ( !drawList )

				return;



			drawList->AddRectFilled(

				pos ,

				ImVec2( pos.x + size.x , pos.y + size.y ) ,

				gui->get_clr( clr->c_window.layout ) ,

				SCALE( 4.f ) );

		}



		auto DrawHudLine(

			ImDrawList* drawList ,

			ImFont* font ,

			const ImVec2& origin ,

			const std::vector<std::string>& lines ) noexcept -> void

		{

			if ( !drawList || !font || !SyntheticUi::FontUsable( font ) )

				return;



			const ImTextureID texId = SyntheticUi::FontAtlasTexture( font );

			if ( texId == ImTextureID{} )

				return;



			const float padX = SCALE( 10.f );

			const float padY = SCALE( 10.f );

			const float sep = SCALE( 20.f );

			const float dotW = SCALE( 3.f );

			const float dotH = SCALE( 14.f );

			const float fontSize = font->FontSize;

			const float baselineY = origin.y + padY;



			float x = origin.x + padX;

			drawList->PushTextureID( texId );



			for ( size_t i = 0; i < lines.size(); ++i )

			{

				if ( lines[i].empty() )

					continue;



				const ImU32 col = gui->get_clr( i == 0 ? clr->c_text.text_active : clr->c_text.text );

				drawList->AddText( font , fontSize , ImVec2( x , baselineY ) , col , lines[i].c_str() );



				const ImVec2 ts = font->CalcTextSizeA( fontSize , FLT_MAX , 0.f , lines[i].c_str() );

				x += ts.x;



				if ( i + 1 < lines.size() )

				{

					const float dotX = x + ( sep - dotW ) * 0.5f;

					drawList->AddRectFilled(

						ImVec2( dotX , baselineY ) ,

						ImVec2( dotX + dotW , baselineY + dotH ) ,

						gui->get_clr( clr->c_child.stroke ) ,

						SCALE( 10.f ) );

					x += sep;

				}

			}



			drawList->PopTextureID();

		}



		auto HandleHudDrag( const ImVec2& pos , const ImVec2& size , bool draggable ) noexcept -> bool

		{

			if ( !draggable )

				return false;



			const ImGuiIO& io = ImGui::GetIO();

			const ImRect bounds( pos , ImVec2( pos.x + size.x , pos.y + size.y ) );

			const bool hovered = bounds.Contains( io.MousePos );



			if ( hovered && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )

			{

				var->c_watermark.use_custom_position = true;

				var->c_watermark.custom_position = pos;

			}



			if ( var->c_watermark.use_custom_position && ImGui::IsMouseDragging( ImGuiMouseButton_Left ) && hovered )

			{

				var->c_watermark.custom_position.x += io.MouseDelta.x;

				var->c_watermark.custom_position.y += io.MouseDelta.y;

				return true;

			}



			return false;

		}

	}



	auto Update() noexcept -> void

	{

		var->c_watermark.watermark_content = BuildHudLines();

	}



	auto Render() noexcept -> void

	{

		if ( !var->c_watermark.watermark || !SyntheticUi::MenuFontsReady() )

			return;



		ImFont* const font = SyntheticUi::MenuFont( 0 );

		if ( !font )

			return;



		ImDrawList* const drawList = ImGui::GetBackgroundDrawList();

		if ( !drawList )

			return;



		const std::vector<std::string> lines = BuildHudLines();

		if ( lines.empty() )

			return;



		const ImVec2 contentSize = MeasureHudSize( font , lines );

		const bool useCustom = var->c_watermark.use_custom_position

			&& var->c_watermark.custom_position.x >= 0.f

			&& var->c_watermark.custom_position.y >= 0.f;



		ImVec2 targetPos = ResolveHudPosition(

			contentSize ,

			static_cast<watermark_position>( var->c_watermark.watermark_position ) ,

			useCustom ,

			var->c_watermark.custom_position );



		if ( !s_hud.pos_initialized )

		{

			s_hud.pos = targetPos;

			s_hud.pos_initialized = true;

		}



		if ( useCustom )

			s_hud.pos = targetPos;

		else

			s_hud.pos = ImLerp( s_hud.pos , targetPos , gui->fixed_speed( 25.f ) );



		s_hud.size = contentSize;



		if ( HandleHudDrag( s_hud.pos , s_hud.size , true ) )

			s_hud.pos = var->c_watermark.custom_position;



		DrawHudBackground( drawList , s_hud.pos , s_hud.size );

		DrawHudLine( drawList , font , s_hud.pos , lines );



		var->c_watermark.use_custom_position = useCustom || var->c_watermark.use_custom_position;

		var->c_watermark.custom_position = s_hud.pos;

		SyncHudLayoutToMenuSettings();



		if ( var->c_watermark.use_custom_position && ImGui::IsMouseReleased( ImGuiMouseButton_Left ) )

			SyntheticMenu::SyncUiSettingsToMenu();

	}



	auto RenderOverlay() noexcept -> void

	{

		if ( SyntheticMenu::IsInitialized() && var->c_watermark.watermark && SyntheticUi::MenuFontsReady() )

			Render();

		else if ( MenuEffects::config.watermark )

			MenuEffects::RenderWatermark();

	}

}


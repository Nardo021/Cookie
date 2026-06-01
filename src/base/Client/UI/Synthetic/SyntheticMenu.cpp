#include "SyntheticMenu.hpp"

#include <algorithm>

#include <Client/CCookieGUI.hpp>
#include <Client/UI/Menu/MenuSettings.hpp>
#include <Client/UI/Synthetic/SyntheticLuaRuntime.hpp>
#include <Common/DevLog.hpp>
#include <Common/Include/Config.hpp>
#include <framework/data/font.h>
#include <framework/data/texture.h>
#include <framework/settings/functions.h>

namespace SyntheticMenu
{
	namespace
	{
		bool s_initialized = false;

		auto LoadFonts() noexcept -> void
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFontConfig cfg{};
			cfg.FontDataOwnedByAtlas = false;

			set->c_font.inter_medium[0] = io.Fonts->AddFontFromMemoryTTF( inter_medium , sizeof( inter_medium ) , 15.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
			set->c_font.inter_medium[1] = io.Fonts->AddFontFromMemoryTTF( inter_medium , sizeof( inter_medium ) , 16.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );

			set->c_font.icon[0] = io.Fonts->AddFontFromMemoryTTF( icon , sizeof( icon ) , 14.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
			set->c_font.icon[1] = io.Fonts->AddFontFromMemoryTTF( icon , sizeof( icon ) , 16.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
			set->c_font.icon[2] = io.Fonts->AddFontFromMemoryTTF( icon , sizeof( icon ) , 40.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
			set->c_font.icon[3] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 15.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
			set->c_font.icon[4] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 9.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
			set->c_font.icon[5] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 76.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
			set->c_font.icon[6] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 96.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );

			set->c_font.name = io.Fonts->AddFontFromMemoryTTF( inter_medium , sizeof( inter_medium ) , 18.f , &cfg , io.Fonts->GetGlyphRangesCyrillic() );

			io.Fonts->Build();
			ImGui_ImplDX11_CreateDeviceObjects();
		}

		auto LoadTextures( ID3D11Device* device ) noexcept -> void
		{
			if ( !device )
				return;

			if ( set->c_texture.bg == nullptr )
			{
				D3DX11CreateShaderResourceViewFromMemory(
					device , background , sizeof( background ) , &g_dx11ImageInfo , pump , &set->c_texture.bg , 0 );
			}

			if ( set->c_texture.logo == nullptr )
			{
				D3DX11CreateShaderResourceViewFromMemory(
					device , logo , sizeof( logo ) , &g_dx11ImageInfo , pump , &set->c_texture.logo , 0 );
			}
		}
	}

	auto Init( ID3D11Device* device , ID3D11DeviceContext* context , IDXGISwapChain* swapChain ) noexcept -> void
	{
		if ( s_initialized )
			return;

		g_pd3dDevice = device;
		g_pd3dDeviceContext = context;
		g_pSwapChain = swapChain;

		var->c_watermark.watermark_content = { CHEAT_NAME , "FPS" , "PING" , "" };
		var->c_selection.selection_icon = { "R" , "L" , "A" , "V" , "S" , "C" , "U" , "M" };

		SyntheticLua::Init();
		ApplyPersistedUiSettings();
		SyntheticLua::SyncScriptList();
		if ( !MenuSettings::activeLuaScript.empty() )
		{
			var->c_lua.editable = MenuSettings::activeLuaScript;
			SyntheticLua::LoadScriptIntoEditor( MenuSettings::activeLuaScript );
		}

		LoadFonts();
		if ( !set->c_font.inter_medium[0] || !set->c_font.inter_medium[1] )
		{
			DEV_LOG( "[error] SyntheticMenu: core font load failed\n" );
			return;
		}

		LoadTextures( device );

		s_initialized = true;
	}

	auto Shutdown() noexcept -> void
	{
		if ( set->c_texture.bg )
		{
			set->c_texture.bg->Release();
			set->c_texture.bg = nullptr;
		}
		if ( set->c_texture.logo )
		{
			set->c_texture.logo->Release();
			set->c_texture.logo = nullptr;
		}

		SyntheticLua::Shutdown();

		s_initialized = false;
		g_pd3dDevice = nullptr;
		g_pd3dDeviceContext = nullptr;
		g_pSwapChain = nullptr;
	}

	auto OnDpiChanged() noexcept -> void
	{
		if ( !s_initialized )
			return;

		var->c_dpi.dpi = var->c_dpi.dpi_saved / 100.f;

		ImGuiIO& io = ImGui::GetIO();
		ImFontConfig cfg{};
		cfg.FontDataOwnedByAtlas = false;

		io.Fonts->Clear();

		const float dpi = var->c_dpi.dpi;
		set->c_font.inter_medium[0] = io.Fonts->AddFontFromMemoryTTF( inter_medium , sizeof( inter_medium ) , 15.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
		set->c_font.inter_medium[1] = io.Fonts->AddFontFromMemoryTTF( inter_medium , sizeof( inter_medium ) , 16.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );

		set->c_font.icon[0] = io.Fonts->AddFontFromMemoryTTF( icon , sizeof( icon ) , 14.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
		set->c_font.icon[1] = io.Fonts->AddFontFromMemoryTTF( icon , sizeof( icon ) , 16.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
		set->c_font.icon[2] = io.Fonts->AddFontFromMemoryTTF( icon , sizeof( icon ) , 40.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
		set->c_font.icon[3] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 15.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
		set->c_font.icon[4] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 9.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
		set->c_font.icon[5] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 76.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );
		set->c_font.icon[6] = io.Fonts->AddFontFromMemoryTTF( icon2 , sizeof( icon2 ) , 96.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );

		set->c_font.name = io.Fonts->AddFontFromMemoryTTF( inter_medium , sizeof( inter_medium ) , 18.f * dpi , &cfg , io.Fonts->GetGlyphRangesCyrillic() );

		var->c_dpi.dpi_changed = false;

		if ( auto* cookieGui = GetCookieGUI() )
			cookieGui->InitFont();

		io.Fonts->Build();
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	auto Render() noexcept -> void
	{
		if ( !s_initialized )
			return;

		if ( var->c_dpi.dpi_changed )
			OnDpiChanged();

		gui->render();
	}

	auto IsInitialized() noexcept -> bool
	{
		return s_initialized;
	}

	auto ApplyPersistedUiSettings() noexcept -> void
	{
		MenuSettings::menuDpiPercent = std::clamp( MenuSettings::menuDpiPercent , 100 , 200 );

		const float newDpi = MenuSettings::menuDpiPercent / 100.f;
		if ( var->c_dpi.dpi_saved != MenuSettings::menuDpiPercent || var->c_dpi.dpi != newDpi )
			var->c_dpi.dpi_changed = true;

		var->c_dpi.dpi_saved = MenuSettings::menuDpiPercent;
		var->c_dpi.dpi = newDpi;
		var->c_watermark.watermark = MenuSettings::syntheticWatermark;
		var->c_watermark.watermark_position = MenuSettings::syntheticWatermarkPosition;
		var->c_watermark.use_custom_position = MenuSettings::useCustomHudPosition;
		var->c_watermark.custom_position = ImVec2( MenuSettings::hudPositionX , MenuSettings::hudPositionY );
		var->c_notify.notify_position = MenuSettings::syntheticNotifyPosition;
	}

	auto SyncUiSettingsToMenu() noexcept -> void
	{
		MenuSettings::menuDpiPercent = std::clamp( var->c_dpi.dpi_saved , 100 , 200 );
		MenuSettings::syntheticWatermark = var->c_watermark.watermark;
		MenuSettings::syntheticWatermarkPosition = var->c_watermark.watermark_position;
		MenuSettings::useCustomHudPosition = var->c_watermark.use_custom_position;
		MenuSettings::hudPositionX = var->c_watermark.custom_position.x;
		MenuSettings::hudPositionY = var->c_watermark.custom_position.y;
		MenuSettings::syntheticNotifyPosition = var->c_notify.notify_position;
		if ( !var->c_lua.editable.empty() )
			MenuSettings::activeLuaScript = var->c_lua.editable;
	}
}

#pragma once

#include <ImGui/imgui.h>

namespace EmbeddedFonts
{
	// Font Awesome Solid — used by notifications and optional menu glyphs.
	auto InitFontAwesome( float sizePixels = 14.f ) noexcept -> ImFont*;
	auto GetFontAwesome() noexcept -> ImFont*;

	// Smallest Pixel-7 — optional ESP / compact UI font.
	auto InitSmallestPixel( float sizePixels ) noexcept -> ImFont*;
	auto GetSmallestPixel() noexcept -> ImFont*;

	// Lexend Bold — menu typography (embedded font.h).
	auto InitLexendBold( float sizePixels = 15.f ) noexcept -> ImFont*;
	auto GetLexendBold() noexcept -> ImFont*;

	auto Invalidate() noexcept -> void;

	inline auto FontBelongsToAtlas( ImFont* font ) noexcept -> bool
	{
		if ( !font )
			return false;

		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;
		if ( !atlas )
			return false;

		for ( ImFont* const atlasFont : atlas->Fonts )
		{
			if ( atlasFont == font )
				return true;
		}

		return false;
	}

	inline auto FontReady( ImFont* font ) noexcept -> bool
	{
		if ( !FontBelongsToAtlas( font ) )
			return false;

		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;
		return atlas && atlas->IsBuilt() && atlas->TexID != ImTextureID{} && font->IsLoaded();
	}
}

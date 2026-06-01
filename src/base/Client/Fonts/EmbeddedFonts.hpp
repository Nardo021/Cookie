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
}

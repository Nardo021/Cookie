#pragma once

#include <cstdint>

#include <ImGui/imgui.h>

// Font asset reference (MIT, from reference project):
//   Client/Resources/CS2GunIcons.ttf (runtime search paths in WeaponIcons.cpp)
//   embedded: game_icons.h, iconscs2_embedded.hpp
inline constexpr const char* kCS2GunIconsFontPath = "src/base/Client/Resources/CS2GunIcons.ttf";
inline constexpr const char* kIconScs2FontPath = "src/base/Client/Resources/iconscs2.ttf";

namespace WeaponIcons
{
	auto Init() noexcept -> bool;
	auto GetFont() noexcept -> ImFont*;
	auto GetKnifeFont() noexcept -> ImFont*;
	auto GetObsFont() noexcept -> ImFont*;

	// Returns a single UTF-16 code unit for CS2GunIcons / iconscs2 font glyphs.
	auto GetWeaponIconChar( int defIndex ) -> wchar_t;

	inline auto GetWeaponIconString( int defIndex ) -> const wchar_t*
	{
		static thread_local wchar_t buffer[2];
		buffer[0] = GetWeaponIconChar( defIndex );
		buffer[1] = L'\0';
		return buffer;
	}

} // namespace WeaponIcons

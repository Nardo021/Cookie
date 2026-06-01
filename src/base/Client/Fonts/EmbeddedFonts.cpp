#include "EmbeddedFonts.hpp"

#include "../Resources/embedded/fa_solid_900.h"
#include "../Resources/embedded/font_awesome_5.h"
#include "../Resources/embedded/font.h"
#include "../Resources/embedded/smallest_pixel.h"

namespace EmbeddedFonts
{
	namespace
	{
		ImFont* s_fontAwesome = nullptr;
		ImFont* s_smallestPixel = nullptr;
		ImFont* s_lexendBold = nullptr;

		auto FontAwesomeRanges() noexcept -> const ImWchar*
		{
			static const ImWchar ranges[] = {
				ICON_MIN_FA , ICON_MAX_FA ,
				0 ,
			};
			return ranges;
		}
	}

	auto InitFontAwesome( float sizePixels ) noexcept -> ImFont*
	{
		if ( FontReady( s_fontAwesome ) )
			return s_fontAwesome;

		ImFontConfig config{};
		config.OversampleH = 2;
		config.OversampleV = 2;

		s_fontAwesome = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
			const_cast<unsigned char*>( fa_solid_900 ) ,
			static_cast<int>( sizeof( fa_solid_900 ) ) ,
			sizePixels ,
			&config ,
			FontAwesomeRanges() );

		return s_fontAwesome;
	}

	auto GetFontAwesome() noexcept -> ImFont*
	{
		return s_fontAwesome;
	}

	auto InitSmallestPixel( float sizePixels ) noexcept -> ImFont*
	{
		if ( FontReady( s_smallestPixel ) )
			return s_smallestPixel;

		ImFontConfig config{};
		config.OversampleH = 1;
		config.OversampleV = 1;

		s_smallestPixel = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
			smallest_pixel_compressed_data ,
			static_cast<int>( smallest_pixel_compressed_size ) ,
			sizePixels ,
			&config ,
			ImGui::GetIO().Fonts->GetGlyphRangesDefault() );

		return s_smallestPixel;
	}

	auto GetSmallestPixel() noexcept -> ImFont*
	{
		return s_smallestPixel;
	}

	auto InitLexendBold( float sizePixels ) noexcept -> ImFont*
	{
		if ( FontReady( s_lexendBold ) )
			return s_lexendBold;

		ImFontConfig config{};
		config.OversampleH = 2;
		config.OversampleV = 2;

		s_lexendBold = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
			lexend_bold ,
			static_cast<int>( sizeof( lexend_bold ) ) ,
			sizePixels ,
			&config ,
			ImGui::GetIO().Fonts->GetGlyphRangesDefault() );

		return s_lexendBold;
	}

	auto GetLexendBold() noexcept -> ImFont*
	{
		return s_lexendBold;
	}

	auto Invalidate() noexcept -> void
	{
		s_fontAwesome = nullptr;
		s_smallestPixel = nullptr;
		s_lexendBold = nullptr;
	}
}

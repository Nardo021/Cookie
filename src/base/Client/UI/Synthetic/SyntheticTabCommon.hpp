#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include <Client/UI/Synthetic/SyntheticBinds.hpp>
#include <framework/settings/functions.h>

namespace SyntheticUI
{
	inline auto UiSeparator() noexcept -> void
	{
		widget->separator();
	}

	inline auto UiCheckbox( const char* label , bool* value ) noexcept -> bool
	{
		return widget->checkbox( label , value );
	}

	inline auto UiCheckboxWithKey(
		const char* label ,
		bool* enabled ,
		int* key ,
		SyntheticBinds::KeyBindUiState bindUi ) noexcept -> bool
	{
		bool holdMode = *bindUi.holdMode;
		bool useKey = *bindUi.useKey;
		bool showInBinds = *bindUi.showInBinds;
		const bool changed = widget->checkbox_with_key( label , enabled , key , &holdMode , &useKey , &showInBinds );
		*bindUi.holdMode = holdMode;
		*bindUi.useKey = useKey;
		*bindUi.showInBinds = showInBinds;
		SyntheticBinds::Register( label , enabled , key , bindUi );
		return changed;
	}

	inline auto UiSliderInt( const char* label , int* value , int minV , int maxV , int step , const char* fmt ) noexcept -> bool
	{
		return widget->slider_int( label , value , minV , maxV , step , fmt );
	}

	inline auto UiSliderFloat( const char* label , float* value , float minV , float maxV , float step , const char* fmt ) noexcept -> bool
	{
		return widget->slider_float( label , value , minV , maxV , step , fmt );
	}

	inline auto UiKeyBind( const char* label , int* key ) noexcept -> bool
	{
		return widget->keybind( label , key );
	}

	inline auto UiCombo( const char* label , int* selection , const std::vector<std::string>& items ) noexcept -> bool
	{
		return widget->dropdown( label , selection , items , static_cast<int>( items.size() ) );
	}

	inline auto ItemStrings( std::initializer_list<const char*> items ) -> std::vector<std::string>
	{
		std::vector<std::string> out;
		out.reserve( items.size() );
		for ( const char* s : items )
			out.emplace_back( s );
		return out;
	}

	inline auto UiColorCheckbox( const char* label , bool* enabled , float color[4] ) noexcept -> bool
	{
		return widget->checkbox_with_color( label , enabled , color , true );
	}
}

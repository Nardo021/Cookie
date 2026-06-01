#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include <Client/UI/Synthetic/SyntheticBinds.hpp>
#include <framework/settings/functions.h>

namespace SyntheticUI
{
	inline auto Separator() noexcept -> void
	{
		widget->separator();
	}

	inline auto Checkbox( const char* label , bool* value ) noexcept -> bool
	{
		return widget->checkbox( label , value );
	}

	inline auto CheckboxWithKey(
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

	inline auto SliderInt( const char* label , int* value , int minV , int maxV , int step , const char* fmt ) noexcept -> bool
	{
		return widget->slider_int( label , value , minV , maxV , step , fmt );
	}

	inline auto SliderFloat( const char* label , float* value , float minV , float maxV , float step , const char* fmt ) noexcept -> bool
	{
		return widget->slider_float( label , value , minV , maxV , step , fmt );
	}

	inline auto KeyBind( const char* label , int* key ) noexcept -> bool
	{
		return widget->keybind( label , key );
	}

	inline auto Combo( const char* label , int* selection , const std::vector<std::string>& items ) noexcept -> bool
	{
		return widget->dropdown( label , selection , items , static_cast<int>( items.size() ) );
	}

	inline auto Strings( std::initializer_list<const char*> items ) -> std::vector<std::string>
	{
		return std::vector<std::string>( items );
	}

	inline auto ColorCheckbox( const char* label , bool* enabled , float color[4] ) noexcept -> bool
	{
		return widget->checkbox_with_color( label , enabled , color , true );
	}
}

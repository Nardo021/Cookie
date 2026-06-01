#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Common/Common.hpp>
#include <ImGui/imgui.h>
#include <Windows.h>

#include <cstdint>

enum class EKeyBindMode : int
{
	HOLD = 0 ,
	TOGGLE
};

struct KeyBind_t
{
	constexpr KeyBind_t( const char* name = nullptr , unsigned int key = 0U , EKeyBindMode mode = EKeyBindMode::HOLD ) :
		szName( name ) , uKey( key ) , nMode( mode )
	{
	}

	bool bEnable = false;
	const char* szName = nullptr;
	unsigned int uKey = 0U;
	EKeyBindMode nMode = EKeyBindMode::HOLD;
};

class CInputSystem final
{
public:
	using KeyState_t = std::uint8_t;

	enum EKeyState : KeyState_t
	{
		KEY_STATE_NONE ,
		KEY_STATE_DOWN ,
		KEY_STATE_UP ,
		KEY_STATE_RELEASED
	};

	auto OnWndProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam ) -> bool;
	auto Update() -> void;

	auto GetBindState( KeyBind_t& keyBind ) -> bool;

	[[nodiscard]] auto IsKeyDown( std::uint32_t keyCode ) const -> bool
	{
		return m_arrKeyState[keyCode] == KEY_STATE_DOWN;
	}

	[[nodiscard]] auto IsKeyReleased( std::uint32_t keyCode ) -> bool
	{
		if ( m_arrKeyState[keyCode] == KEY_STATE_RELEASED )
		{
			m_arrKeyState[keyCode] = KEY_STATE_UP;
			return true;
		}

		return false;
	}

private:
	KeyState_t m_arrKeyState[256]{};
};

auto GetInputSystem() -> CInputSystem*;

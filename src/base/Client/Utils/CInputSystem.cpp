#include "CInputSystem.hpp"

#include <windowsx.h>

static CInputSystem g_CInputSystem{};

auto CInputSystem::OnWndProc( HWND /*hWnd*/ , UINT uMsg , WPARAM wParam , LPARAM /*lParam*/ ) -> bool
{
	int key = 0;
	KeyState_t state = KEY_STATE_NONE;

	switch ( uMsg )
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if ( wParam < 256U )
		{
			key = static_cast<int>( wParam );
			state = KEY_STATE_DOWN;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if ( wParam < 256U )
		{
			key = static_cast<int>( wParam );
			state = KEY_STATE_UP;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		key = VK_LBUTTON;
		state = uMsg == WM_LBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		key = VK_RBUTTON;
		state = uMsg == WM_RBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		key = VK_MBUTTON;
		state = uMsg == WM_MBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		key = ( GET_XBUTTON_WPARAM( wParam ) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2 );
		state = uMsg == WM_XBUTTONUP ? KEY_STATE_UP : KEY_STATE_DOWN;
		break;
	default:
		return false;
	}

	if ( state == KEY_STATE_UP && m_arrKeyState[key] == KEY_STATE_DOWN )
		m_arrKeyState[key] = KEY_STATE_RELEASED;
	else if ( state != KEY_STATE_NONE )
		m_arrKeyState[key] = state;

	return true;
}

auto CInputSystem::Update() -> void
{
	for ( auto& keyState : m_arrKeyState )
	{
		if ( keyState == KEY_STATE_RELEASED )
			keyState = KEY_STATE_UP;
	}
}

auto CInputSystem::GetBindState( KeyBind_t& keyBind ) -> bool
{
	if ( keyBind.uKey == 0U )
	{
		keyBind.bEnable = false;
		return false;
	}

	switch ( keyBind.nMode )
	{
	case EKeyBindMode::HOLD:
		keyBind.bEnable = IsKeyDown( keyBind.uKey );
		break;
	case EKeyBindMode::TOGGLE:
		if ( IsKeyReleased( keyBind.uKey ) )
			keyBind.bEnable = !keyBind.bEnable;
		break;
	default:
		break;
	}

	return keyBind.bEnable;
}

auto GetInputSystem() -> CInputSystem*
{
	return &g_CInputSystem;
}

#include "Hook_IsRelativeMouseMode.hpp"

#include <Client/CCookieGUI.hpp>

auto Hook_IsRelativeMouseMode( CInputSystem* pInputSystem , bool Active ) -> void
{
	GetCookieGUI()->m_bMainActive = Active;

	if ( GetCookieGUI()->IsVisible() )
		return IsRelativeMouseMode_o( pInputSystem , false );

	return IsRelativeMouseMode_o( pInputSystem , Active );
}

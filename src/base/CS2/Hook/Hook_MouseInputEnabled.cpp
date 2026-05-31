#include "Hook_MouseInputEnabled.hpp"

#include <Client/CCookieGUI.hpp>

auto Hook_MouseInputEnabled( void* RCX ) -> bool
{
	if ( GetCookieGUI()->IsVisible() )
		return false;

	return MouseInputEnabled_o( RCX );
}

#include "SyntheticNotifyBridge.hpp"

#include <Client/CCookieGUI.hpp>
#include <framework/settings/functions.h>

namespace SyntheticNotifyBridge
{
	auto Push( ENotificationType type , const char* text ) noexcept -> void
	{
		if ( !text || !text[0] )
			return;

		GetNotify()->Push( type , "%s" , text );

		if ( GetCookieGUI() && GetCookieGUI()->IsVisible() )
		{
			notify->add_notify(
				text ,
				4.f ,
				static_cast<notify_position>( var->c_notify.notify_position ) );
		}
	}
}

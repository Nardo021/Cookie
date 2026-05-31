#include "Hook_OnClientOutput.hpp"

#include <Client/CCookieClient.hpp>

auto Hook_OnClientOutput( void* a1 , void* a2 , void* a3 , void* a4 ) -> void
{
	GetCookieClient()->OnClientOutput();

	return OnClientOutput_o( a1 , a2 , a3 , a4 );
}

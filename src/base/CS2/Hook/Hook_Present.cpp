#include "Hook_Present.hpp"

#include <Client/CCookieGUI.hpp>
#include <Client/Utils/CInputSystem.hpp>
#include <Client/Utils/CNotify.hpp>

auto Hook_Present( IDXGISwapChain* pSwapChain , UINT SyncInterval , UINT Flags ) -> HRESULT
{
	GetCookieGUI()->OnPresent( pSwapChain );

	GetNotify()->Render();

	return Present_o( pSwapChain , SyncInterval , Flags );
}

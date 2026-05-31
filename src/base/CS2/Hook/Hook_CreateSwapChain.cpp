#include "Hook_CreateSwapChain.hpp"

#include <Client/CCookieGUI.hpp>

auto WINAPI Hook_CreateSwapChain( IDXGIFactory* pFactory , IUnknown* pDevice , DXGI_SWAP_CHAIN_DESC* pDesc , IDXGISwapChain** ppSwapChain )->HRESULT
{
	GetCookieGUI()->ClearRenderTargetView();

	return CreateSwapChain_o( pFactory , pDevice , pDesc , ppSwapChain );
}

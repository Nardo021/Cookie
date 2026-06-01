#pragma once

#include <d3d11.h>
#include <dxgi.h>

namespace SyntheticMenu
{
	auto Init( ID3D11Device* device , ID3D11DeviceContext* context , IDXGISwapChain* swapChain ) noexcept -> void;
	auto Shutdown() noexcept -> void;
	auto OnDpiChanged() noexcept -> void;
	auto Render() noexcept -> void;
	auto IsInitialized() noexcept -> bool;

	auto ApplyPersistedUiSettings() noexcept -> void;
	auto SyncUiSettingsToMenu() noexcept -> void;
}

#pragma once

#include <d3d11.h>

#include <ImGui/imgui.h>

namespace MenuAssets
{
	auto Init( ID3D11Device* device ) noexcept -> bool;
	auto Shutdown() noexcept -> void;

	auto GetMenuBackgroundTexture() noexcept -> ImTextureID;
	auto GetEspPreviewTexture() noexcept -> ImTextureID;

	auto RenderMenuBackgroundImage( ImDrawList* draw , ImVec2 origin , ImVec2 size , float alpha ) noexcept -> void;
	auto RenderEspPreviewPanel( ImDrawList* draw , ImVec2 origin , ImVec2 size ) noexcept -> void;

} // namespace MenuAssets

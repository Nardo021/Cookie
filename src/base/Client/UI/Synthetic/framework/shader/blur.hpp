#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include <d3d11.h>

#include <Client/UI/Menu/MenuEffects.hpp>

inline void draw_background_blur(
	ImDrawList* draw_list ,
	IDXGISwapChain* swap_chain ,
	ID3D11Device* device ,
	ID3D11DeviceContext* ctx ,
	ImVec2 start ,
	ImVec2 end ,
	int rounding = 0 ,
	ImDrawFlags flags = 0 )
{
	(void)swap_chain;
	(void)device;
	(void)ctx;
	(void)flags;

	MenuEffects::DrawPanelBlurOverlay( draw_list , start , end , static_cast<float>( rounding ) );
}

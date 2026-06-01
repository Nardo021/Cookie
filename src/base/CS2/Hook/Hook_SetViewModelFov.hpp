#pragma once

#include <Common/Common.hpp>

auto Hook_SetViewModelFov( void* rcx ) -> float;

using SetViewModelFov_t = decltype( &Hook_SetViewModelFov );
inline SetViewModelFov_t SetViewModelFov_o = nullptr;

struct ViewModelFovConfig
{
	bool  enabled = false;
	float amount = 68.f;
};

inline ViewModelFovConfig g_viewModelFovConfig{};

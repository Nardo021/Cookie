#pragma once

#include <Common/Common.hpp>

auto Hook_GetRenderFov( void* rcx ) -> float;

using GetRenderFov_t = decltype( &Hook_GetRenderFov );
inline GetRenderFov_t GetRenderFov_o = nullptr;

inline float g_fovOverride = 0.f;

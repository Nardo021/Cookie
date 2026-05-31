#pragma once

#include <Common/Common.hpp>

class CCSGOInput;

auto Hook_AllowCameraChange( CCSGOInput* input , void* a2 ) -> void;

using AllowCameraChange_t = decltype( &Hook_AllowCameraChange );

inline AllowCameraChange_t AllowCameraChange_o = nullptr;

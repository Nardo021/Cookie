#pragma once

#include <Common/Common.hpp>

auto Hook_OverrideView( void* rcx , void* pSetup ) -> void;

using OverrideView_t = decltype( &Hook_OverrideView );
inline OverrideView_t OverrideView_o = nullptr;

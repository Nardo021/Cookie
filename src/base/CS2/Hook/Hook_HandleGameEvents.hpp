#pragma once

#include <Common/Common.hpp>

class IGameEvent;

auto Hook_HandleGameEvents( void* rcx , IGameEvent* event ) -> void;

using HandleGameEvents_t = decltype( &Hook_HandleGameEvents );
inline HandleGameEvents_t HandleGameEvents_o = nullptr;

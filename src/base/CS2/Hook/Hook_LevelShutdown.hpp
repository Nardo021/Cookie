#pragma once

#include <Common/Common.hpp>

auto Hook_LevelShutdown( void* clientMode ) -> void;

using LevelShutdown_t = decltype( &Hook_LevelShutdown );
inline LevelShutdown_t LevelShutdown_o = nullptr;

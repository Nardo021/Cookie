#pragma once

#include <Common/Common.hpp>

auto Hook_LevelInit( void* clientMode , const char* mapName ) -> void*;

using LevelInit_t = decltype( &Hook_LevelInit );
inline LevelInit_t LevelInit_o = nullptr;

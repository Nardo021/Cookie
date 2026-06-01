#pragma once

#include <Common/Common.hpp>

class C_BaseModelEntity;

auto Hook_SetModel( C_BaseModelEntity* entity , const char* modelName ) -> void;

using SetModel_t = decltype( &Hook_SetModel );
inline SetModel_t SetModel_o = nullptr;

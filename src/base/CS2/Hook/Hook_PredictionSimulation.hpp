#pragma once

#include <Common/Common.hpp>

class CCSGOInput;
class CUserCmd;

auto Hook_PredictionSimulation( CCSGOInput* input , int slot , CUserCmd* cmd ) -> void;

using PredictionSimulation_t = decltype( &Hook_PredictionSimulation );
inline PredictionSimulation_t PredictionSimulation_o = nullptr;

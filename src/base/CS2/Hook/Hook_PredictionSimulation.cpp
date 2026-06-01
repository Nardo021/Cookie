#include "Hook_PredictionSimulation.hpp"

#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <Client/Features/Combat/EnginePred.hpp>

auto Hook_PredictionSimulation( CCSGOInput* input , int slot , CUserCmd* cmd ) -> void
{
	EnginePred::Start( cmd );
	PredictionSimulation_o( input , slot , cmd );
	EnginePred::End();
}

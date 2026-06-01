#pragma once

#include <cstdint>

class CUserCmd;
class C_CSPlayerPawn;

namespace EnginePred
{
	auto Start( CUserCmd* cmd ) noexcept -> void;
	auto End() noexcept -> void;
	auto IsActive() noexcept -> bool;
	auto GetPredictedTick() noexcept -> int;
	auto GetPredictedFlags() noexcept -> uint32_t;
	auto GetPawnFlags( C_CSPlayerPawn* pawn ) noexcept -> uint32_t;
	auto IsOnGround( C_CSPlayerPawn* pawn ) noexcept -> bool;
}

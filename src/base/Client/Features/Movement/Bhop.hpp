#pragma once

#include <Windows.h>
#include <cstdint>

class CCSGOInput;
class CUserCmd;

namespace Bhop
{
	struct BhopConfig
	{
		bool enabled = false;
		bool edgeJump = false;
		bool jumpBug = true;
		bool autoForward = true;
		bool requireSpace = true;
		int  hopChance = 100;
	};

	extern BhopConfig config;

	inline auto NeedsTrace() noexcept -> bool
	{
		return config.edgeJump || config.jumpBug;
	}

	auto Process( CCSGOInput* input , CUserCmd* cmd ) noexcept -> void;
}

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
		bool autoStrafe = false;
		bool autoForward = true;
		bool requireSpace = false;
	};

	extern BhopConfig config;

	auto Process( CCSGOInput* input , CUserCmd* cmd ) noexcept -> void;
}

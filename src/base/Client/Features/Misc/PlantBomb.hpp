#pragma once

class CCSGOInput;
class CUserCmd;

namespace PlantBomb
{
	struct Config
	{
		bool enabled = false;
	};

	inline Config config;

	auto Process( CCSGOInput* input , CUserCmd* cmd ) noexcept -> void;
	auto OnFrameStageNotify( int frameStage ) noexcept -> void;
}

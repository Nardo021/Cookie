#pragma once

namespace WorldVisuals
{
	struct Config
	{
		bool  nightMode = false;
		float nightAmbient = 0.55f;
		bool  worldModulate = false;
		float modulateColor[4] = { 0.15f , 0.08f , 0.22f , 1.f };
		bool  noShadow = false;
	};

	inline Config config;

	auto OnFrameStageNotify( int frameStage ) noexcept -> void;
	auto Shutdown() noexcept -> void;

} // namespace WorldVisuals

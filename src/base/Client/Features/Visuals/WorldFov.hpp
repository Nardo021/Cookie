#pragma once

namespace WorldFov
{
	struct Config
	{
		bool  enabled = false;
		float amount = 90.f;
	};

	inline Config config;

	auto ApplyOverride( float original , bool scoped ) noexcept -> float;

} // namespace WorldFov

#pragma once

#include <cstdint>

class CCSGOInput;
class CViewSetup;

namespace ThirdPerson
{
	struct Config
	{
		static constexpr int kFovDefault = 90;
		static constexpr float kDistanceDefault = 150.f;

		bool enabled = false;
		int fov = kFovDefault;
		float distance = kDistanceDefault;
	};

	inline Config config;

	auto ApplyInput( CCSGOInput* input ) noexcept -> void;
	auto ApplyView( CViewSetup* setup ) noexcept -> void;
	auto Shutdown() noexcept -> void;
}

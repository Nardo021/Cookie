#pragma once

#include <Windows.h>

#include <string>

namespace MenuSettings
{
	inline int menuToggleKey = VK_INSERT;
	inline int menuDpiPercent = 100;
	inline bool syntheticWatermark = true;
	inline int syntheticWatermarkPosition = 1;
	inline int syntheticNotifyPosition = 0;
	inline std::string activeLuaScript;

	inline bool useCustomHudPosition = false;
	inline float hudPositionX = -1.f;
	inline float hudPositionY = -1.f;
}

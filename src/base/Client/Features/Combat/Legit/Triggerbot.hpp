#pragma once

#include <Windows.h>
#include <cstdint>

#include <Client/Game/Game.hpp>

class CUserCmd;

namespace Triggerbot
{
	struct TriggerConfig
	{
		bool enabled = false;
		int key = VK_MENU;
		bool useKey = true;
		bool teamCheck = true;
		bool visCheck = true;
		int delayMs = 15;
		int hitchance = 0;
	};

	inline TriggerConfig config;

	auto Tick(
		CUserCmd* cmd ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		const Game::QAngle& viewAngles ) -> void;
}

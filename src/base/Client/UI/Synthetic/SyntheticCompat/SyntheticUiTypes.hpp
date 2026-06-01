#pragma once

#include "imgui.h"

enum watermark_position : int
{
	mark_top_left,
	mark_top_right,
	mark_bottom_left,
	mark_bottom_right,
};

struct watermark_layout
{
	bool use_custom_pos = false;
	ImVec2 custom_pos{ -1.f, -1.f };
	bool draggable = false;
	bool request_settings_sync = false;
};

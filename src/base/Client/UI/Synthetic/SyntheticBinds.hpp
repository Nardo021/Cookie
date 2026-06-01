#pragma once

namespace SyntheticBinds
{
	struct KeyBindUiState
	{
		bool* holdMode = nullptr;
		bool* useKey = nullptr;
		bool* showInBinds = nullptr;
	};

	auto BeginFrame() noexcept -> void;
	auto Register(
		const char* label ,
		bool* enabled ,
		int* key ,
		KeyBindUiState ui ) noexcept -> void;

	auto RenderOverlay() noexcept -> void;
}

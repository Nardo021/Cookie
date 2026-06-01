#pragma once

#include <cstdint>

#include <ImGui/imgui.h>

class VMatrix;

namespace EspBBoxCache
{
	struct Entry
	{
		ImVec4 box{};
		bool   visible = false;
	};

	auto UpdateViewMatrix( const VMatrix* worldToProjection ) noexcept -> void;
	auto CalculateBoundingBoxes() noexcept -> void;
	auto TryGetBox( uint32_t pawnHandle , ImVec4& outBox ) noexcept -> bool;

} // namespace EspBBoxCache

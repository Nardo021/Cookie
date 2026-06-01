#pragma once

namespace SyntheticWatermark
{
	auto Update() noexcept -> void;

	/// 刷新内容并绘制 Synthetic 水印（菜单开/关均可调用）
	auto Render() noexcept -> void;
}

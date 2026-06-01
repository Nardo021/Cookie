#pragma once

namespace SyntheticConfig
{
	auto InvalidateConfigList() noexcept -> void;
	auto SyncConfigListFromDisk() noexcept -> void;
	auto RenderConfigTab() noexcept -> void;
}

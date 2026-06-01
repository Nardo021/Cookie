#pragma once

#include <string>
#include <vector>

namespace FeatureInit
{
	struct ModuleStatus
	{
		const char* name = nullptr;
		bool ok = false;
	};

	auto Init() noexcept -> std::vector<ModuleStatus>;
	auto VerifyHooks() noexcept -> void;
	auto Shutdown() noexcept -> void;
	auto AllReady() noexcept -> bool;
}

#include "hooks.hpp"
#include "memory.hpp"

#include <algorithm>

namespace
{
	constexpr const char* kLoadingPatterns[] = {
		"55 8B EC 83 EC 28 53 56 8B 75 08 8B",
		"55 8B EC 83 EC 28 53 56 57 8B 7D 08",
	};

	constexpr const char* kCallingPatterns[] = {
		"55 8B EC 6A FF 68 ? ? ? ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 6C 53 56",
		"55 8B EC 6A FF 68 ? ? ? ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 68 53 56",
		"55 8B EC 83 EC 6C 53 56 57 8B F9 8B 4D 08",
	};
}

bool hooks::initialize()
{
	auto* const hSteamService = GetModuleHandleA("steamservice.dll");
	if (!hSteamService)
	{
		valve::msg("[ VAC3 ] steamservice.dll is not loaded\n");
		return false;
	}

	{
		auto* const dwLoading = memory::scan_first(hSteamService, { kLoadingPatterns[0], kLoadingPatterns[1] });
		if (!dwLoading)
		{
			valve::msg("[ VAC3 ] dw_loading pattern not found\n");
			return false;
		}

		o_loading = reinterpret_cast<loading_t>(memory::tramp_hook(
			reinterpret_cast<std::uintptr_t*>(dwLoading),
			reinterpret_cast<std::uintptr_t>(hk_loading),
			6));
	}

	{
		auto* const dwCalling = memory::scan_first(hSteamService, { kCallingPatterns[0], kCallingPatterns[1], kCallingPatterns[2] });
		if (!dwCalling)
		{
			valve::msg("[ VAC3 ] dw_calling pattern not found\n");
			return false;
		}

		o_calling = reinterpret_cast<calling_t>(memory::tramp_hook(
			reinterpret_cast<std::uintptr_t*>(dwCalling),
			reinterpret_cast<std::uintptr_t>(hk_calling),
			5));
	}

	valve::msg("[ VAC3 ] hooks ready\n");
	return true;
}

bool __stdcall hooks::hk_loading(valve::vac_buffer* hMod, char injectionFlags)
{
	const auto ntHeaderCrc32 = memory::hash_header(hMod->m_pRawModule);
	const auto bRet = o_loading(hMod, injectionFlags);

	if (ntHeaderCrc32)
	{
		valve::msg("[ VAC3 ] loading module crc32 [ 0x%.8X ]\n", ntHeaderCrc32);

		if (ntHeaderCrc32 == 0xCC29049A || ntHeaderCrc32 == 0x2B8DD987)
			valve::uid_whitelist.push_back(hMod->m_unCRC32);
	}

	if (hMod->m_unCRC32 && std::find(valve::uid_whitelist.begin(), valve::uid_whitelist.end(), hMod->m_unCRC32) != valve::uid_whitelist.end())
		return bRet;

	if (hMod->m_pRunFunc)
		hMod->m_pRunFunc = nullptr;

	return bRet;
}

int __fastcall hooks::hk_calling(void* ecx, void* edx, std::uint32_t crcHash, char injectionMode, int unused1, int id, int param1, int unused2, int param2, int param3, int* param4, int* sizeCheck)
{
	auto status = o_calling(ecx, edx, crcHash, injectionMode, unused1, id, param1, unused2, param2, param3, param4, sizeCheck);

	if (status != valve::SUCCESS && status != valve::OTHER_SUCCESS)
		status = valve::SUCCESS;

	return status;
}

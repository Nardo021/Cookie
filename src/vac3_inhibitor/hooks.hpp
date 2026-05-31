#pragma once

#include "valve.hpp"

namespace hooks
{
	bool initialize();

	using loading_t = bool(__stdcall*)(valve::vac_buffer* hMod, char injectionFlags);
	inline loading_t o_loading{};

	using calling_t = int(__fastcall*)(void* ecx, void* edx, std::uint32_t crcHash, char injectionMode, int unused1, int id, int param1, int unused2, int param2, int param3, int* param4, int* sizeCheck);
	inline calling_t o_calling{};

	bool __stdcall hk_loading(valve::vac_buffer* hMod, char injectionFlags);
	int __fastcall hk_calling(void* ecx, void* edx, std::uint32_t crcHash, char injectionMode, int unused1, int id, int param1, int unused2, int param2, int param3, int* param4, int* sizeCheck);
}

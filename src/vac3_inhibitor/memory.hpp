#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace memory
{
	inline std::uint8_t* pattern_scan(std::uint8_t* start, std::size_t size, const std::vector<std::uint8_t>& pattern, const std::vector<bool>& mask)
	{
		if (pattern.empty() || pattern.size() != mask.size() || size < pattern.size())
			return nullptr;

		for (std::size_t i = 0; i <= size - pattern.size(); ++i)
		{
			bool found = true;
			for (std::size_t j = 0; j < pattern.size(); ++j)
			{
				if (mask[j] && start[i + j] != pattern[j])
				{
					found = false;
					break;
				}
			}

			if (found)
				return start + i;
		}

		return nullptr;
	}

	inline std::uint8_t* pattern_scan_ida(HMODULE module, const char* pattern)
	{
		if (!module)
			return nullptr;

		std::vector<std::uint8_t> bytes;
		std::vector<bool> mask;

		for (const char* current = pattern; *current; ++current)
		{
			if (*current == ' ')
				continue;

			if (*current == '?')
			{
				bytes.push_back(0);
				mask.push_back(false);
				if (current[1] == '?')
					++current;
				continue;
			}

			char byteStr[3] = { current[0], current[1], '\0' };
			bytes.push_back(static_cast<std::uint8_t>(strtoul(byteStr, nullptr, 16)));
			mask.push_back(true);
			++current;
		}

		const auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
		const auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(module) + dos->e_lfanew);
		const auto size = nt->OptionalHeader.SizeOfImage;
		return pattern_scan(reinterpret_cast<std::uint8_t*>(module), size, bytes, mask);
	}

	inline std::uint8_t* scan_first(HMODULE module, std::initializer_list<const char*> patterns)
	{
		for (const char* pattern : patterns)
		{
			if (auto* match = pattern_scan_ida(module, pattern))
				return match;
		}

		return nullptr;
	}

	inline std::uint32_t hash_header(void* rawModule)
	{
		if (!rawModule)
			return 0;

		const auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(rawModule);
		if (dos->e_magic != IMAGE_DOS_SIGNATURE)
			return 0;

		const auto nt = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(rawModule) + dos->e_lfanew);
		if (nt->Signature != IMAGE_NT_SIGNATURE)
			return 0;

		return nt->OptionalHeader.CheckSum;
	}

	inline void* tramp_hook(std::uintptr_t* target, std::uintptr_t detour, std::size_t length)
	{
		DWORD oldProtect{};
		VirtualProtect(target, length, PAGE_EXECUTE_READWRITE, &oldProtect);

		const auto gateway = static_cast<std::uint8_t*>(VirtualAlloc(nullptr, length + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
		if (!gateway)
			return nullptr;

		std::memcpy(gateway, target, length);
		gateway[length] = 0xE9;
		*reinterpret_cast<std::uint32_t*>(gateway + length + 1) =
			static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(target) + length - (reinterpret_cast<std::uintptr_t>(gateway) + length + 5));

		target[0] = 0xE9;
		*reinterpret_cast<std::uint32_t*>(&target[1]) =
			static_cast<std::uint32_t>(detour - (reinterpret_cast<std::uintptr_t>(target) + 5));

		VirtualProtect(target, length, oldProtect, &oldProtect);
		return gateway;
	}
}

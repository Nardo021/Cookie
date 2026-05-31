#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <vector>

namespace valve
{
	inline void msg(const char* message, ...)
	{
		auto* const hTier0 = GetModuleHandleA("tier0_s.dll");
		if (!hTier0)
			return;

		using fn = void(__cdecl*)(const char* msg, va_list);
		const auto log = reinterpret_cast<fn>(GetProcAddress(hTier0, "Msg"));
		if (!log)
			return;

		char buf[1024]{};
		va_list list;
		va_start(list, message);
		vsprintf_s(buf, message, list);
		va_end(list);

		log(buf, list);
	}

	enum vac_result_t
	{
		SUCCESS = 1,
		OTHER_SUCCESS = 2,
		ENTRY_POINT_FAIL = 16,
		MODULE_LOAD_FAILED = 22,
	};

	struct mapped_module
	{
		std::uint8_t pad04[4];
		void* m_pModuleBase;
		PIMAGE_NT_HEADERS m_pNTHeaders;
	};

	struct vac_buffer
	{
		std::uint32_t m_unCRC32;
		std::uint8_t pad04[4];
		mapped_module* m_pMappedModule;
		void* m_pRunFunc;
		int m_nLastResult;
		int m_nModuleSize;
		void* m_pRawModule;
	};

	inline std::vector<std::uint32_t> uid_whitelist{};
}

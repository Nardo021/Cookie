#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace vars
{
	inline std::wstring_view str_steam_process_name{ L"steam.exe" };
	inline std::wstring_view str_game_process_name{ L"cs2.exe" };
	inline std::wstring_view str_game_mod_name{ L"matchmaking.dll" };
	inline std::wstring_view str_embedded_dll_name{ L"Cookie.dll" };

	// VAC runs inside the 32-bit Steam service process on current Steam builds.
	inline std::wstring_view str_vac_service_process{ L"steamservice.exe" };
	inline std::wstring_view str_vac_service_module{ L"steamservice.dll" };

	inline const std::vector<std::wstring_view> vac_host_processes{
		L"steamservice.exe",
		L"SteamService.exe",
		L"steam.exe",
	};

	inline const std::vector<std::wstring_view> vac_service_modules{
		L"steamservice.dll",
		L"SteamService.dll",
	};
}

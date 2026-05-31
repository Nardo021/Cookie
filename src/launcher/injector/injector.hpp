#pragma once

#include <BlackBone/Process/Process.h>
#include "memory/memory.hpp"

#include <chrono>
#include <filesystem>

class Injector
{
public:
	Injector() = default;
	~Injector() = default;

	bool bypassVAC();

	bool inject();

	static void injectThreadEntry();

	bool vacBypassed{ false };
	bool steamRunning{ false };
	bool csgoRunning{ false };

private:
	bool map(std::wstring_view procname, std::wstring_view modname, std::vector<BYTE> buffer, blackbone::eLoadFlags flags = blackbone::WipeHeader);

	bool waitForProcess(std::wstring_view procname, DWORD& pid, std::chrono::seconds timeout) const;
	bool waitForModule(blackbone::Process& proc, std::wstring_view modname, std::chrono::seconds timeout) const;
	bool waitForVacHost(mem::VacHostInfo& host, std::chrono::seconds timeout) const;
	bool findVacHostByAttach(mem::VacHostInfo& host) const;
	bool tryLaunchSteamService() const;

	bool attachProcess(blackbone::Process& proc, DWORD pid, std::string_view label) const;
	bool attachProcessByName(blackbone::Process& proc, std::wstring_view procname, std::string_view label, DWORD& pid) const;

	bool injectLoadLibrary(const std::filesystem::path& dllPath);

	void closeProcesses(std::vector<std::wstring> processes);
};

inline auto g_injector = std::make_unique<Injector>();
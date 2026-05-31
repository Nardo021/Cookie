#pragma once
#include "utils/utils.hpp"
#include "vars/vars.hpp"

#include <TlHelp32.h>
#include <vector>

namespace mem
{
	struct CompareProc {
		bool operator()(const std::pair<std::uint32_t, std::wstring>& lhs, const std::pair<std::uint32_t, std::wstring>& rhs) const {
			if (lhs.second != rhs.second)
				return lhs.second < rhs.second;
			return lhs.first < rhs.first;
		}
	};

	inline bool isSystemProcess(const std::wstring& name) {
		static const std::set<std::wstring> systemProcesses = {
			L"System", L"svchost.exe", L"csrss.exe", L"smss.exe", L"wininit.exe", L"services.exe"
		};
		return systemProcesses.find(name) != systemProcesses.end();
	}

	inline std::set<std::pair<std::uint32_t, std::wstring>, CompareProc> getProcList() {
		std::set<std::pair<std::uint32_t, std::wstring>, CompareProc> procList;

		auto hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		PROCESSENTRY32 e;
		e.dwSize = sizeof(e);

		if (!Process32First(hSnap, &e)) {
			CloseHandle(hSnap);
			return {};
		}

		do {
			if (!isSystemProcess(e.szExeFile)) {
				procList.insert(std::make_pair(e.th32ProcessID, e.szExeFile));
			}
		} while (Process32Next(hSnap, &e));

		CloseHandle(hSnap);
		return procList;
	}

	inline std::vector<DWORD> getProcIDs(std::wstring_view procname) {
		std::vector<DWORD> pids;
		const auto procList = getProcList();
		if (procname.empty())
			return pids;

		const auto targetName = string::toLower(std::wstring(procname));
		for (const auto& proc : procList)
		{
			if (string::toLower(proc.second) == targetName)
				pids.push_back(proc.first);
		}

		return pids;
	}

	inline DWORD getProcID(std::wstring_view procname) {
		const auto ids = getProcIDs(procname);
		return ids.empty() ? NULL : ids.front();
	}

	inline bool processHasModule(DWORD pid, std::wstring_view modname) {
		if (!pid || modname.empty())
			return false;

		const auto target = string::toLower(std::wstring(modname));
		const auto snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
		if (snap == INVALID_HANDLE_VALUE)
			return false;

		MODULEENTRY32W entry{};
		entry.dwSize = sizeof(entry);
		bool found = false;

		if (Module32FirstW(snap, &entry)) {
			do {
				if (string::toLower(entry.szModule) == target) {
					found = true;
					break;
				}
			} while (Module32NextW(snap, &entry));
		}

		CloseHandle(snap);
		return found;
	}

	struct VacHostInfo {
		DWORD pid{};
		std::wstring processName;
		std::wstring moduleName;
	};

	inline VacHostInfo findVacHost() {
		for (const auto proc : vars::vac_host_processes) {
			for (const DWORD pid : getProcIDs(proc)) {
				for (const auto mod : vars::vac_service_modules) {
					if (processHasModule(pid, mod))
						return { pid, std::wstring(proc), std::wstring(mod) };
				}
			}
		}

		return {};
	}

	inline bool openProcess(std::wstring exePath, std::vector<std::wstring> args, PROCESS_INFORMATION& pi) {
		STARTUPINFO si{};
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		std::wstring procCmdLine = exePath;
		for (auto& arg : args)
			procCmdLine += L" " + arg;

		std::vector<wchar_t> cmdLine(procCmdLine.begin(), procCmdLine.end());
		cmdLine.push_back(L'\0');

		return CreateProcess(nullptr, cmdLine.data(), nullptr, nullptr, false, NULL, nullptr,
			nullptr, &si, &pi);
	}
}
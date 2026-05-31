#include "pch.h"
#include "injector.hpp"
#include "vac3_bypass/vac3_bypass.hpp"

#include <exception>

namespace
{
	constexpr auto kProcessWait = 60s;
	constexpr auto kModuleWait = 120s;

	std::wstring toLower(std::wstring value)
	{
		std::transform(value.begin(), value.end(), value.begin(),
			[](wchar_t c) { return std::towlower(c); });
		return value;
	}

	bool enableDebugPrivilege()
	{
		HANDLE token = nullptr;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
			return false;

		TOKEN_PRIVILEGES tp{};
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &tp.Privileges[0].Luid))
		{
			CloseHandle(token);
			return false;
		}

		const BOOL ok = AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), nullptr, nullptr);
		const DWORD err = GetLastError();
		CloseHandle(token);
		return ok && err == ERROR_SUCCESS;
	}

	std::string narrow(std::wstring_view text)
	{
		if (text.empty())
			return {};

		const int size = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
		if (size <= 0)
			return {};

		std::string out(size, '\0');
		WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), out.data(), size, nullptr, nullptr);
		return out;
	}

	std::string ntStatusHex(NTSTATUS status)
	{
		char buf[16]{};
		sprintf_s(buf, "0x%08X", static_cast<unsigned long>(status));
		return buf;
	}

	void safeDetach(blackbone::Process& proc)
	{
		proc.remote().reset();
		proc.Detach();
	}

	void freeRemoteMemBlock(blackbone::call_result_t<blackbone::MemBlock>& block)
	{
		if (block.success())
			block.result().Free();
	}
}

bool Injector::attachProcess(blackbone::Process& proc, DWORD pid, std::string_view label) const
{
	const NTSTATUS status = proc.Attach(pid, DEFAULT_ACCESS_P);
	if (!NT_SUCCESS(status))
	{
		injlog::error(
			"Failed to attach to " + std::string(label) +
			" (PID " + std::to_string(pid) +
			", status " + ntStatusHex(status) +
			", Win32 " + std::to_string(GetLastError()) + ")");
		return false;
	}

	injlog::info("Attached to " + std::string(label) + " (PID " + std::to_string(pid) + ")");
	return true;
}

bool Injector::attachProcessByName(blackbone::Process& proc, std::wstring_view procname, std::string_view label, DWORD& pid) const
{
	const auto pids = mem::getProcIDs(procname);
	if (pids.empty())
	{
		injlog::error("No process instances found for " + narrow(procname));
		return false;
	}

	if (pids.size() > 1)
		injlog::info("Found " + std::to_string(pids.size()) + " " + narrow(procname) + " instances, trying attach");

	for (const DWORD candidate : pids)
	{
		safeDetach(proc);
		if (attachProcess(proc, candidate, label))
		{
			pid = candidate;
			return true;
		}
	}

	return false;
}

bool Injector::waitForProcess(std::wstring_view procname, DWORD& pid, std::chrono::seconds timeout) const
{
	injlog::info("Waiting for process: " + narrow(procname));
	const auto deadline = std::chrono::steady_clock::now() + timeout;
	while (std::chrono::steady_clock::now() < deadline)
	{
		pid = mem::getProcID(procname);
		if (pid)
		{
			injlog::info("Found process " + narrow(procname) + " (PID " + std::to_string(pid) + ")");
			return true;
		}
		std::this_thread::sleep_for(500ms);
	}

	injlog::error("Timeout waiting for process: " + narrow(procname));
	return false;
}

bool Injector::waitForModule(blackbone::Process& proc, std::wstring_view modname, std::chrono::seconds timeout) const
{
	injlog::info("Waiting for module: " + narrow(modname));
	const auto toLower = [](std::wstring str) {
		std::transform(str.begin(), str.end(), str.begin(),
			[](wchar_t c) { return std::towlower(c); });
		return str;
	};

	const auto target = toLower(std::wstring(modname));
	const auto deadline = std::chrono::steady_clock::now() + timeout;

	while (std::chrono::steady_clock::now() < deadline)
	{
		const auto mods = proc.modules().GetAllModules();
		for (const auto& mod : mods)
		{
			if (toLower(mod.first.first) == target)
			{
				injlog::info("Module ready: " + narrow(modname));
				return true;
			}
		}

		std::this_thread::sleep_for(1s);
	}

	injlog::error("Timeout waiting for module: " + narrow(modname));
	return false;
}

bool Injector::findVacHostByAttach(mem::VacHostInfo& host) const
{
	for (const auto procName : vars::vac_host_processes)
	{
		for (const DWORD pid : mem::getProcIDs(procName))
		{
			blackbone::Process proc;
			const NTSTATUS status = proc.Attach(pid, DEFAULT_ACCESS_P);
			if (!NT_SUCCESS(status))
			{
				injlog::info(
					"Skip PID " + std::to_string(pid) + " (" + narrow(procName) +
					"): attach failed " + ntStatusHex(status));
				continue;
			}

			const auto mods = proc.modules().GetAllModules();
			for (const auto modName : vars::vac_service_modules)
			{
				const auto target = toLower(std::wstring(modName));
				for (const auto& mod : mods)
				{
					if (toLower(mod.first.first) == target)
					{
						host = { pid, std::wstring(procName), std::wstring(modName) };
						safeDetach(proc);
						return true;
					}
				}
			}

			safeDetach(proc);
		}
	}

	return false;
}

bool Injector::tryLaunchSteamService() const
{
	const auto servicePath = utils::getSteamServicePath();
	if (servicePath.empty())
	{
		injlog::error("SteamService.exe not found under Steam install path");
		return false;
	}

	PROCESS_INFORMATION pi = {};
	if (!mem::openProcess(servicePath, {}, pi))
	{
		injlog::error("Failed to start SteamService.exe");
		return false;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	injlog::info("Started SteamService.exe");
	return true;
}

bool Injector::waitForVacHost(mem::VacHostInfo& host, std::chrono::seconds timeout) const
{
	injlog::info("Waiting for VAC host (SteamService + steamservice.dll)");
	const auto deadline = std::chrono::steady_clock::now() + timeout;
	auto lastProgress = std::chrono::steady_clock::now();
	bool triedLaunchService = false;

	while (std::chrono::steady_clock::now() < deadline)
	{
		if (findVacHostByAttach(host))
		{
			injlog::info(
				"VAC host ready: " + narrow(host.processName) +
				" PID " + std::to_string(host.pid) +
				" (" + narrow(host.moduleName) + " loaded)");
			return true;
		}

		const auto now = std::chrono::steady_clock::now();
		if (now - lastProgress >= 5s)
		{
			lastProgress = now;

			const auto servicePids = mem::getProcIDs(vars::str_vac_service_process);
			const auto steamPids = mem::getProcIDs(vars::str_steam_process_name);

			if (!servicePids.empty())
			{
				injlog::info(
					"SteamService.exe running (PID " + std::to_string(servicePids.front()) +
					"), waiting for steamservice.dll to load...");
			}
			else if (!steamPids.empty())
			{
				injlog::info(
					"steam.exe running (PID " + std::to_string(steamPids.front()) +
					"), waiting for SteamService.exe...");

				if (!triedLaunchService)
				{
					triedLaunchService = true;
					tryLaunchSteamService();
				}
			}
			else
			{
				injlog::info("Waiting for Steam / SteamService to start...");
			}
		}

		std::this_thread::sleep_for(500ms);
	}

	injlog::error("Timeout waiting for VAC host (SteamService with steamservice.dll)");
	return false;
}

bool Injector::bypassVAC()
{
	g_menu->isPatchingVac = true;

	if (enableDebugPrivilege())
		injlog::info("SeDebugPrivilege enabled");
	else
		injlog::info("SeDebugPrivilege not granted (some attach attempts may fail)");

	mem::VacHostInfo host{};
	if (!findVacHostByAttach(host))
	{
		injlog::info("VAC host not found, restarting Steam client");
		this->closeProcesses({
			vars::str_game_process_name.data(),
			vars::str_steam_process_name.data(),
			vars::str_vac_service_process.data(),
			L"SteamService.exe",
		});

		const auto& steamPath = utils::getSteamPath();
		if (steamPath.empty())
		{
			injlog::error("Steam path not found in registry");
			g_menu->isPatchingVac = false;
			return false;
		}

		PROCESS_INFORMATION pi = {};
		if (!mem::openProcess(steamPath, {}, pi))
		{
			injlog::error("Failed to start Steam");
			g_menu->isPatchingVac = false;
			return false;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		injlog::info("Steam process started, waiting for SteamService");
	}

	if (!waitForVacHost(host, kModuleWait))
	{
		g_menu->isPatchingVac = false;
		return false;
	}

	blackbone::Process proc;
	if (!attachProcess(proc, host.pid, narrow(host.processName)))
	{
		g_menu->isPatchingVac = false;
		return false;
	}

	if (proc.core().isWow64())
		injlog::info("VAC host is 32-bit (WOW64) - compatible with VAC inhibitor");
	else
	{
		injlog::error(
			"VAC host is 64-bit native; inhibitor requires 32-bit SteamService. "
			"Close Steam and patch again after SteamService starts.");
		safeDetach(proc);
		g_menu->isPatchingVac = false;
		return false;
	}

	if (!waitForModule(proc, host.moduleName, 10s))
	{
		safeDetach(proc);
		g_menu->isPatchingVac = false;
		return false;
	}

	injlog::info("Manual mapping VAC inhibitor into " + narrow(host.processName));
	std::vector<BYTE> vacBypassInstBuf(std::begin(vac3_data), std::end(vac3_data));

	const auto modCallback = [](blackbone::CallbackType type, void* /*context*/, blackbone::Process& /*process*/, const blackbone::ModuleData& modInfo)
	{
		if (type == blackbone::PreCallback && modInfo.name == L"user32.dll")
			return blackbone::LoadData(blackbone::MT_Native, blackbone::Ldr_Ignore);

		return blackbone::LoadData(blackbone::MT_Default, blackbone::Ldr_Ignore);
	};

	const auto result = proc.mmap().MapImage(vacBypassInstBuf.size(), vacBypassInstBuf.data(), false, blackbone::WipeHeader, modCallback);
	safeDetach(proc);

	if (!result.success())
	{
		injlog::error("VAC inhibitor ManualMap failed (status " + std::to_string(result.status) + ")");
		g_menu->isPatchingVac = false;
		return false;
	}

	g_menu->isPatchingVac = false;
	this->vacBypassed = true;
	injlog::info("VAC patch completed in " + narrow(host.processName) + " (PID " + std::to_string(host.pid) + ")");
	return true;
}

bool Injector::inject()
{
	g_menu->isInjecting = true;
	injlog::info("Inject pipeline begin");
	injlog::flush();

	std::vector<BYTE> buffer;
	if (!utils::loadEmbeddedPayload(buffer))
	{
		injlog::error("Failed to load embedded Cookie.dll from resources");
		g_menu->isInjecting = false;
		injlog::flush();
		return false;
	}

	injlog::info("Embedded payload loaded (" + std::to_string(buffer.size()) + " bytes, " + std::string(COOKIE_VERSION) + ")");
	injlog::flush();

	std::filesystem::path dllPath;
	if (!utils::writePayloadToTempFile(buffer, dllPath))
	{
		injlog::error("Failed to write payload to temp file");
		g_menu->isInjecting = false;
		injlog::flush();
		return false;
	}

	injlog::info("Payload written to " + dllPath.string());
	injlog::flush();

	const auto ok = injectLoadLibrary(dllPath);
	std::error_code ec;
	std::filesystem::remove(dllPath, ec);

	if (ok)
		injlog::info("Inject completed successfully");
	else
		injlog::error("Inject failed");

	g_menu->isInjecting = false;
	injlog::flush();
	return ok;
}

void Injector::injectThreadEntry()
{
	try
	{
		g_injector->inject();
	}
	catch (const std::exception& ex)
	{
		injlog::error(std::string("Inject thread C++ exception: ") + ex.what());
		g_menu->isInjecting = false;
		injlog::flush();
	}
	catch (...)
	{
		injlog::error("Inject thread unknown C++ exception");
		g_menu->isInjecting = false;
		injlog::flush();
	}
}

bool Injector::injectLoadLibrary(const std::filesystem::path& dllPath)
{
	DWORD pid = 0;
	if (!waitForProcess(vars::str_game_process_name.data(), pid, kProcessWait))
		return false;

	blackbone::Process proc;
	if (!attachProcessByName(proc, vars::str_game_process_name.data(), "CS2", pid))
		return false;

	if (!waitForModule(proc, vars::str_game_mod_name.data(), kModuleWait))
	{
		safeDetach(proc);
		return false;
	}

	injlog::info("LoadLibrary injecting payload");
	injlog::flush();
	const auto injected = proc.modules().Inject(dllPath.wstring());
	if (!injected.success() || !injected.result())
	{
		injlog::error("LoadLibrary inject failed (status " + std::to_string(injected.status) + ")");
		safeDetach(proc);
		injlog::flush();
		return false;
	}

	injlog::info("Payload module loaded, resolving CookieBootstrap");
	injlog::flush();
	const auto bootstrap = proc.modules().GetExport(*injected.result(), "CookieBootstrap");
	if (!bootstrap.success() || !bootstrap.result().procAddress)
	{
		injlog::error("CookieBootstrap export not found");
		safeDetach(proc);
		return false;
	}

	CookieBootstrapParam_t param{};
	param.ArgsSize = sizeof(param);
	strncpy_s(param.DllPath, dllPath.string().c_str(), _TRUNCATE);

	auto paramMem = proc.memory().Allocate(sizeof(param), PAGE_READWRITE);
	if (!paramMem.success())
	{
		injlog::error("Failed to allocate bootstrap param in target process");
		safeDetach(proc);
		return false;
	}

	if (!NT_SUCCESS(proc.memory().Write(paramMem.result().ptr(), sizeof(param), &param)))
	{
		injlog::error("Failed to write bootstrap param to target process");
		freeRemoteMemBlock(paramMem);
		safeDetach(proc);
		return false;
	}

	injlog::info("Calling CookieBootstrap in CS2");
	injlog::flush();
	const auto bootstrapResult = proc.remote().ExecDirect(bootstrap.result().procAddress, paramMem.result().ptr());
	freeRemoteMemBlock(paramMem);
	injlog::info("CookieBootstrap returned " + std::to_string(bootstrapResult));
	injlog::flush();

	if (bootstrapResult != 1)
	{
		injlog::error("CookieBootstrap failed to start initialization thread");
		safeDetach(proc);
		return false;
	}

	injlog::info("Cheat initialization running in CS2 background thread");
	safeDetach(proc);
	return true;
}

bool Injector::map(std::wstring_view procname, std::wstring_view modname, std::vector<BYTE> buffer, blackbone::eLoadFlags flags)
{
	DWORD pid = 0;
	if (!waitForProcess(procname, pid, kProcessWait))
		return false;

	blackbone::Process proc;
	if (!attachProcessByName(proc, procname, narrow(procname), pid))
		return false;

	if (!waitForModule(proc, modname, kModuleWait))
	{
		safeDetach(proc);
		return false;
	}

	const auto modCallback = [](blackbone::CallbackType type, void* context, blackbone::Process& process, const blackbone::ModuleData& modInfo)
	{
		if (type == blackbone::PreCallback && modInfo.name == L"user32.dll")
			return blackbone::LoadData(blackbone::MT_Native, blackbone::Ldr_Ignore);

		return blackbone::LoadData(blackbone::MT_Default, blackbone::Ldr_Ignore);
	};

	const auto result = proc.mmap().MapImage(buffer.size(), buffer.data(), false, flags, modCallback);
	safeDetach(proc);

	if (!result.success())
		return false;

	std::this_thread::sleep_for(1s);
	return true;
}

void Injector::closeProcesses(std::vector<std::wstring> processes)
{
	for (const auto& proc : processes)
	{
		DWORD pID = mem::getProcID(proc);
		if (pID)
		{
			injlog::info("Terminating " + narrow(proc) + " (PID " + std::to_string(pID) + ")");
			HANDLE hProc = OpenProcess(PROCESS_TERMINATE, false, pID);
			if (hProc != nullptr)
			{
				TerminateProcess(hProc, 9);
				CloseHandle(hProc);
			}
		}
	}
}

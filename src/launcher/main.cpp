#include "pch.h"

int main()
{
	FreeConsole();

	if (!injlog::init())
	{
		MessageBoxW(nullptr, L"Failed to initialize injector log file.", L"Cookie Injector", MB_ICONERROR | MB_OK);
		return 1;
	}

	injlog::installCrashHandler();
	g_logWindow->start();

	injlog::info("Cookie Injector started (" + std::string(COOKIE_VERSION) + ")");

	const auto logPath = injlog::detail::logFilePath();
	if (!logPath.empty())
	{
		const int size = WideCharToMultiByte(CP_UTF8, 0, logPath.c_str(), static_cast<int>(logPath.size()), nullptr, 0, nullptr, nullptr);
		if (size > 0)
		{
			std::string path(size, '\0');
			WideCharToMultiByte(CP_UTF8, 0, logPath.c_str(), static_cast<int>(logPath.size()), path.data(), size, nullptr, nullptr);
			injlog::info("Log file: " + path);
		}
	}

	const auto crashDir = injlog::detail::crashLogDirectory();
	if (!crashDir.empty())
	{
		const int size = WideCharToMultiByte(CP_UTF8, 0, crashDir.c_str(), static_cast<int>(crashDir.size()), nullptr, 0, nullptr, nullptr);
		if (size > 0)
		{
			std::string path(size, '\0');
			WideCharToMultiByte(CP_UTF8, 0, crashDir.c_str(), static_cast<int>(crashDir.size()), path.data(), size, nullptr, nullptr);
			injlog::info("Crash log directory: " + path);
		}
	}

	g_menu->initialize();
	injlog::info("Main window ready");
	g_menu->loop();

	injlog::info("Cookie Injector exiting");
	g_logWindow->shutdown();
	injlog::shutdown();
	return 0;
}

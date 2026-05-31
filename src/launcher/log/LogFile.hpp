#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace injlog::detail
{
	bool initFileLog();
	void shutdownFileLog();
	void flushFileLog();
	void writeFileLine(std::wstring_view line);
	void beginCrashLogFile();
	std::wstring logDirectory();
	std::wstring logFilePath();
	std::wstring crashLogPath();
	std::wstring crashLogDirectory();
}

namespace injlog
{
	void writeCrashRaw(const char* text, std::size_t length);
}

#include "pch.h"
#include "log/Log.hpp"
#include "log/LogFile.hpp"
#include "log/LogWindow.hpp"

namespace
{
	std::wstring toWide(std::string_view text)
	{
		if (text.empty())
			return {};

		const int size = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
		if (size <= 0)
			return {};

		std::wstring wide(size, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wide.data(), size);
		return wide;
	}

	const wchar_t* levelTag(injlog::Level level)
	{
		switch (level)
		{
		case injlog::Level::Warn: return L"WARN";
		case injlog::Level::Error: return L"ERROR";
		default: return L"INFO";
		}
	}

	std::wstring timestamp()
	{
		SYSTEMTIME st{};
		GetLocalTime(&st);

		wchar_t buf[32]{};
		swprintf_s(buf, L"%02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);
		return buf;
	}
}

bool injlog::init()
{
	return injlog::detail::initFileLog();
}

void injlog::shutdown()
{
	injlog::detail::shutdownFileLog();
}

void injlog::flush()
{
	injlog::detail::flushFileLog();
}

void injlog::write(Level level, std::string_view message)
{
	std::wstring line = L"[";
	line += timestamp();
	line += L"] [";
	line += levelTag(level);
	line += L"] ";
	line += toWide(message);
	line += L"\r\n";

	injlog::detail::writeFileLine(line);

	if (g_logWindow)
		g_logWindow->append(std::move(line));
}

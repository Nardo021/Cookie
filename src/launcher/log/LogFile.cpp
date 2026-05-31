#include "pch.h"
#include "log/LogFile.hpp"

#include <mutex>
#include <string>

namespace
{
	std::mutex g_fileMutex;
	HANDLE g_logFile = INVALID_HANDLE_VALUE;
	std::wstring g_logRoot;
	std::wstring g_logFilePath;
	std::wstring g_crashLogPath;

	std::string wideToUtf8(std::wstring_view text)
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

	std::wstring buildLogRoot()
	{
		wchar_t tempDir[MAX_PATH]{};
		if (!GetTempPathW(MAX_PATH, tempDir))
			return L"Cookie";

		std::wstring dir = tempDir;
		if (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/'))
			dir.pop_back();

		dir += L"\\Cookie";
		CreateDirectoryW(dir.c_str(), nullptr);
		return dir;
	}

	void ensureDirectory(const std::wstring& path)
	{
		CreateDirectoryW(path.c_str(), nullptr);
	}

	std::wstring formatTimestamp(const SYSTEMTIME& st)
	{
		wchar_t buf[32]{};
		swprintf_s(
			buf,
			L"%04u-%02u-%02u_%02u-%02u-%02u",
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond);
		return buf;
	}

	std::wstring buildDatedLogPath(const wchar_t* subFolder, const std::wstring& stamp)
	{
		const std::wstring root = g_logRoot.empty() ? buildLogRoot() : g_logRoot;
		const std::wstring folder = root + L"\\" + subFolder;
		ensureDirectory(root);
		ensureDirectory(folder);
		return folder + L"\\" + stamp + L".log";
	}

	void appendToFile(HANDLE file, std::string_view text)
	{
		if (file == INVALID_HANDLE_VALUE || text.empty())
			return;

		DWORD written = 0;
		WriteFile(file, text.data(), static_cast<DWORD>(text.size()), &written, nullptr);
		FlushFileBuffers(file);
	}
}

bool injlog::detail::initFileLog()
{
	SYSTEMTIME st{};
	GetLocalTime(&st);

	g_logRoot = buildLogRoot();
	g_logFilePath = buildDatedLogPath(L"log", formatTimestamp(st));
	g_crashLogPath.clear();

	g_logFile = CreateFileW(
		g_logFilePath.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	return g_logFile != INVALID_HANDLE_VALUE;
}

void injlog::detail::shutdownFileLog()
{
	std::lock_guard lock(g_fileMutex);
	if (g_logFile != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(g_logFile);
		CloseHandle(g_logFile);
		g_logFile = INVALID_HANDLE_VALUE;
	}
}

void injlog::detail::flushFileLog()
{
	std::lock_guard lock(g_fileMutex);
	if (g_logFile != INVALID_HANDLE_VALUE)
		FlushFileBuffers(g_logFile);
}

void injlog::detail::writeFileLine(std::wstring_view line)
{
	const auto utf8 = wideToUtf8(line);
	if (utf8.empty())
		return;

	std::lock_guard lock(g_fileMutex);
	appendToFile(g_logFile, utf8);
}

void injlog::detail::beginCrashLogFile()
{
	if (g_logRoot.empty())
		g_logRoot = buildLogRoot();

	SYSTEMTIME st{};
	GetLocalTime(&st);
	g_crashLogPath = buildDatedLogPath(L"crash", formatTimestamp(st));
}

std::wstring injlog::detail::logDirectory()
{
	return g_logRoot;
}

std::wstring injlog::detail::logFilePath()
{
	return g_logFilePath;
}

std::wstring injlog::detail::crashLogPath()
{
	return g_crashLogPath;
}

std::wstring injlog::detail::crashLogDirectory()
{
	if (g_logRoot.empty())
		g_logRoot = buildLogRoot();

	return g_logRoot + L"\\crash";
}

void injlog::writeCrashRaw(const char* text, std::size_t length)
{
	if (!text || !length)
		return;

	if (g_crashLogPath.empty())
		injlog::detail::beginCrashLogFile();

	HANDLE crashFile = CreateFileW(
		g_crashLogPath.c_str(),
		FILE_APPEND_DATA,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (crashFile == INVALID_HANDLE_VALUE)
		return;

	DWORD written = 0;
	WriteFile(crashFile, text, static_cast<DWORD>(length), &written, nullptr);
	FlushFileBuffers(crashFile);
	CloseHandle(crashFile);

	std::lock_guard lock(g_fileMutex);
	if (g_logFile != INVALID_HANDLE_VALUE)
		FlushFileBuffers(g_logFile);
}

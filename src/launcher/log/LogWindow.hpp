#pragma once

#include <atomic>
#include <string>

class LogWindow
{
public:
	bool start();
	void shutdown();
	void append(std::wstring text);

private:
	void threadMain();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void appendToEdit(const std::wstring& text);

	HWND hwnd_{ nullptr };
	HWND edit_{ nullptr };
	HANDLE thread_{ nullptr };
	DWORD threadId_{ 0 };
	std::atomic<bool> running_{ false };
};

inline auto g_logWindow = std::make_unique<LogWindow>();

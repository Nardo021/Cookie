#include "pch.h"
#include "log/LogWindow.hpp"

namespace
{
	constexpr UINT WM_LOG_APPEND = WM_APP + 1;
	constexpr int kMaxLogLines = 4000;
}

bool LogWindow::start()
{
	if (running_.exchange(true))
		return true;

	thread_ = CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
		static_cast<LogWindow*>(param)->threadMain();
		return 0;
	}, this, 0, &threadId_);

	if (!thread_)
	{
		running_ = false;
		return false;
	}

	for (int i = 0; i < 40 && !hwnd_; ++i)
		Sleep(50);

	return true;
}

void LogWindow::shutdown()
{
	if (!running_.exchange(false))
		return;

	if (threadId_)
		PostThreadMessageW(threadId_, WM_QUIT, 0, 0);

	if (thread_)
	{
		WaitForSingleObject(thread_, 3000);
		CloseHandle(thread_);
		thread_ = nullptr;
	}

	threadId_ = 0;
	hwnd_ = nullptr;
	edit_ = nullptr;
}

void LogWindow::append(std::wstring text)
{
	if (!running_ || !threadId_)
		return;

	auto* copy = new std::wstring(std::move(text));
	if (!PostThreadMessageW(threadId_, WM_LOG_APPEND, 0, reinterpret_cast<LPARAM>(copy)))
		delete copy;
}

void LogWindow::appendToEdit(const std::wstring& text)
{
	if (!edit_)
		return;

	SendMessageW(edit_, EM_SETSEL, static_cast<WPARAM>(-1), static_cast<LPARAM>(-1));
	SendMessageW(edit_, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text.c_str()));

	const int lineCount = static_cast<int>(SendMessageW(edit_, EM_GETLINECOUNT, 0, 0));
	if (lineCount > kMaxLogLines)
	{
		const int firstLineIndex = static_cast<int>(SendMessageW(edit_, EM_LINEINDEX, lineCount - kMaxLogLines, 0));
		if (firstLineIndex >= 0)
		{
			SendMessageW(edit_, EM_SETSEL, 0, firstLineIndex);
			SendMessageW(edit_, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(L""));
		}
	}

	SendMessageW(edit_, EM_SCROLLCARET, 0, 0);
}

void LogWindow::threadMain()
{
	const HINSTANCE instance = GetModuleHandleW(nullptr);

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = LogWindow::WndProc;
	wc.hInstance = instance;
	wc.lpszClassName = L"CookieInjectorLog";
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	RegisterClassExW(&wc);

	hwnd_ = CreateWindowExW(
		0,
		wc.lpszClassName,
		L"Cookie Injector Log",
		WS_OVERLAPPEDWINDOW,
		330, 100, 640, 360,
		nullptr, nullptr, instance, this);

	if (!hwnd_)
	{
		running_ = false;
		return;
	}

	edit_ = CreateWindowExW(
		WS_EX_CLIENTEDGE,
		L"EDIT",
		nullptr,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
		0, 0, 0, 0,
		hwnd_, reinterpret_cast<HMENU>(1), instance, nullptr);

	SendMessageW(edit_, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(ANSI_FIXED_FONT)), TRUE);

	RECT rc{};
	GetClientRect(hwnd_, &rc);
	SetWindowPos(edit_, nullptr, 0, 0, rc.right, rc.bottom, SWP_NOZORDER);

	ShowWindow(hwnd_, SW_SHOW);
	UpdateWindow(hwnd_);

	MSG msg{};
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (msg.message == WM_LOG_APPEND)
		{
			auto* text = reinterpret_cast<std::wstring*>(msg.lParam);
			if (text)
			{
				appendToEdit(*text);
				delete text;
			}
			continue;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	if (hwnd_)
	{
		DestroyWindow(hwnd_);
		hwnd_ = nullptr;
		edit_ = nullptr;
	}

	UnregisterClassW(wc.lpszClassName, instance);
}

LRESULT CALLBACK LogWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LogWindow* self = reinterpret_cast<LogWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

	if (msg == WM_NCCREATE)
	{
		auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
		self = static_cast<LogWindow*>(create->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
	}

	if (self)
	{
		switch (msg)
		{
		case WM_SIZE:
			if (self->edit_)
			{
				const int width = LOWORD(lParam);
				const int height = HIWORD(lParam);
				SetWindowPos(self->edit_, nullptr, 0, 0, width, height, SWP_NOZORDER);
			}
			return 0;
		case WM_CLOSE:
			ShowWindow(hwnd, SW_HIDE);
			return 0;
		case WM_DESTROY:
			return 0;
		default:
			break;
		}
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

#include "pch.h"
#include "log/CrashHandler.hpp"
#include "log/Log.hpp"
#include "log/LogFile.hpp"

#include <csignal>
#include <cstdio>
#include <cstring>

namespace
{
	const char* exceptionCodeName(DWORD code)
	{
		switch (code)
		{
		case EXCEPTION_ACCESS_VIOLATION: return "EXCEPTION_ACCESS_VIOLATION";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
		case EXCEPTION_BREAKPOINT: return "EXCEPTION_BREAKPOINT";
		case EXCEPTION_DATATYPE_MISALIGNMENT: return "EXCEPTION_DATATYPE_MISALIGNMENT";
		case EXCEPTION_FLT_DENORMAL_OPERAND: return "EXCEPTION_FLT_DENORMAL_OPERAND";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
		case EXCEPTION_FLT_INEXACT_RESULT: return "EXCEPTION_FLT_INEXACT_RESULT";
		case EXCEPTION_FLT_INVALID_OPERATION: return "EXCEPTION_FLT_INVALID_OPERATION";
		case EXCEPTION_FLT_OVERFLOW: return "EXCEPTION_FLT_OVERFLOW";
		case EXCEPTION_FLT_STACK_CHECK: return "EXCEPTION_FLT_STACK_CHECK";
		case EXCEPTION_FLT_UNDERFLOW: return "EXCEPTION_FLT_UNDERFLOW";
		case EXCEPTION_ILLEGAL_INSTRUCTION: return "EXCEPTION_ILLEGAL_INSTRUCTION";
		case EXCEPTION_IN_PAGE_ERROR: return "EXCEPTION_IN_PAGE_ERROR";
		case EXCEPTION_INT_DIVIDE_BY_ZERO: return "EXCEPTION_INT_DIVIDE_BY_ZERO";
		case EXCEPTION_INT_OVERFLOW: return "EXCEPTION_INT_OVERFLOW";
		case EXCEPTION_INVALID_DISPOSITION: return "EXCEPTION_INVALID_DISPOSITION";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
		case EXCEPTION_PRIV_INSTRUCTION: return "EXCEPTION_PRIV_INSTRUCTION";
		case EXCEPTION_SINGLE_STEP: return "EXCEPTION_SINGLE_STEP";
		case EXCEPTION_STACK_OVERFLOW: return "EXCEPTION_STACK_OVERFLOW";
		default: return "UNKNOWN_EXCEPTION";
		}
	}

	void writeCrashHeader(const char* reason)
	{
		injlog::detail::beginCrashLogFile();

		SYSTEMTIME st{};
		GetLocalTime(&st);

		char header[256]{};
		sprintf_s(
			header,
			"[%04u-%02u-%02u %02u:%02u:%02u] CRASH %s PID=%lu TID=%lu\r\n",
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond,
			reason,
			GetCurrentProcessId(),
			GetCurrentThreadId());
		injlog::writeCrashRaw(header, strlen(header));
	}

	void writeStackTrace()
	{
		void* frames[32]{};
		const USHORT count = CaptureStackBackTrace(0, 32, frames, nullptr);

		char line[128]{};
		sprintf_s(line, "Stack trace (%hu frames):\r\n", count);
		injlog::writeCrashRaw(line, strlen(line));

		for (USHORT i = 0; i < count; ++i)
		{
			sprintf_s(line, "  #%02hu 0x%p\r\n", i, frames[i]);
			injlog::writeCrashRaw(line, strlen(line));
		}
	}

	LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* info)
	{
		writeCrashHeader("UnhandledException");

		if (info && info->ExceptionRecord)
		{
			const auto* record = info->ExceptionRecord;
			char line[512]{};
			sprintf_s(
				line,
				"Code=0x%08lX (%s) Address=0x%p Flags=0x%08lX\r\n",
				record->ExceptionCode,
				exceptionCodeName(record->ExceptionCode),
				record->ExceptionAddress,
				record->ExceptionFlags);
			injlog::writeCrashRaw(line, strlen(line));

			if (record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2)
			{
				sprintf_s(
					line,
					"Access type=%llu Address=0x%p\r\n",
					static_cast<unsigned long long>(record->ExceptionInformation[0]),
					reinterpret_cast<void*>(record->ExceptionInformation[1]));
				injlog::writeCrashRaw(line, strlen(line));
			}
		}

		writeStackTrace();
		injlog::flush();
		return EXCEPTION_EXECUTE_HANDLER;
	}

	void terminateHandler()
	{
		writeCrashHeader("std::terminate");
		writeStackTrace();
		injlog::flush();
		std::abort();
	}

	void purecallHandler()
	{
		writeCrashHeader("purecall");
		writeStackTrace();
		injlog::flush();
		TerminateProcess(GetCurrentProcess(), 1);
	}

	void invalidParameterHandler(
		const wchar_t* /*expression*/,
		const wchar_t* /*function*/,
		const wchar_t* /*file*/,
		unsigned int /*line*/,
		uintptr_t /*reserved*/)
	{
		writeCrashHeader("invalid_parameter");
		writeStackTrace();
		injlog::flush();
		TerminateProcess(GetCurrentProcess(), 1);
	}

	void signalHandler(int signal)
	{
		char reason[64]{};
		sprintf_s(reason, "signal_%d", signal);
		writeCrashHeader(reason);
		writeStackTrace();
		injlog::flush();
		std::signal(signal, SIG_DFL);
		std::raise(signal);
	}
}

void injlog::installCrashHandler()
{
	SetUnhandledExceptionFilter(unhandledExceptionFilter);
	std::set_terminate(terminateHandler);
	_set_purecall_handler(purecallHandler);
	_set_invalid_parameter_handler(invalidParameterHandler);

	std::signal(SIGABRT, signalHandler);
	std::signal(SIGILL, signalHandler);
	std::signal(SIGSEGV, signalHandler);
}

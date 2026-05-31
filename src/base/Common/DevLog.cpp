#include "DevLog.hpp"

#include "Include/Config.hpp"

#if ENABLE_CONSOLE_DEBUG == 1

#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <fcntl.h>

#endif

static CDevLog g_DevLog{};

auto CDevLog::Init() -> void
{
#if ENABLE_CONSOLE_DEBUG == 1
	AllocConsole();

	SetConsoleCP( CP_UTF8 );
	SetConsoleOutputCP( CP_UTF8 );

	// Get STDOUT handle
	HANDLE ConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	int SystemOutput = _open_osfhandle( intptr_t( ConsoleOutput ) , _O_TEXT );
	COutputHandle = _fdopen( SystemOutput , XorStr( "w" ) );

	// Get STDERR handle
	HANDLE ConsoleError = GetStdHandle( STD_ERROR_HANDLE );
	int SystemError = _open_osfhandle( intptr_t( ConsoleError ) , _O_TEXT );
	CErrorHandle = _fdopen( SystemError , XorStr( "w" ) );

	// Redirect the CRT standard input, output, and error handles to the console
	freopen_s( &COutputHandle , XorStr( "CONOUT$" ) , XorStr( "w" ) , stdout );
	freopen_s( &CErrorHandle , XorStr( "CONOUT$" ) , XorStr( "w" ) , stderr );

	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
#endif

	wchar_t tempDir[MAX_PATH]{};
	if ( !GetTempPathW( MAX_PATH , tempDir ) )
	{
		hLogFile = INVALID_HANDLE_VALUE;
		return;
	}

	std::wstring dir = tempDir;
	if ( !dir.empty() && ( dir.back() == L'\\' || dir.back() == L'/' ) )
		dir.pop_back();

	dir += L"\\Cookie";
	CreateDirectoryW( dir.c_str() , nullptr );
	dir += L"\\log";
	CreateDirectoryW( dir.c_str() , nullptr );

	SYSTEMTIME st{};
	GetLocalTime( &st );

	wchar_t stamp[32]{};
	swprintf_s(
		stamp ,
		L"%04u-%02u-%02u_%02u-%02u-%02u" ,
		st.wYear ,
		st.wMonth ,
		st.wDay ,
		st.wHour ,
		st.wMinute ,
		st.wSecond );

	const std::wstring logPath = dir + L"\\dll-" + stamp + L".log";

	hLogFile = CreateFileW(
		logPath.c_str() ,
		GENERIC_WRITE ,
		FILE_SHARE_READ ,
		nullptr ,
		CREATE_ALWAYS ,
		FILE_ATTRIBUTE_NORMAL ,
		nullptr );
}

auto CDevLog::Destroy() -> void
{
#if ENABLE_CONSOLE_DEBUG == 1
	if ( CErrorHandle && COutputHandle )
	{
		fflush( CErrorHandle );
		fflush( COutputHandle );

		FreeConsole();

		fclose( CErrorHandle );
		fclose( COutputHandle );
	}
#endif

	if ( hLogFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hLogFile );
		hLogFile = INVALID_HANDLE_VALUE;
	}
}

auto CDevLog::AddLog( const char* fmt , ... ) -> void
{
	std::lock_guard lk( m_Lock );

	char buff[4096] = { 0 };

	va_list args;
	va_start( args , fmt );
	vsnprintf( buff , sizeof( buff ) - 1 , fmt , args );
	va_end( args );

#if ENABLE_CONSOLE_DEBUG == 1
	printf( XorStr( "%s" ) , buff );
#endif

	if ( hLogFile != INVALID_HANDLE_VALUE )
	{
		DWORD written = 0;
		WriteFile( hLogFile , buff , lstrlenA( buff ) , &written , nullptr );
		FlushFileBuffers( hLogFile );
	}
}

auto GetDevLog() -> CDevLog*
{
	return &g_DevLog;
}

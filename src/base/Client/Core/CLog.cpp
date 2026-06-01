#include "CLog.hpp"

#include <Common/Include/Config.hpp>

namespace CookieCore
{
	auto CLog::GetLevelPrefix( ECookieLogLevel level ) -> const char*
	{
		switch ( level )
		{
		case ECookieLogLevel::Info: return XorStr( "[info]" );
		case ECookieLogLevel::Warning: return XorStr( "[warn]" );
		case ECookieLogLevel::Error: return XorStr( "[error]" );
		case ECookieLogLevel::Success: return XorStr( "[ok]" );
		default: return XorStr( "[log]" );
		}
	}

	auto CLog::GetLevelColor( ECookieLogLevel level ) -> std::uint16_t
	{
		switch ( level )
		{
		case ECookieLogLevel::Info: return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		case ECookieLogLevel::Warning: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		case ECookieLogLevel::Error: return FOREGROUND_RED | FOREGROUND_INTENSITY;
		case ECookieLogLevel::Success: return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		default: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		}
	}

	auto CLog::SetConsoleColor( std::uint16_t color ) -> void
	{
#if ENABLE_CONSOLE_DEBUG == 1
		const auto handle = GetStdHandle( STD_OUTPUT_HANDLE );
		if ( handle != INVALID_HANDLE_VALUE )
			SetConsoleTextAttribute( handle , color );
#endif
	}

	auto CLog::ResetConsoleColor() -> void
	{
#if ENABLE_CONSOLE_DEBUG == 1
		const auto handle = GetStdHandle( STD_OUTPUT_HANDLE );
		if ( handle != INVALID_HANDLE_VALUE )
			SetConsoleTextAttribute( handle , FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#endif
	}

	auto CLog::Write( ECookieLogLevel level , const char* file , int line , const char* message ) -> void
	{
		if ( !message )
			return;

		char formatted[4096]{};
		_snprintf_s(
			formatted ,
			sizeof( formatted ) ,
			_TRUNCATE ,
			XorStr( "%s %s:%i | %s\n" ) ,
			GetLevelPrefix( level ) ,
			file ? file : XorStr( "?" ) ,
			line ,
			message );

#if ENABLE_CONSOLE_DEBUG == 1
		SetConsoleColor( GetLevelColor( level ) );
#endif

		DEV_LOG( "%s" , formatted );

#if ENABLE_CONSOLE_DEBUG == 1
		ResetConsoleColor();
#endif
	}

	CLog::Stream::Stream( ECookieLogLevel level , const char* file , int line )
		: m_Level( level )
		, m_File( file )
		, m_Line( line )
	{
	}

	CLog::Stream::~Stream()
	{
		Write( m_Level , m_File , m_Line , m_Stream.str().c_str() );
	}
}

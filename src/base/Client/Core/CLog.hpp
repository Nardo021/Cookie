#pragma once

#include <Common/Common.hpp>

#include <cstdint>
#include <sstream>
#include <string>

namespace CookieCore
{
	enum class ECookieLogLevel : std::uint8_t
	{
		Info = 0 ,
		Warning ,
		Error ,
		Success
	};

	class CLog final
	{
	public:
		class Stream final
		{
		public:
			Stream( ECookieLogLevel level , const char* file , int line );
			~Stream();

			Stream( const Stream& ) = delete;
			Stream& operator=( const Stream& ) = delete;

			template<typename T>
			Stream& operator<<( const T& value )
			{
				m_Stream << value;
				return *this;
			}

			Stream& operator<<( const char* value )
			{
				if ( value )
					m_Stream << value;

				return *this;
			}

			Stream& operator<<( char* value )
			{
				if ( value )
					m_Stream << value;

				return *this;
			}

		private:
			ECookieLogLevel m_Level = ECookieLogLevel::Info;
			const char* m_File = nullptr;
			int m_Line = 0;
			std::ostringstream m_Stream;
		};

	public:
		static auto Write( ECookieLogLevel level , const char* file , int line , const char* message ) -> void;

	private:
		static auto GetLevelPrefix( ECookieLogLevel level ) -> const char*;
		static auto GetLevelColor( ECookieLogLevel level ) -> std::uint16_t;
		static auto SetConsoleColor( std::uint16_t color ) -> void;
		static auto ResetConsoleColor() -> void;
	};

	inline auto Stream( ECookieLogLevel level , const char* file , int line ) -> CLog::Stream
	{
		return CLog::Stream( level , file , line );
	}
}

#define COOKIE_LOG( level ) \
	CookieCore::Stream( level , __FILE__ , __LINE__ )

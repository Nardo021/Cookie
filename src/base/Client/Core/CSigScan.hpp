#pragma once

#include <Common/Common.hpp>

#include <CS2/CBasePattern.hpp>

#include <cstdint>
#include <functional>
#include <vector>

namespace CookieCore
{
	class CSigScan final
	{
	public:
		using ProcedureFn = std::function<void( PVOID& )>;

	public:
		CSigScan(
			const char* name ,
			const char* pattern ,
			const char* dll ,
			std::uint32_t offset = 0 ,
			eBasePatternSearchType type = eBasePatternSearchType::SEARCH_TYPE_NONE ,
			ProcedureFn procedure = nullptr );

		CSigScan( const CSigScan& ) = delete;
		CSigScan& operator=( const CSigScan& ) = delete;

		auto FindSignature( bool skipError = false ) -> bool;
		auto GetFunction() -> PVOID;

		template<typename T>
		auto GetFunctionAs() -> T
		{
			return reinterpret_cast<T>( GetFunction() );
		}

	private:
		const char* m_Name = nullptr;
		CBasePattern m_Pattern;
		ProcedureFn m_Procedure;
	};

	class CSigScanManager final
	{
	public:
		static auto Get() -> CSigScanManager&;

		auto ScheduleScan( CSigScan* scan ) -> void;
		auto ProcessScans( bool skipError = false ) -> void;

	private:
		std::vector<CSigScan*> m_ScheduledScans{};
	};
}

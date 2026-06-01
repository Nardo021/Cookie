#include "CSigScan.hpp"

namespace CookieCore
{
	CSigScan::CSigScan(
		const char* name ,
		const char* pattern ,
		const char* dll ,
		const std::uint32_t offset ,
		const eBasePatternSearchType type ,
		ProcedureFn procedure )
		: m_Name( name )
		, m_Pattern( name , pattern , dll , offset , type )
		, m_Procedure( std::move( procedure ) )
	{
		CSigScanManager::Get().ScheduleScan( this );
	}

	auto CSigScan::FindSignature( const bool skipError ) -> bool
	{
		if ( !m_Pattern.Search( skipError ) )
		{
			if ( !skipError )
				DEV_LOG( XorStr( "[error] CSigScan: %s\n" ) , m_Name ? m_Name : XorStr( "unknown" ) );

			return false;
		}

		auto function = m_Pattern.GetFunction();
		if ( m_Procedure )
			m_Procedure( function );

		return function != nullptr;
	}

	auto CSigScan::GetFunction() -> PVOID
	{
		return m_Pattern.GetFunction();
	}

	auto CSigScanManager::Get() -> CSigScanManager&
	{
		static CSigScanManager instance{};
		return instance;
	}

	auto CSigScanManager::ScheduleScan( CSigScan* scan ) -> void
	{
		if ( scan )
			m_ScheduledScans.emplace_back( scan );
	}

	auto CSigScanManager::ProcessScans( const bool skipError ) -> void
	{
		for ( auto* scan : m_ScheduledScans )
		{
			if ( scan )
				scan->FindSignature( skipError );
		}

		m_ScheduledScans.clear();
	}
}

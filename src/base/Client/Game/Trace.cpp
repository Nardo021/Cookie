#include "Trace.hpp"

#include <cstring>
#include <cstdio>

#include "Patterns.hpp"

namespace Trace
{
	TraceShapeFn fnTraceShape = nullptr;
	InitTraceFilterFn fnInitTraceFilter = nullptr;
	void** pTraceManager = nullptr;
	bool ready = false;

	namespace
	{
		auto ResolveClientPattern( const char* pattern , int relOffset = 3 , int insnSize = 7 ) -> uintptr_t
		{
			const uintptr_t insn = Game::FindPattern( L"client.dll" , pattern );
			if ( !insn )
				return 0;

			return Game::ResolveRelativeAddress( insn , relOffset , insnSize );
		}

		auto SetupFilterManual( uint8_t* filter , uint32_t localPawnHandle , const FilterProfile& profile ) -> void
		{
			std::memset( filter , 0 , kFilterSize );
			*reinterpret_cast<uint64_t*>( filter + 0x8 ) = profile.mask;
			*reinterpret_cast<int16_t*>( filter + 0x34 ) = 7;
			*reinterpret_cast<int32_t*>( filter + 0x20 ) = static_cast<int32_t>( localPawnHandle );
			filter[0x36] = profile.collisionGroup;
		}

		auto SetupFilter(
			uint8_t* filter ,
			uintptr_t localPawn ,
			uint32_t localPawnHandle ,
			const FilterProfile& profile ) -> void
		{
			std::memset( filter , 0 , kFilterSize );

			if ( fnInitTraceFilter && localPawn )
			{
				__try
				{
					fnInitTraceFilter(
						filter ,
						reinterpret_cast<void*>( localPawn ) ,
						profile.mask ,
						profile.layer ,
						profile.uk );
					return;
				}
				__except ( EXCEPTION_EXECUTE_HANDLER )
				{
					std::memset( filter , 0 , kFilterSize );
				}
			}

			SetupFilterManual( filter , localPawnHandle , profile );
		}
	}

	auto Init() -> bool
	{
		if ( ready )
			return true;

		if ( !Game::clientBase )
			return false;

		const uintptr_t traceShapeAddr = Game::FindPattern( L"client.dll" , Patterns::sig_TraceShape );
		if ( !traceShapeAddr )
		{
			printf( "[Trace] FAIL: TraceShape pattern not found\n" );
			return false;
		}

		fnTraceShape = reinterpret_cast<TraceShapeFn>( traceShapeAddr );

		uintptr_t mgrGlobal = ResolveClientPattern( Patterns::sig_TraceManager );
		if ( !mgrGlobal )
			mgrGlobal = Game::FindRipTarget( traceShapeAddr , 0x120 );

		if ( !mgrGlobal )
		{
			printf( "[Trace] FAIL: CGameTraceManager global not found\n" );
			return false;
		}

		pTraceManager = reinterpret_cast<void**>( mgrGlobal );

		const uintptr_t initFilterAddr = Game::FindPattern( L"client.dll" , Patterns::sig_InitFilter );
		if ( initFilterAddr )
			fnInitTraceFilter = reinterpret_cast<InitTraceFilterFn>( initFilterAddr );

		void* mgr = pTraceManager ? Game::Read<void*>( reinterpret_cast<uintptr_t>( pTraceManager ) ) : nullptr;
		ready = fnTraceShape && pTraceManager && mgr;

		printf(
			"[Trace] TraceShape=0x%IX mgrGlobal=0x%IX mgr=0x%p initFilter=0x%IX ready=%d\n" ,
			traceShapeAddr ,
			mgrGlobal ,
			mgr ,
			initFilterAddr ,
			ready );

		return ready;
	}

	auto RunTrace(
		const Game::Vector3& start ,
		const Game::Vector3& end ,
		uintptr_t localPawn ,
		uint32_t localPawnHandle ,
		const FilterProfile& profile ,
		float& outFraction ,
		float* outNormalZ ,
		uintptr_t* outHitEntity ) -> bool
	{
		outFraction = 1.f;

		if ( !ready && !Init() )
			return false;

		void* traceMgr = pTraceManager ? Game::Read<void*>( reinterpret_cast<uintptr_t>( pTraceManager ) ) : nullptr;
		if ( !traceMgr || !fnTraceShape )
			return false;

		Ray_t ray{};
		alignas( 16 ) uint8_t filter[kFilterSize]{};
		alignas( 16 ) uint8_t traceResult[kTraceSize]{};

		SetupFilter( filter , localPawn , localPawnHandle , profile );

		Game::Vector3 traceStart = start;
		Game::Vector3 traceEnd = end;

		bool ok = false;
		__try
		{
			ok = fnTraceShape( traceMgr , &ray , &traceStart , &traceEnd , filter , traceResult );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return false;
		}

		if ( !ok )
			return false;

		outFraction = *reinterpret_cast<float*>( traceResult + kTraceFraction );
		if ( outNormalZ )
			*outNormalZ = *reinterpret_cast<float*>( traceResult + kTraceNormalZ );
		if ( outHitEntity )
			*outHitEntity = *reinterpret_cast<uintptr_t*>( traceResult + kTraceHitEntity );

		return true;
	}
}

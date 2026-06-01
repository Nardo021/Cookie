#include "SpreadHooks.hpp"

#include <Client/Features/Combat/NoSpread.hpp>
#include <Common/DevLog.hpp>
#include <MinHook/MinHook.h>

namespace CookieCore
{
	namespace SpreadHooks
	{
		namespace
		{
			using GetSpreadFn = float( __fastcall* )( void* weapon );
			using GetInaccuracyFn = float( __fastcall* )( void* weapon , float* unk1 , float* unk2 );

			GetSpreadFn s_getSpreadOriginal = nullptr;
			GetInaccuracyFn s_getInaccuracyOriginal = nullptr;

			bool s_hooksInstalled = false;
			bool s_zeroSpreadActive = false;

			auto __fastcall Hook_GetSpread( void* weapon ) -> float
			{
				if ( s_zeroSpreadActive || NoSpread::config.enabled )
					return 0.f;

				return s_getSpreadOriginal ? s_getSpreadOriginal( weapon ) : 0.f;
			}

			auto __fastcall Hook_GetInaccuracy( void* weapon , float* unk1 , float* unk2 ) -> float
			{
				if ( s_zeroSpreadActive || NoSpread::config.enabled )
					return 0.f;

				return s_getInaccuracyOriginal ? s_getInaccuracyOriginal( weapon , unk1 , unk2 ) : 0.f;
			}

			auto InstallDetour( PVOID target , PVOID detour , PVOID* original ) noexcept -> bool
			{
				if ( !target || !detour || !original )
					return false;

				if ( MH_CreateHook( target , detour , original ) != MH_OK )
					return false;

				return MH_EnableHook( target ) == MH_OK;
			}
		}

		auto SetZeroSpreadActive( bool active ) noexcept -> void
		{
			s_zeroSpreadActive = active;
		}

		auto ShouldZeroSpread() noexcept -> bool
		{
			return s_zeroSpreadActive || NoSpread::config.enabled;
		}

		auto Install() noexcept -> bool
		{
			if ( s_hooksInstalled )
				return IsHooked();

			if ( !IsReady() )
				return false;

			const auto spreadAddr = s_GetSpread->GetFunction();
			const auto inaccuracyAddr = s_GetInaccuracy->GetFunction();
			if ( !spreadAddr || !inaccuracyAddr )
				return false;

			const bool spreadOk = InstallDetour(
				spreadAddr ,
				reinterpret_cast<PVOID>( &Hook_GetSpread ) ,
				reinterpret_cast<PVOID*>( &s_getSpreadOriginal ) );

			const bool inaccuracyOk = InstallDetour(
				inaccuracyAddr ,
				reinterpret_cast<PVOID>( &Hook_GetInaccuracy ) ,
				reinterpret_cast<PVOID*>( &s_getInaccuracyOriginal ) );

			s_hooksInstalled = spreadOk && inaccuracyOk;

			if ( s_hooksInstalled )
				DEV_LOG( "[SpreadHooks] GetSpread/GetInaccuracy detours installed\n" );
			else
				DEV_LOG( "[warn] [SpreadHooks] Failed to install spread detours\n" );

			return s_hooksInstalled;
		}

		auto IsHooked() noexcept -> bool
		{
			return s_hooksInstalled && s_getSpreadOriginal != nullptr && s_getInaccuracyOriginal != nullptr;
		}
	}
}

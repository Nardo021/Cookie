#pragma once

#include <Common/Common.hpp>

#include <intrin.h>
#include <utility>

/*
 * Call stack spoofing stub.
 *
 * This header intentionally keeps a no-op passthrough implementation.
 * A full asm-backed version can replace SPOOF_CALL later by routing calls
 * through a gadget and preserving the real return address from _ReturnAddress().
 */

namespace CookieCore
{
	inline auto SpoofReturnAddress() -> void*
	{
		return _ReturnAddress();
	}

	template<typename Fn , typename... Args>
	inline auto InvokeSpoofed( Fn function , Args&&... args )
	{
		(void)SpoofReturnAddress();
		return function( std::forward<Args>( args )... );
	}
}

#define SPOOF_CALL( fn , ... ) \
	CookieCore::InvokeSpoofed( fn , __VA_ARGS__ )

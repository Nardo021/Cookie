#include "SDL3_Functions.hpp"

static CSDL3Functions g_CSDL3Functions{};

namespace
{
	using WarpMouseFn = int ( * )( void* , float , float );
}

auto CSDL3Functions::OnInit() -> bool
{
	auto hSDL3Module = GetModuleHandleA( XorStr( "SDL3.dll" ) );

	if ( !hSDL3Module )
	{
		DEV_LOG( "[error] SDL3.dll Module\n" );
		return false;
	}

	m_pfnWarpMouseInWindow = reinterpret_cast<void*>(
		GetProcAddress( hSDL3Module , XorStr( "SDL_WarpMouseInWindow" ) ) );

	if ( !m_pfnWarpMouseInWindow )
	{
		DEV_LOG( "[error] SDL3 SDL_WarpMouseInWindow\n" );
		return false;
	}

	return true;
}

auto CSDL3Functions::WarpMouseInWindow( void* sdlWindow , float x , float y ) -> int
{
	if ( !m_pfnWarpMouseInWindow )
		return 0;

	const auto fn = reinterpret_cast<WarpMouseFn>( m_pfnWarpMouseInWindow );
	return fn( sdlWindow , x , y );
}

auto GetSDL3Functions() -> CSDL3Functions*
{
	return &g_CSDL3Functions;
}

#pragma once

#include <Common/Common.hpp>

class CSDL3Functions final
{
public:
	auto OnInit() -> bool;

	auto WarpMouseInWindow( void* sdlWindow , float x , float y ) -> int;

private:
	void* m_pfnWarpMouseInWindow = nullptr;
};

auto GetSDL3Functions() -> CSDL3Functions*;

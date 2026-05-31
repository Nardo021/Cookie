#pragma once

#include <Common/Common.hpp>

class CTIER0Functions final
{
public:
	auto OnInit() -> bool;

public:
	using RandomFloat_t = float( __fastcall* )( float , float );
	using RandomSeed_t = void( __cdecl* )( uint32_t );

public:
	RandomFloat_t RandomFloat_o = nullptr;
	RandomSeed_t  RandomSeed_o = nullptr;
};

auto GetTIER0Functions() -> CTIER0Functions*;

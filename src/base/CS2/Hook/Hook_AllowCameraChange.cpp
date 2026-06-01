#include "Hook_AllowCameraChange.hpp"

#include <Windows.h>

#include <Client/Features/Misc/AntiAim.hpp>
#include <Client/Features/Visuals/ThirdPerson.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <GameClient/CL_Input.hpp>

namespace
{
	uintptr_t* g_pAllowCameraChangeVtableSlot = nullptr;
	bool g_hookInstalled = false;
}

auto Hook_AllowCameraChange( CCSGOInput* input , void* a2 ) -> void
{
	if ( ( AntiAim::config.enabled || ThirdPerson::config.enabled ) && input )
	{
		QAngle saved = *CCSGOInput_GetViewAngles( input , 0 );
		AllowCameraChange_o( input , a2 );
		CCSGOInput_SetViewAngles( input , 0 , saved );
		return;
	}

	AllowCameraChange_o( input , a2 );
}

auto AntiAim::InitHook() -> void
{
	if ( g_hookInstalled )
		return;

	if ( !Game::clientBase )
		Game::clientBase = Game::GetModuleBase( L"client.dll" );

	auto* input = GetCL_Input();
	if ( !input )
		return;

	auto** vtable = reinterpret_cast<uintptr_t**>( input );
	if ( !vtable || !*vtable )
		return;

	const uintptr_t originalFn = ( *vtable )[Offsets::CCSGOInput_AllowCameraChangeIdx];
	if ( !originalFn )
		return;

	const auto clientBase = Game::clientBase;
	const auto clientEnd = clientBase + 0x4000000;
	if ( originalFn < clientBase || originalFn >= clientEnd )
		return;

	g_pAllowCameraChangeVtableSlot = &( *vtable )[Offsets::CCSGOInput_AllowCameraChangeIdx];

	DWORD oldProtect = 0;
	if ( !VirtualProtect( g_pAllowCameraChangeVtableSlot , sizeof( uintptr_t ) , PAGE_READWRITE , &oldProtect ) )
		return;

	AllowCameraChange_o = reinterpret_cast<AllowCameraChange_t>( *g_pAllowCameraChangeVtableSlot );
	*g_pAllowCameraChangeVtableSlot = reinterpret_cast<uintptr_t>( &Hook_AllowCameraChange );

	VirtualProtect( g_pAllowCameraChangeVtableSlot , sizeof( uintptr_t ) , oldProtect , &oldProtect );
	g_hookInstalled = true;
}

auto AntiAim::ShutdownHook() -> void
{
	if ( !g_hookInstalled || !g_pAllowCameraChangeVtableSlot || !AllowCameraChange_o )
	{
		g_hookInstalled = false;
		g_pAllowCameraChangeVtableSlot = nullptr;
		AllowCameraChange_o = nullptr;
		return;
	}

	DWORD oldProtect = 0;
	if ( VirtualProtect( g_pAllowCameraChangeVtableSlot , sizeof( uintptr_t ) , PAGE_READWRITE , &oldProtect ) )
	{
		*g_pAllowCameraChangeVtableSlot = reinterpret_cast<uintptr_t>( AllowCameraChange_o );
		VirtualProtect( g_pAllowCameraChangeVtableSlot , sizeof( uintptr_t ) , oldProtect , &oldProtect );
	}

	g_hookInstalled = false;
	g_pAllowCameraChangeVtableSlot = nullptr;
	AllowCameraChange_o = nullptr;
}

#include "DllMain.hpp"
#include "DllLauncher.hpp"
#include "../Version.h"

BOOL WINAPI DllMain( HINSTANCE hInstace , DWORD dwReason , LPVOID lpReserved )
{
	UNREFERENCED_PARAMETER( lpReserved );

	switch ( dwReason )
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls( hInstace );
			GetDllLauncher()->OnDllAttach( hInstace );
			break;
		case DLL_PROCESS_DETACH:
			GetDllLauncher()->OnDestroy();
			break;
	}

	return TRUE;
}

extern "C" __declspec( dllexport ) const char* WINAPI CookieGetVersion()
{
	return COOKIE_VER_LABEL;
}

extern "C" __declspec( dllexport ) DWORD WINAPI CookieBootstrap( LPVOID lpParameter )
{
	GetDllLauncher()->ApplyBootstrapParam( lpParameter );
	return GetDllLauncher()->RunBootstrap();
}

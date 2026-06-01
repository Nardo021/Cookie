#include "DllLauncher.hpp"

#include <string>
#include <winternl.h>

#include <Common/CrashLog.hpp>
#include <Common/Helpers/StringHelper.hpp>

#include <CS2/CSDK_Loader.hpp>
#include <CS2/CHook_Loader.hpp>
#include <CS2/SDK/CFunctionList.hpp>

#include <Client/CCookieClient.hpp>
#include <Client/Features/Init/FeatureInit.hpp>

static CDllLauncher g_CDllLauncher{};

auto CDllLauncher::InitPaths() -> void
{
	m_DllDir = m_DllPathRaw;

	const auto lastSlash = m_DllDir.find_last_of( "\\/" );
	if ( lastSlash != std::string::npos )
	{
		const auto lastDot = m_DllDir.find_last_of( '.' );
		if ( lastDot != std::string::npos && lastDot > lastSlash )
			m_DllDir = m_DllDir.substr( 0 , lastSlash + 1 );
	}

	if ( !m_DllDir.empty() && m_DllDir.back() != '\\' && m_DllDir.back() != '/' )
		m_DllDir += '\\';

	m_CS2Dir = m_CS2PathRaw;
	const auto cs2Slash = m_CS2Dir.find_last_of( "\\/" );
	if ( cs2Slash != std::string::npos )
		m_CS2Dir = m_CS2Dir.substr( 0 , cs2Slash + 1 );

	m_SizeofImage = GetSizeOfImageInternal();
	m_BaseOfCode = GetBaseOfCodeInternal();
}

auto CDllLauncher::OnDllAttach( HINSTANCE hInstace ) -> void
{
	m_hDllImage = hInstace;
}

auto CDllLauncher::ApplyBootstrapParam( LPVOID lpParameter ) -> void
{
	if ( lpParameter )
	{
		const auto* pParam = static_cast<const ManualMapParam_t*>( lpParameter );
		if ( pParam && pParam->DllPath[0] )
			lstrcpynA( m_DllPathRaw , pParam->DllPath , MAX_PATH );
	}

	GetModuleFileNameA( nullptr , m_CS2PathRaw , MAX_PATH );
}

	auto CDllLauncher::RunBootstrap() -> DWORD
{
	const HANDLE hThread = CreateThread( nullptr , 0 , StartCheatTheard , nullptr , 0 , nullptr );
	if ( !hThread )
		return 0;

	CloseHandle( hThread );
	return 1;
}

auto CDllLauncher::OnDestroy() -> void
{
	if ( !m_bDestroyed )
	{
		GetDevLog()->Destroy();
		GetHook_Loader()->DestroyHooks();
		GetCrashLog()->DestroyVectorExceptionHandler();
		
		m_bDestroyed = true;
	}
}

auto WINAPI CDllLauncher::StartCheatTheard( LPVOID lpThreadParameter ) -> DWORD
{
	UNREFERENCED_PARAMETER( lpThreadParameter );

	Sleep( 2000 );

	GetDllLauncher()->InitPaths();

	GetDevLog()->Init();
	GetCrashLog()->InitVectorExceptionHandler();

	DEV_LOG( "[+] StartCheatThread: %s\n" , GetDllDir().c_str() );

	for ( int i = 0 ; i < 120 ; ++i )
	{
		if ( GetModuleHandleA( "client.dll" ) && GetModuleHandleA( "engine2.dll" ) )
			break;

		Sleep( 500 );
	}

	if ( !GetModuleHandleA( "client.dll" ) || !GetModuleHandleA( "engine2.dll" ) )
	{
		DEV_LOG( "[error] Game modules not loaded (client.dll / engine2.dll)\n" );
		return 0;
	}

	DEV_LOG( "[+] Game modules ready\n" );

	if ( !GetHook_Loader()->InitalizeMH() )
	{
		DEV_LOG( "[error] Hook_Loader::InitalizeMH\n" );
		return 0;
	}

	if ( !GetHook_Loader()->InstallFirstHook() )
	{
		DEV_LOG( "[error] Hook_Loader::InstallFirstHook\n" );
		return 0;
	}

	if ( !GetFunctionList()->OnInit() )
	{
		DEV_LOG( "[error] FunctionList::OnInit\n" );
		return 0;
	}

	if ( !GetSDK_Loader()->LoadSDK() )
	{
		DEV_LOG( "[error] CSDK_Loader::LoadSDK\n" );
		return 0;
	}

	GetCookieClient()->OnInit();

	if ( !GetHook_Loader()->InstallSecondHook() )
	{
		DEV_LOG( "[error] Hook_Loader::InstallSecondHook\n" );
		return 0;
	}

	FeatureInit::VerifyHooks();

	DEV_LOG( "[+] Cookie initialized\n" );

	return 1;
}

auto GetDllDir()->std::string&
{
	return GetDllLauncher()->m_DllDir;
}

auto GetCS2Dir() -> std::string
{
	return GetDllLauncher()->m_CS2Dir;
}

auto GetDllLauncher() -> CDllLauncher*
{
	return &g_CDllLauncher;
}

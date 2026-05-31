#include "CCookieClient.hpp"

#include <Windows.h>

#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>

#include <GameClient/CL_Players.hpp>

#include <Client/CCookieGUI.hpp>
#include <Client/Fonts/CFontManager.hpp>
#include <Client/Settings/CSettingsJson.hpp>
#include <Client/Features/Cookie/Aimbot.hpp>
#include <Client/Features/Cookie/AntiAim.hpp>
#include <Client/Features/Cookie/Bhop.hpp>
#include <Client/Features/Cookie/Esp.hpp>
#include <Client/Features/Cookie/SkinChanger.hpp>
#include <Client/Features/Cookie/TextureOverride.hpp>
#include <Client/Features/Cookie/Tracers.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Trace.hpp>
#include <Client/UI/CookieMenu.hpp>
#include <DllLauncher.hpp>

static CCookieClient g_CookieClient{};

namespace CookieUI
{
	ID3D11Device* pDevice = nullptr;
}

auto CCookieClient::OnInit() -> void
{
	if ( m_initialized )
		return;

	Game::clientBase = Game::GetModuleBase( L"client.dll" );
	Game::engine2Base = Game::GetModuleBase( L"engine2.dll" );
	Game::Init();

	if ( !Trace::Init() )
		DEV_LOG( "[Cookie] TraceShape init failed\n" );

	Aimbot::Init();
	SkinChanger::running.store( true );

	GetSettingsJson()->UpdateConfigList();

	m_initialized = true;
}

auto CCookieClient::OnShutdown() -> void
{
	if ( !m_initialized )
		return;

	SkinChanger::running.store( false );
	SkinChanger::ShutdownThirdPerson();
	AntiAim::ShutdownHook();
	Aimbot::Shutdown();
	CustomTexture::Shutdown();
	m_initialized = false;
}

auto CCookieClient::PollHotkeys() -> void
{
	if ( GetAsyncKeyState( VK_END ) & 1 )
	{
		OnShutdown();
		GetDllLauncher()->OnDestroy();
		FreeLibraryAndExitThread( GetDllLauncher()->GetDllImage() , 0 );
	}
}

auto CCookieClient::OnRender() -> void
{
	if ( !m_initialized )
		return;

	if ( !Game::clientBase )
		Game::clientBase = Game::GetModuleBase( L"client.dll" );
	if ( !Game::engine2Base )
		Game::engine2Base = Game::GetModuleBase( L"engine2.dll" );
	if ( !Game::clientBase )
		return;

	auto* engine = SDK::Interfaces::EngineToClient();
	if ( !engine || !engine->IsInGame() )
		return;

	if ( !GetCL_Players()->IsLocalPlayerAlive() )
		return;

	auto* gui = GetCookieGUI();
	if ( !gui || !gui->IsInited() )
		return;

	CookieUI::pDevice = gui->GetDevice();
	CustomTexture::UpdateHookState( gui->GetDeviceContext() );
	CustomTexture::HandleInput();

	if ( SkinChanger::NeedsTick() )
		SkinChanger::Tick();

	if ( gui->IsVisible() )
		CookieUI::RenderMenu();

	Game::BeginFrameRender();
	ESP::Render();
	Aimbot::RenderFOV();
	BulletTracer::Render();

	GetFontManager()->FirstInitFonts();
	GetFontManager()->m_VerdanaFont.DrawString( 1 , 1 , ImColor( 255 , 255 , 0 ) , FONT_ALIGN_LEFT , XorStr( CHEAT_NAME ) );
}

auto CCookieClient::OnClientOutput() -> void
{
	PollHotkeys();
}

auto CCookieClient::OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd ) -> void
{
	if ( !m_initialized || !cmd )
		return;

	if ( !Game::clientBase )
		Game::clientBase = Game::GetModuleBase( L"client.dll" );

	if ( AntiAim::config.enabled )
		AntiAim::InitHook();

	Aimbot::OnCreateMove( input , slot , cmd );

	if ( !Aimbot::blockAntiAim )
		AntiAim::Process( input , cmd );

	Bhop::Process( input , cmd );

	if ( !Aimbot::blockAntiAim )
		AntiAim::FixMovementPost( input , cmd );
	else
		AntiAim::CancelMovementFix();
}

auto GetCookieClient() -> CCookieClient*
{
	return &g_CookieClient;
}

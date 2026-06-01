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
#include <Client/Features/Init/FeatureInit.hpp>
#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Misc/AntiAim.hpp>
#include <Client/Features/Movement/Movement.hpp>
#include <Client/Features/Combat/EnginePred.hpp>
#include <Client/Features/Visuals/Esp.hpp>
#include <Client/Features/Visuals/EspOverlay.hpp>
#include <Client/Features/Inventory/Gloves.hpp>
#include <Client/Features/Inventory/SkinChangerInventory.hpp>
#include <Client/Features/Combat/Legit/LegitBot.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/UI/Menu/MenuEffects.hpp>
#include <Client/Features/Misc/PlantBomb.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Inventory/SkinChanger.hpp>
#include <Client/Features/Visuals/ThirdPerson.hpp>
#include <Client/Features/Inventory/TextureOverride.hpp>
#include <Client/Features/Visuals/Tracers.hpp>
#include <Client/Features/Visuals/WorldVisuals.hpp>
#include <Client/Core/CLog.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Trace.hpp>
#include <Client/UI/Menu/CookieMenu.hpp>
#include <Client/Utils/CInputSystem.hpp>
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

	FeatureInit::Init();

	Aimbot::Init();
	SkinChanger::running.store( true );

	GetSettingsJson()->UpdateConfigList();

	COOKIE_LOG( CookieCore::ECookieLogLevel::Success ) << "Cookie modules initialized";

	m_initialized = true;
}

auto CCookieClient::OnShutdown() -> void
{
	if ( !m_initialized )
		return;

	SkinChanger::running.store( false );
	ThirdPerson::Shutdown();
	Gloves::Shutdown();
	SkinChangerInventory::Shutdown();
	WorldVisuals::Shutdown();
	Movement::Shutdown();
	AntiAim::ShutdownHook();
	Aimbot::Shutdown();
	FeatureInit::Shutdown();
	CustomTexture::Shutdown();
	m_initialized = false;
}

auto CCookieClient::PollHotkeys() -> void
{
	GetInputSystem()->Update();

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

	GetInputSystem()->Update();

	Game::BeginFrameRender();
	EspOverlay::RenderAll();
	Aimbot::RenderFOV();
	LagComp::RenderDebug();
	MenuEffects::RenderWatermark();
	BulletTracer::Render();

	GetFontManager()->FirstInitFonts();
}

auto CCookieClient::OnClientOutput() -> void
{
	PollHotkeys();
}

auto CCookieClient::OnCreateMove( CCSGOInput* input , uint32_t slot , CUserCmd* cmd ) -> void
{
	if ( !m_initialized || !cmd )
		return;

	if ( auto* pawn = GetCL_Players()->GetLocalPlayerPawn(); pawn && pawn->IsAlive() )
		SkinChangerInventory::ProcessNetworkFullUpdate();

	EnginePred::Start( cmd );
	BulletTracer::OnCreateMove();

	if ( !Game::clientBase )
		Game::clientBase = Game::GetModuleBase( L"client.dll" );

	if ( AntiAim::config.enabled || ThirdPerson::config.enabled )
		AntiAim::InitHook();

	ThirdPerson::ApplyInput( input );

	if ( Ragebot::config.enabled )
		Ragebot::OnCreateMove( input , slot , cmd );
	else
		LegitBot::OnCreateMove( input , slot , cmd );

	const bool combatOwnsView = Ragebot::blockAntiAim || Aimbot::blockAntiAim;

	if ( !combatOwnsView )
		AntiAim::Process( input , cmd );

	Movement::Process( input , cmd );
	PlantBomb::Process( input , cmd );

	if ( !combatOwnsView )
		AntiAim::FixMovementPost( input , cmd );
	else
		AntiAim::CancelMovementFix();

	EnginePred::End();
}

auto GetCookieClient() -> CCookieClient*
{
	return &g_CookieClient;
}

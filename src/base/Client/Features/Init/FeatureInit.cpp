#include "FeatureInit.hpp"

#include <cstring>

#include <Client/Core/CConVars.hpp>
#include <Client/Core/CLog.hpp>
#include <Client/Core/CSpoofCall.hpp>
#include <Client/Core/CSigScan.hpp>
#include <Client/Core/SpreadHooks.hpp>
#include <Client/Settings/CSettingsJson.hpp>
#include <Client/Features/Visuals/Chams.hpp>
#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Features/Combat/Rage/RageSubTick.hpp>
#include <Client/Utils/CNotify.hpp>
#include <Common/DevLog.hpp>
#include <CS2/Hook/Hook_DrawObject.hpp>
#include <CS2/Hook/Hook_PredictionSimulation.hpp>

namespace FeatureInit
{
	namespace
	{
		bool s_hooksLogged = false;
	}

	auto Init() noexcept -> std::vector<ModuleStatus>
	{
		std::vector<ModuleStatus> status;

		auto push = [&]( const char* name , bool ok )
		{
			status.push_back( { name , ok } );
		};

		push( "ConVars" , SPOOF_CALL( CookieCore::CConVars::Setup ) );

		CookieCore::CSigScanManager::Get().ProcessScans();
		push( "SigScan" , true );
		const bool spreadSigsReady = CookieCore::SpreadHooks::IsReady();
		push( "SpreadHooks" , spreadSigsReady );
		if ( spreadSigsReady )
			push( "SpreadHookDetour" , CookieCore::SpreadHooks::Install() );

		GetSettingsJson()->UpdateConfigList();
		GetSettingsJson()->LoadConfig( "default.json" );
		push( "Config" , true );
		push( "Chams" , Chams::Init() );
		HitboxData::InitNative();
		push( "HitboxNative" , HitboxData::IsNativeReady() );
		RageSubTick::BuildSeedTable();
		push( "RageSubTick" , true );

		for ( const auto& item : status )
			COOKIE_LOG( CookieCore::ECookieLogLevel::Info ) << "[FeatureInit] " << item.name << ": " << ( item.ok ? "OK" : "MISSING" );

		return status;
	}

	auto VerifyHooks() noexcept -> void
	{
		if ( s_hooksLogged )
			return;

		s_hooksLogged = true;

		const bool drawObject = DrawObject_o != nullptr;
		const bool chamsMaterials = Chams::IsReady();
		const bool prediction = PredictionSimulation_o != nullptr;

		if ( drawObject )
		{
			DEV_LOG( "[FeatureInit] DrawObjectHook: OK (scenesystem.dll)\n" );
		}
		else
		{
			DEV_LOG(
				"[warn] [Chams] DrawObjectHook: MISSING — signature in scenesystem.dll outdated or hook skipped. "
				"Chams will not render; all other features continue normally.\n" );
		}

		if ( chamsMaterials )
		{
			DEV_LOG( "[FeatureInit] Chams materials: OK\n" );
		}
		else
		{
			DEV_LOG( "[warn] [Chams] Material init failed — Chams disabled even if DrawObject hook is present.\n" );
		}

		DEV_LOG( "[FeatureInit] PredictionHook: %s\n" , prediction ? "OK" : "MISSING" );
		DEV_LOG( "[FeatureInit] SpreadHooks: %s (detour: %s)\n" ,
			CookieCore::SpreadHooks::IsReady() ? "OK" : "MISSING" ,
			CookieCore::SpreadHooks::IsHooked() ? "OK" : "MISSING" );
		DEV_LOG( "[FeatureInit] InputParser: SerializePartial hook active\n" );

		if ( drawObject && chamsMaterials )
		{
			DEV_LOG( "[FeatureInit] Chams: ready\n" );
			if ( prediction )
				GetNotify()->Push( N_TYPE_SUCCESS , "Cookie hooks ready" );
		}
		else if ( !drawObject )
		{
			GetNotify()->Push( N_TYPE_WARNING , "DrawObject hook missing (Chams disabled)" );
		}
		else if ( !chamsMaterials )
		{
			GetNotify()->Push( N_TYPE_WARNING , "Chams materials missing (Chams disabled)" );
		}
	}

	auto Shutdown() noexcept -> void
	{
		Chams::Shutdown();
	}

	auto AllReady() noexcept -> bool
	{
		return Chams::IsReady() && DrawObject_o != nullptr;
	}
}

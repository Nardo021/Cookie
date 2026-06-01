#include "MenuConfig.hpp"

#include <mutex>
#include <algorithm>

#include <Client/Features/Misc/AntiAim.hpp>
#include <Client/Features/Movement/Bhop.hpp>
#include <Client/Features/Visuals/Chams.hpp>
#include <Client/Features/Visuals/Esp.hpp>
#include <Client/Features/Visuals/EspOverlay.hpp>
#include <Client/Features/Inventory/Gloves.hpp>
#include <Client/Features/Inventory/TextureOverride.hpp>
#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/Features/Combat/Legit/LegitBot.hpp>
#include <Client/UI/Menu/MenuEffects.hpp>
#include <Client/UI/Menu/MenuSettings.hpp>
#include <Client/Features/Movement/Movement.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Features/Misc/PlantBomb.hpp>
#include <Client/Features/Combat/Rage/Ragebot.hpp>
#include <Client/Features/Inventory/SkinChanger.hpp>
#include <Client/Features/Visuals/ThirdPerson.hpp>
#include <Client/Features/Visuals/Tracers.hpp>
#include <Client/Features/Combat/Legit/Triggerbot.hpp>
#include <Client/Features/Combat/Legit/WeaponConfig.hpp>
#include <Client/Features/Visuals/WorldFov.hpp>
#include <Client/Features/Visuals/WorldVisuals.hpp>
#include <Client/Settings/ConfigJson.hpp>
#include <CS2/Hook/Hook_SetViewModelFov.hpp>

namespace MenuConfig
{
	namespace
	{
		using namespace ConfigJson;

		auto WriteLegitWeaponSettings(
			rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer ,
			const char* name ,
			const WeaponConfig::LegitSettings& settings ) -> void
		{
			writer.String( name );
			writer.StartObject();
			writer.Key( "fov" ); writer.Int( settings.fov );
			writer.Key( "aimKey" ); writer.Int( settings.aimKey );
			writer.Key( "smooth" ); writer.Int( settings.smooth );
			writer.Key( "targetSelection" ); writer.Int( settings.targetSelection );
			writer.Key( "rcs" ); writer.Bool( settings.rcs );
			writer.Key( "rcsShotsEnable" ); writer.Bool( settings.rcsShotsEnable );
			writer.Key( "rcsShots" ); writer.Int( settings.rcsShots );
			writer.Key( "rcsSmoothX" ); writer.Double( settings.rcsSmoothX );
			writer.Key( "rcsSmoothY" ); writer.Double( settings.rcsSmoothY );
			writer.Key( "punchRandomization" ); writer.Bool( settings.punchRandomization );
			writer.Key( "punchRandomX" ); writer.Double( settings.punchRandomX );
			writer.Key( "punchRandomY" ); writer.Double( settings.punchRandomY );
			writer.Key( "visCheck" ); writer.Bool( settings.visCheck );
			writer.Key( "hitboxHead" ); writer.Bool( settings.hitboxHead );
			writer.Key( "hitboxNeck" ); writer.Bool( settings.hitboxNeck );
			writer.Key( "hitboxChest" ); writer.Bool( settings.hitboxChest );
			writer.Key( "hitboxUpperChest" ); writer.Bool( settings.hitboxUpperChest );
			writer.Key( "hitboxStomach" ); writer.Bool( settings.hitboxStomach );
			writer.Key( "hitboxLegs" ); writer.Bool( settings.hitboxLegs );
			writer.Key( "hitboxPelvis" ); writer.Bool( settings.hitboxPelvis );
			writer.Key( "hitboxArms" ); writer.Bool( settings.hitboxArms );
			writer.Key( "triggerEnabled" ); writer.Bool( settings.triggerEnabled );
			writer.Key( "triggerHitchance" ); writer.Int( settings.triggerHitchance );
			writer.Key( "removeSpread" ); writer.Bool( settings.removeSpread );
			writer.EndObject();
		}

		auto ReadLegitWeaponSettings( const rapidjson::Value& obj , const char* name , WeaponConfig::LegitSettings& settings ) -> void
		{
			if ( !obj.HasMember( name ) || !obj[name].IsObject() )
				return;

			const auto& v = obj[name];
			ReadInt( v , "fov" , settings.fov );
			ReadInt( v , "aimKey" , settings.aimKey );
			ReadInt( v , "smooth" , settings.smooth );
			ReadInt( v , "targetSelection" , settings.targetSelection );
			ReadBool( v , "rcs" , settings.rcs );
			ReadBool( v , "rcsShotsEnable" , settings.rcsShotsEnable );
			ReadInt( v , "rcsShots" , settings.rcsShots );
			ReadFloat( v , "rcsSmoothX" , settings.rcsSmoothX );
			ReadFloat( v , "rcsSmoothY" , settings.rcsSmoothY );
			ReadBool( v , "punchRandomization" , settings.punchRandomization );
			ReadFloat( v , "punchRandomX" , settings.punchRandomX );
			ReadFloat( v , "punchRandomY" , settings.punchRandomY );
			ReadBool( v , "visCheck" , settings.visCheck );
			ReadBool( v , "hitboxHead" , settings.hitboxHead );
			ReadBool( v , "hitboxNeck" , settings.hitboxNeck );
			ReadBool( v , "hitboxChest" , settings.hitboxChest );
			ReadBool( v , "hitboxUpperChest" , settings.hitboxUpperChest );
			ReadBool( v , "hitboxStomach" , settings.hitboxStomach );
			ReadBool( v , "hitboxLegs" , settings.hitboxLegs );
			ReadBool( v , "hitboxPelvis" , settings.hitboxPelvis );
			ReadBool( v , "hitboxArms" , settings.hitboxArms );
			ReadBool( v , "triggerEnabled" , settings.triggerEnabled );
			ReadInt( v , "triggerHitchance" , settings.triggerHitchance );
			ReadBool( v , "removeSpread" , settings.removeSpread );
		}

		auto WriteRageWeaponSettings(
			rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer ,
			const char* name ,
			const WeaponConfig::RageSettings& settings ) -> void
		{
			writer.String( name );
			writer.StartObject();
			writer.Key( "minDamage" ); writer.Int( settings.minDamage );
			writer.Key( "hitchance" ); writer.Int( settings.hitchance );
			writer.Key( "targetSelect" ); writer.Int( settings.targetSelect );
			writer.Key( "rapidFire" ); writer.Bool( settings.rapidFire );
			writer.Key( "autoScope" ); writer.Bool( settings.autoScope );
			writer.Key( "penetration" ); writer.Bool( settings.penetration );
			writer.Key( "autoStop" ); writer.Bool( settings.autoStop );
			writer.Key( "safePoint" ); writer.Bool( settings.safePoint );
			writer.Key( "earlyAutoStop" ); writer.Bool( settings.earlyAutoStop );
			writer.Key( "hitboxHead" ); writer.Bool( settings.hitboxHead );
			writer.Key( "hitboxNeck" ); writer.Bool( settings.hitboxNeck );
			writer.Key( "hitboxChest" ); writer.Bool( settings.hitboxChest );
			writer.Key( "hitboxUpperChest" ); writer.Bool( settings.hitboxUpperChest );
			writer.Key( "hitboxPelvis" ); writer.Bool( settings.hitboxPelvis );
			writer.Key( "hitboxStomach" ); writer.Bool( settings.hitboxStomach );
			writer.Key( "hitboxArms" ); writer.Bool( settings.hitboxArms );
			writer.Key( "hitboxLegs" ); writer.Bool( settings.hitboxLegs );
			writer.Key( "hitboxFeet" ); writer.Bool( settings.hitboxFeet );
			writer.Key( "removeSpread" ); writer.Bool( settings.removeSpread );
			writer.EndObject();
		}

		auto ReadRageWeaponSettings( const rapidjson::Value& obj , const char* name , WeaponConfig::RageSettings& settings ) -> void
		{
			if ( !obj.HasMember( name ) || !obj[name].IsObject() )
				return;

			const auto& v = obj[name];
			ReadInt( v , "minDamage" , settings.minDamage );
			ReadInt( v , "hitchance" , settings.hitchance );
			ReadInt( v , "targetSelect" , settings.targetSelect );
			ReadBool( v , "rapidFire" , settings.rapidFire );
			ReadBool( v , "autoScope" , settings.autoScope );
			ReadBool( v , "penetration" , settings.penetration );
			ReadBool( v , "autoStop" , settings.autoStop );
			ReadBool( v , "safePoint" , settings.safePoint );
			ReadBool( v , "earlyAutoStop" , settings.earlyAutoStop );
			ReadBool( v , "hitboxHead" , settings.hitboxHead );
			ReadBool( v , "hitboxNeck" , settings.hitboxNeck );
			ReadBool( v , "hitboxChest" , settings.hitboxChest );
			ReadBool( v , "hitboxUpperChest" , settings.hitboxUpperChest );
			ReadBool( v , "hitboxPelvis" , settings.hitboxPelvis );
			ReadBool( v , "hitboxStomach" , settings.hitboxStomach );
			ReadBool( v , "hitboxArms" , settings.hitboxArms );
			ReadBool( v , "hitboxLegs" , settings.hitboxLegs );
			ReadBool( v , "hitboxFeet" , settings.hitboxFeet );
			ReadBool( v , "removeSpread" , settings.removeSpread );
		}

		auto WriteWeaponConfig( rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer ) -> void
		{
			writer.String( "WeaponConfig" );
			writer.StartObject();
			WriteLegitWeaponSettings( writer , "legitDefault" , WeaponConfig::s_legitDefault );
			WriteLegitWeaponSettings( writer , "legitPistol" , WeaponConfig::s_legitPistol );
			WriteLegitWeaponSettings( writer , "legitHeavyPistol" , WeaponConfig::s_legitHeavyPistol );
			WriteLegitWeaponSettings( writer , "legitAssaultRifle" , WeaponConfig::s_legitAssaultRifle );
			WriteLegitWeaponSettings( writer , "legitAuto" , WeaponConfig::s_legitAuto );
			WriteLegitWeaponSettings( writer , "legitScout" , WeaponConfig::s_legitScout );
			WriteLegitWeaponSettings( writer , "legitAwp" , WeaponConfig::s_legitAwp );
			WriteRageWeaponSettings( writer , "rageDefault" , WeaponConfig::s_rageDefault );
			WriteRageWeaponSettings( writer , "ragePistol" , WeaponConfig::s_ragePistol );
			WriteRageWeaponSettings( writer , "rageHeavyPistol" , WeaponConfig::s_rageHeavyPistol );
			WriteRageWeaponSettings( writer , "rageAssaultRifle" , WeaponConfig::s_rageAssaultRifle );
			WriteRageWeaponSettings( writer , "rageAuto" , WeaponConfig::s_rageAuto );
			WriteRageWeaponSettings( writer , "rageScout" , WeaponConfig::s_rageScout );
			WriteRageWeaponSettings( writer , "rageAwp" , WeaponConfig::s_rageAwp );
			writer.EndObject();
		}

		auto ReadWeaponConfig( const rapidjson::Value& root , int schemaVersion ) -> void
		{
			if ( !root.HasMember( "WeaponConfig" ) || !root["WeaponConfig"].IsObject() )
				return;

			const auto& v = root["WeaponConfig"];

			ReadLegitWeaponSettings( v , "legitDefault" , WeaponConfig::s_legitDefault );
			ReadLegitWeaponSettings( v , "legitPistol" , WeaponConfig::s_legitPistol );
			ReadLegitWeaponSettings( v , "legitHeavyPistol" , WeaponConfig::s_legitHeavyPistol );
			ReadLegitWeaponSettings( v , "legitAssaultRifle" , WeaponConfig::s_legitAssaultRifle );
			ReadLegitWeaponSettings( v , "legitAuto" , WeaponConfig::s_legitAuto );
			ReadLegitWeaponSettings( v , "legitScout" , WeaponConfig::s_legitScout );
			ReadLegitWeaponSettings( v , "legitAwp" , WeaponConfig::s_legitAwp );
			ReadRageWeaponSettings( v , "rageDefault" , WeaponConfig::s_rageDefault );
			ReadRageWeaponSettings( v , "ragePistol" , WeaponConfig::s_ragePistol );
			ReadRageWeaponSettings( v , "rageHeavyPistol" , WeaponConfig::s_rageHeavyPistol );
			ReadRageWeaponSettings( v , "rageAssaultRifle" , WeaponConfig::s_rageAssaultRifle );
			ReadRageWeaponSettings( v , "rageAuto" , WeaponConfig::s_rageAuto );
			ReadRageWeaponSettings( v , "rageScout" , WeaponConfig::s_rageScout );
			ReadRageWeaponSettings( v , "rageAwp" , WeaponConfig::s_rageAwp );

			if ( schemaVersion >= kSchemaVersion )
				return;

			ReadLegitWeaponSettings( v , "legitMachineGun" , WeaponConfig::s_legitAuto );
			ReadLegitWeaponSettings( v , "legitSniper" , WeaponConfig::s_legitScout );
			if ( !v.HasMember( "legitAwp" ) )
				WeaponConfig::s_legitAwp = WeaponConfig::s_legitScout;

			ReadRageWeaponSettings( v , "rageMachineGun" , WeaponConfig::s_rageAuto );
			ReadRageWeaponSettings( v , "rageSniper" , WeaponConfig::s_rageScout );
			if ( !v.HasMember( "rageAwp" ) )
				WeaponConfig::s_rageAwp = WeaponConfig::s_rageScout;
		}
	}

	auto WriteSettings( rapidjson::PrettyWriter<rapidjson::OStreamWrapper>& writer ) -> void
	{
		writer.String( "Ragebot" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( Ragebot::config.enabled );
		writer.Key( "minDamage" ); writer.Int( Ragebot::config.minDamage );
		writer.Key( "hitchance" ); writer.Int( Ragebot::config.hitchance );
		writer.Key( "multipointScale" ); writer.Int( Ragebot::config.multipointScale );
		writer.Key( "autoStop" ); writer.Bool( Ragebot::config.autoStop );
		writer.Key( "earlyAutoStop" ); writer.Bool( Ragebot::config.earlyAutoStop );
		writer.Key( "penetration" ); writer.Bool( Ragebot::config.penetration );
		writer.Key( "safePoint" ); writer.Bool( Ragebot::config.safePoint );
		writer.Key( "adaptiveWeapon" ); writer.Bool( Ragebot::config.adaptiveWeapon );
		writer.Key( "autoShoot" ); writer.Bool( Ragebot::config.autoShoot );
		writer.Key( "silentAim" ); writer.Bool( Ragebot::config.silentAim );
		writer.Key( "backtrack" ); writer.Bool( Ragebot::config.backtrack );
		writer.Key( "teamCheck" ); writer.Bool( Ragebot::config.teamCheck );
		writer.Key( "hitscanMode" ); writer.Int( static_cast<int>( Ragebot::config.hitscanMode ) );
		writer.Key( "scanMode" ); writer.Int( static_cast<int>( Ragebot::config.scanMode ) );
		writer.Key( "stopMode" ); writer.Int( static_cast<int>( Ragebot::config.stopMode ) );
		writer.Key( "targetSelect" ); writer.Int( static_cast<int>( Ragebot::config.targetSelect ) );
		writer.Key( "hitboxHead" ); writer.Bool( Ragebot::config.hitboxHead );
		writer.Key( "hitboxNeck" ); writer.Bool( Ragebot::config.hitboxNeck );
		writer.Key( "hitboxChest" ); writer.Bool( Ragebot::config.hitboxChest );
		writer.Key( "hitboxPelvis" ); writer.Bool( Ragebot::config.hitboxPelvis );
		writer.Key( "hitboxStomach" ); writer.Bool( Ragebot::config.hitboxStomach );
		writer.Key( "hitboxArms" ); writer.Bool( Ragebot::config.hitboxArms );
		writer.Key( "hitboxLegs" ); writer.Bool( Ragebot::config.hitboxLegs );
		writer.Key( "hitboxUpperChest" ); writer.Bool( Ragebot::config.hitboxUpperChest );
		writer.Key( "hitboxFeet" ); writer.Bool( Ragebot::config.hitboxFeet );
		writer.Key( "rapidFire" ); writer.Bool( Ragebot::config.rapidFire );
		writer.Key( "autoScope" ); writer.Bool( Ragebot::config.autoScope );
		writer.Key( "delayAim" ); writer.Bool( Ragebot::config.delayAim );
		writer.Key( "delayAimMs" ); writer.Int( Ragebot::config.delayAimMs );
		writer.Key( "usePerWeapon" ); writer.Bool( Ragebot::config.usePerWeapon );
		writer.Key( "perWeaponClass" ); writer.Int( Ragebot::config.perWeaponClass );
		writer.EndObject();

		writer.String( "LegitBot" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( LegitBot::config.enabled );
		writer.Key( "usePerWeapon" ); writer.Bool( LegitBot::config.usePerWeapon );
		writer.Key( "perWeaponClass" ); writer.Int( LegitBot::config.perWeaponClass );
		writer.Key( "targetSelection" ); writer.Int( LegitBot::config.targetSelection );
		writer.Key( "noScope" ); writer.Bool( LegitBot::config.noScope );
		writer.Key( "delayAim" ); writer.Bool( LegitBot::config.delayAim );
		writer.Key( "delayAimMs" ); writer.Int( LegitBot::config.delayAimMs );
		writer.Key( "conditions" ); writer.Uint( LegitBot::config.conditions );
		writer.Key( "triggerOverride" ); writer.Bool( LegitBot::config.triggerOverride );
		writer.Key( "triggerHitchance" ); writer.Int( LegitBot::config.triggerHitchance );
		writer.Key( "removeSpread" ); writer.Bool( LegitBot::config.removeSpread );
		writer.Key( "fovVisualize" ); writer.Bool( LegitBot::config.fovVisualize );
		ConfigJson::WriteColor( writer , "fovVisualizeColor" , LegitBot::config.fovVisualizeColor );
		writer.Key( "hitboxHead" ); writer.Bool( LegitBot::config.hitboxHead );
		writer.Key( "hitboxNeck" ); writer.Bool( LegitBot::config.hitboxNeck );
		writer.Key( "hitboxChest" ); writer.Bool( LegitBot::config.hitboxChest );
		writer.Key( "hitboxUpperChest" ); writer.Bool( LegitBot::config.hitboxUpperChest );
		writer.Key( "hitboxPelvis" ); writer.Bool( LegitBot::config.hitboxPelvis );
		writer.Key( "hitboxStomach" ); writer.Bool( LegitBot::config.hitboxStomach );
		writer.Key( "hitboxLegs" ); writer.Bool( LegitBot::config.hitboxLegs );
		writer.Key( "hitboxArms" ); writer.Bool( LegitBot::config.hitboxArms );
		writer.EndObject();

		WriteWeaponConfig( writer );

		writer.String( "Aimbot" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( Aimbot::config.enabled );
		writer.Key( "fovType" ); writer.Int( Aimbot::config.fovType );
		writer.Key( "fov" ); writer.Double( Aimbot::config.fov );
		writer.Key( "screenFov" ); writer.Double( Aimbot::config.screenFov );
		writer.Key( "targetHitbox" ); writer.Int( Aimbot::config.targetHitbox );
		writer.Key( "aimKey" ); writer.Int( Aimbot::config.aimKey );
		writer.Key( "autoShoot" ); writer.Bool( Aimbot::config.autoShoot );
		writer.Key( "silentAim" ); writer.Bool( Aimbot::config.silentAim );
		writer.Key( "teamCheck" ); writer.Bool( Aimbot::config.teamCheck );
		writer.Key( "visCheck" ); writer.Bool( Aimbot::config.visCheck );
		writer.Key( "penetration" ); writer.Bool( Aimbot::config.penetration );
		writer.Key( "recoilControl" ); writer.Bool( Aimbot::config.recoilControl );
		writer.Key( "smooth" ); writer.Int( Aimbot::config.smooth );
		writer.Key( "punchRandomization" ); writer.Bool( Aimbot::config.punchRandomization );
		writer.Key( "punchRandomX" ); writer.Double( Aimbot::config.punchRandomX );
		writer.Key( "punchRandomY" ); writer.Double( Aimbot::config.punchRandomY );
		writer.Key( "targetSelection" ); writer.Int( Aimbot::config.targetSelection );
		writer.Key( "hitboxHead" ); writer.Bool( Aimbot::config.hitboxHead );
		writer.Key( "hitboxNeck" ); writer.Bool( Aimbot::config.hitboxNeck );
		writer.Key( "hitboxChest" ); writer.Bool( Aimbot::config.hitboxChest );
		writer.Key( "hitboxUpperChest" ); writer.Bool( Aimbot::config.hitboxUpperChest );
		writer.Key( "hitboxPelvis" ); writer.Bool( Aimbot::config.hitboxPelvis );
		writer.Key( "hitboxStomach" ); writer.Bool( Aimbot::config.hitboxStomach );
		writer.Key( "hitboxLegs" ); writer.Bool( Aimbot::config.hitboxLegs );
		writer.Key( "hitboxArms" ); writer.Bool( Aimbot::config.hitboxArms );
		writer.Key( "rcsShotsEnable" ); writer.Bool( Aimbot::config.rcsShotsEnable );
		writer.Key( "rcsShots" ); writer.Int( Aimbot::config.rcsShots );
		writer.Key( "rcsSmoothX" ); writer.Double( Aimbot::config.rcsSmoothX );
		writer.Key( "rcsSmoothY" ); writer.Double( Aimbot::config.rcsSmoothY );
		writer.Key( "fovVisualize" ); writer.Bool( Aimbot::config.fovVisualize );
		ConfigJson::WriteColor( writer , "fovVisualizeColor" , Aimbot::config.fovVisualizeColor );
		writer.EndObject();

		writer.String( "NoSpread" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( NoSpread::config.enabled );
		writer.EndObject();

		writer.String( "Triggerbot" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( Triggerbot::config.enabled );
		writer.Key( "key" ); writer.Int( Triggerbot::config.key );
		writer.Key( "useKey" ); writer.Bool( Triggerbot::config.useKey );
		writer.Key( "teamCheck" ); writer.Bool( Triggerbot::config.teamCheck );
		writer.Key( "visCheck" ); writer.Bool( Triggerbot::config.visCheck );
		writer.Key( "hitchance" ); writer.Int( Triggerbot::config.hitchance );
		writer.Key( "delayMs" ); writer.Int( Triggerbot::config.delayMs );
		writer.EndObject();

		writer.String( "ESP" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( ESP::config.enabled );
		writer.Key( "bBox" ); writer.Bool( ESP::config.bBox );
		writer.Key( "boxMode" ); writer.Int( ESP::config.boxMode );
		writer.Key( "bSkeleton" ); writer.Bool( ESP::config.bSkeleton );
		writer.Key( "bHealthBar" ); writer.Bool( ESP::config.bHealthBar );
		writer.Key( "bName" ); writer.Bool( ESP::config.bName );
		writer.Key( "bDistance" ); writer.Bool( ESP::config.bDistance );
		writer.Key( "teamCheck" ); writer.Bool( ESP::config.teamCheck );
		writer.Key( "maxDistance" ); writer.Double( ESP::config.maxDistance );
		writer.Key( "bBombTimer" ); writer.Bool( ESP::config.bBombTimer );
		writer.Key( "bGlow" ); writer.Bool( ESP::config.bGlow );
		writer.Key( "bSpectators" ); writer.Bool( ESP::config.bSpectators );
		ConfigJson::WriteColor( writer , "boxColor" , ESP::config.boxColor );
		ConfigJson::WriteColor( writer , "skeletonColor" , ESP::config.skeletonColor );
		ConfigJson::WriteColor( writer , "glowColor" , ESP::config.glowColor );
		writer.EndObject();

		writer.String( "EspOverlay" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( EspOverlay::config.enabled );
		writer.Key( "useComponents" ); writer.Bool( EspOverlay::config.useComponents );
		writer.Key( "showBox" ); writer.Bool( EspOverlay::config.showBox );
		writer.Key( "showHealthBar" ); writer.Bool( EspOverlay::config.showHealthBar );
		writer.Key( "showName" ); writer.Bool( EspOverlay::config.showName );
		writer.Key( "showDistance" ); writer.Bool( EspOverlay::config.showDistance );
		writer.Key( "showWeaponIcon" ); writer.Bool( EspOverlay::config.showWeaponIcon );
		writer.Key( "showAmmoBar" ); writer.Bool( EspOverlay::config.showAmmoBar );
		writer.Key( "showHelmetFlag" ); writer.Bool( EspOverlay::config.showHelmetFlag );
		writer.Key( "showKitFlag" ); writer.Bool( EspOverlay::config.showKitFlag );
		ConfigJson::WriteColor( writer , "boxColor" , EspOverlay::config.boxColor );
		ConfigJson::WriteColor( writer , "healthBarColor" , EspOverlay::config.healthBarColor );
		ConfigJson::WriteColor( writer , "ammoBarColor" , EspOverlay::config.ammoBarColor );
		ConfigJson::WriteColor( writer , "textColor" , EspOverlay::config.textColor );
		writer.Key( "boxThickness" ); writer.Double( EspOverlay::config.boxThickness );
		writer.Key( "barThickness" ); writer.Double( EspOverlay::config.barThickness );
		writer.EndObject();

		writer.String( "Chams" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( Chams::config.enabled );
		writer.Key( "enemy" ); writer.Bool( Chams::config.enemy );
		writer.Key( "local" ); writer.Bool( Chams::config.local );
		writer.Key( "teammate" ); writer.Bool( Chams::config.teammate );
		writer.Key( "weapon" ); writer.Bool( Chams::config.weapon );
		writer.Key( "hands" ); writer.Bool( Chams::config.hands );
		writer.Key( "materialStyle" ); writer.Int( static_cast<int>( Chams::config.materialStyle ) );
		writer.Key( "ignoreZ" ); writer.Bool( Chams::config.ignoreZ );
		ConfigJson::WriteColor( writer , "color" , Chams::config.color );
		ConfigJson::WriteColor( writer , "ignoreZColor" , Chams::config.ignoreZColor );
		writer.EndObject();

		writer.String( "Movement" );
		writer.StartObject();
		writer.Key( "movementFix" ); writer.Bool( Movement::config.movementFix );
		writer.Key( "movementCorrection" ); writer.Bool( Movement::config.movementCorrection );
		writer.Key( "validateAngles" ); writer.Bool( Movement::config.validateAngles );
		writer.Key( "edgeBug" ); writer.Bool( Movement::config.edgeBug );
		writer.Key( "edgeBugUseKey" ); writer.Bool( Movement::config.edgeBugUseKey );
		writer.Key( "edgeBugKey" ); writer.Int( Movement::config.edgeBugKey );
		writer.Key( "strafeSmooth" ); writer.Double( Movement::config.strafeSmooth );
		writer.Key( "strafeAssist" ); writer.Bool( Movement::config.strafeAssist );
		writer.Key( "strafeMode" ); writer.Int( static_cast<int>( Movement::config.strafeMode ) );
		writer.EndObject();

		writer.String( "Bhop" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( Bhop::config.enabled );
		writer.Key( "edgeJump" ); writer.Bool( Bhop::config.edgeJump );
		writer.Key( "jumpBug" ); writer.Bool( Bhop::config.jumpBug );
		writer.Key( "requireSpace" ); writer.Bool( Bhop::config.requireSpace );
		writer.Key( "autoForward" ); writer.Bool( Bhop::config.autoForward );
		writer.Key( "hopChance" ); writer.Int( Bhop::config.hopChance );
		writer.EndObject();

		writer.String( "WorldVisuals" );
		writer.StartObject();
		writer.Key( "nightMode" ); writer.Bool( WorldVisuals::config.nightMode );
		writer.Key( "nightAmbient" ); writer.Double( WorldVisuals::config.nightAmbient );
		writer.Key( "worldModulate" ); writer.Bool( WorldVisuals::config.worldModulate );
		ConfigJson::WriteColor( writer , "modulateColor" , WorldVisuals::config.modulateColor );
		writer.Key( "noShadow" ); writer.Bool( WorldVisuals::config.noShadow );
		writer.EndObject();

		writer.String( "HitboxData" );
		writer.StartObject();
		writer.Key( "mode" ); writer.Int( static_cast<int>( HitboxData::config.mode ) );
		writer.EndObject();

		writer.String( "LagComp" );
		writer.StartObject();
		writer.Key( "drawBacktrack" ); writer.Bool( LagComp::debugConfig.drawBacktrack );
		ConfigJson::WriteColor( writer , "backtrackColor" , LagComp::debugConfig.color );
		writer.EndObject();

		writer.String( "WorldFov" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( WorldFov::config.enabled );
		writer.Key( "amount" ); writer.Double( WorldFov::config.amount );
		writer.EndObject();

		writer.String( "ViewModelFov" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( g_viewModelFovConfig.enabled );
		writer.Key( "amount" ); writer.Double( g_viewModelFovConfig.amount );
		writer.EndObject();

		writer.String( "Tracers" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( BulletTracer::config.enabled );
		writer.Key( "trailLife" ); writer.Double( BulletTracer::config.trailLife );
		writer.Key( "bulletSpeed" ); writer.Double( BulletTracer::config.bulletSpeed );
		writer.Key( "thickness" ); writer.Double( BulletTracer::config.thickness );
		writer.Key( "rayLength" ); writer.Double( BulletTracer::config.rayLength );
		writer.EndObject();

		writer.String( "AntiAim" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( AntiAim::config.enabled );
		writer.Key( "mode" ); writer.Int( AntiAim::config.mode );
		writer.Key( "pitchType" ); writer.Int( AntiAim::config.pitchType );
		writer.Key( "yawType" ); writer.Int( AntiAim::config.yawType );
		writer.Key( "pitch" ); writer.Double( AntiAim::config.pitch );
		writer.Key( "yaw" ); writer.Double( AntiAim::config.yaw );
		writer.Key( "spinSpeed" ); writer.Double( AntiAim::config.spinSpeed );
		writer.EndObject();

		writer.String( "ThirdPerson" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( ThirdPerson::config.enabled );
		writer.Key( "smoothCamera" ); writer.Bool( ThirdPerson::config.smoothCamera );
		writer.Key( "distance" ); writer.Double( ThirdPerson::config.distance );
		writer.Key( "fov" ); writer.Int( ThirdPerson::config.fov );
		writer.Key( "smoothSpeed" ); writer.Double( ThirdPerson::config.smoothSpeed );
		writer.EndObject();

		writer.String( "Menu" );
		writer.StartObject();
		writer.Key( "toggleKey" ); writer.Int( MenuSettings::menuToggleKey );
		writer.EndObject();

		writer.String( "MenuEffects" );
		writer.StartObject();
		writer.Key( "watermark" ); writer.Bool( MenuEffects::config.watermark );
		writer.Key( "particles" ); writer.Bool( MenuEffects::config.particles );
		writer.Key( "blurPlaceholder" ); writer.Bool( MenuEffects::config.blurPlaceholder );
		writer.Key( "menuBackgroundImage" ); writer.Bool( MenuEffects::config.menuBackgroundImage );
		writer.Key( "menuBackgroundAlpha" ); writer.Double( MenuEffects::config.menuBackgroundAlpha );
		writer.Key( "maxParticles" ); writer.Int( MenuEffects::config.maxParticles );
		writer.Key( "particleLinkDistance" ); writer.Double( MenuEffects::config.particleLinkDistance );
		writer.EndObject();

		writer.String( "Misc" );
		writer.StartObject();
		writer.Key( "plantBomb" ); writer.Bool( PlantBomb::config.enabled );
		writer.EndObject();

		writer.String( "TextureOverride" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( CustomTexture::enabled );
		ConfigJson::WriteString( writer , "imagePath" , CustomTexture::imagePath );
		writer.Key( "filterWidth" ); writer.Int( CustomTexture::filterWidth );
		writer.Key( "filterHeight" ); writer.Int( CustomTexture::filterHeight );
		writer.Key( "minSize" ); writer.Int( CustomTexture::minSize );
		writer.Key( "noFilter" ); writer.Bool( CustomTexture::noFilter );
		writer.EndObject();

		writer.String( "Gloves" );
		writer.StartObject();
		writer.Key( "enabled" ); writer.Bool( Gloves::config.enabled );
		writer.Key( "paintKit" ); writer.Int( Gloves::config.paintKit );
		writer.Key( "modelIndex" ); writer.Int( Gloves::config.modelIndex );
		writer.Key( "wear" ); writer.Double( Gloves::config.wear );
		writer.Key( "seed" ); writer.Int( Gloves::config.seed );
		writer.Key( "team" ); writer.Int( Gloves::config.team );
		writer.EndObject();

		writer.String( "Skins" );
		writer.StartObject();
		writer.Key( "knifeChangerEnabled" ); writer.Bool( SkinChanger::knifeChangerEnabled );
		writer.Key( "selectedKnifeModel" ); writer.Int( SkinChanger::selectedKnifeModel );
		writer.Key( "weaponSkins" );
		writer.StartArray();
		{
			std::lock_guard<std::mutex> lock( SkinChanger::configMutex );
			for ( const auto& entry : SkinChanger::weaponSkins )
			{
				const int defIndex = entry.first;
				const auto& cfg = entry.second;
				if ( !cfg.enabled && cfg.paintKit <= 0 )
					continue;

				writer.StartObject();
				writer.Key( "defIndex" ); writer.Int( defIndex );
				writer.Key( "enabled" ); writer.Bool( cfg.enabled );
				writer.Key( "paintKit" ); writer.Int( cfg.paintKit );
				writer.Key( "wear" ); writer.Double( cfg.wear );
				writer.Key( "seed" ); writer.Int( cfg.seed );
				writer.Key( "statTrak" ); writer.Int( cfg.statTrak );
				writer.Key( "knifeModel" ); writer.Int( cfg.knifeModel );
				writer.EndObject();
			}
		}
		writer.EndArray();
		writer.EndObject();
	}

	auto ReadSettings( const rapidjson::Document& document ) -> void
	{
		int schemaVersion = 0;
		if ( document.HasMember( ConfigSchema::kVersionKey ) && document[ConfigSchema::kVersionKey].IsInt() )
			schemaVersion = document[ConfigSchema::kVersionKey].GetInt();

		if ( !document.HasMember( ConfigSchema::kSettingsKey ) || !document[ConfigSchema::kSettingsKey].IsObject() )
			return;

		const auto& root = document[ConfigSchema::kSettingsKey];

		if ( root.HasMember( "Ragebot" ) && root["Ragebot"].IsObject() )
		{
			const auto& v = root["Ragebot"];
			ConfigJson::ReadBool( v , "enabled" , Ragebot::config.enabled );
			ConfigJson::ReadInt( v , "minDamage" , Ragebot::config.minDamage );
			ConfigJson::ReadInt( v , "hitchance" , Ragebot::config.hitchance );
			ConfigJson::ReadInt( v , "multipointScale" , Ragebot::config.multipointScale );
			ConfigJson::ReadBool( v , "autoStop" , Ragebot::config.autoStop );
			ConfigJson::ReadBool( v , "earlyAutoStop" , Ragebot::config.earlyAutoStop );
			ConfigJson::ReadBool( v , "penetration" , Ragebot::config.penetration );
			ConfigJson::ReadBool( v , "safePoint" , Ragebot::config.safePoint );
			ConfigJson::ReadBool( v , "adaptiveWeapon" , Ragebot::config.adaptiveWeapon );
			ConfigJson::ReadBool( v , "autoShoot" , Ragebot::config.autoShoot );
			ConfigJson::ReadBool( v , "silentAim" , Ragebot::config.silentAim );
			ConfigJson::ReadBool( v , "backtrack" , Ragebot::config.backtrack );
			ConfigJson::ReadBool( v , "teamCheck" , Ragebot::config.teamCheck );
			int mode = static_cast<int>( Ragebot::config.hitscanMode );
			ConfigJson::ReadInt( v , "hitscanMode" , mode );
			Ragebot::config.hitscanMode = static_cast<RageScan::HitscanMode>( mode );
			mode = static_cast<int>( Ragebot::config.scanMode );
			ConfigJson::ReadInt( v , "scanMode" , mode );
			Ragebot::config.scanMode = static_cast<RageScan::ScanMode>( mode );
			mode = static_cast<int>( Ragebot::config.stopMode );
			ConfigJson::ReadInt( v , "stopMode" , mode );
			Ragebot::config.stopMode = static_cast<RageScan::StopMode>( mode );
			mode = static_cast<int>( Ragebot::config.targetSelect );
			ConfigJson::ReadInt( v , "targetSelect" , mode );
			Ragebot::config.targetSelect = static_cast<RageScan::TargetSelect>( mode );
			ConfigJson::ReadBool( v , "hitboxHead" , Ragebot::config.hitboxHead );
			ConfigJson::ReadBool( v , "hitboxNeck" , Ragebot::config.hitboxNeck );
			ConfigJson::ReadBool( v , "hitboxChest" , Ragebot::config.hitboxChest );
			ConfigJson::ReadBool( v , "hitboxPelvis" , Ragebot::config.hitboxPelvis );
			ConfigJson::ReadBool( v , "hitboxStomach" , Ragebot::config.hitboxStomach );
			ConfigJson::ReadBool( v , "hitboxArms" , Ragebot::config.hitboxArms );
			ConfigJson::ReadBool( v , "hitboxLegs" , Ragebot::config.hitboxLegs );
			ConfigJson::ReadBool( v , "hitboxUpperChest" , Ragebot::config.hitboxUpperChest );
			ConfigJson::ReadBool( v , "hitboxFeet" , Ragebot::config.hitboxFeet );
			ConfigJson::ReadBool( v , "rapidFire" , Ragebot::config.rapidFire );
			ConfigJson::ReadBool( v , "autoScope" , Ragebot::config.autoScope );
			ConfigJson::ReadBool( v , "delayAim" , Ragebot::config.delayAim );
			ConfigJson::ReadInt( v , "delayAimMs" , Ragebot::config.delayAimMs );
			ConfigJson::ReadBool( v , "usePerWeapon" , Ragebot::config.usePerWeapon );
			ConfigJson::ReadInt( v , "perWeaponClass" , Ragebot::config.perWeaponClass );
			Ragebot::config.perWeaponClass = ConfigJson::ClampWeaponClassIndex( Ragebot::config.perWeaponClass );
		}

		if ( root.HasMember( "LegitBot" ) && root["LegitBot"].IsObject() )
		{
			const auto& v = root["LegitBot"];
			ConfigJson::ReadBool( v , "enabled" , LegitBot::config.enabled );
			ConfigJson::ReadBool( v , "usePerWeapon" , LegitBot::config.usePerWeapon );
			ConfigJson::ReadInt( v , "perWeaponClass" , LegitBot::config.perWeaponClass );
			LegitBot::config.perWeaponClass = ConfigJson::ClampWeaponClassIndex( LegitBot::config.perWeaponClass );
			ConfigJson::ReadInt( v , "targetSelection" , LegitBot::config.targetSelection );
			ConfigJson::ReadBool( v , "noScope" , LegitBot::config.noScope );
			ConfigJson::ReadBool( v , "delayAim" , LegitBot::config.delayAim );
			ConfigJson::ReadInt( v , "delayAimMs" , LegitBot::config.delayAimMs );
			unsigned int conditions = LegitBot::config.conditions;
			if ( v.HasMember( "conditions" ) && v["conditions"].IsUint() )
				conditions = v["conditions"].GetUint();
			LegitBot::config.conditions = conditions;
			ConfigJson::ReadBool( v , "triggerOverride" , LegitBot::config.triggerOverride );
			ConfigJson::ReadInt( v , "triggerHitchance" , LegitBot::config.triggerHitchance );
			ConfigJson::ReadBool( v , "removeSpread" , LegitBot::config.removeSpread );
			ConfigJson::ReadBool( v , "fovVisualize" , LegitBot::config.fovVisualize );
			ConfigJson::ReadColor( v , "fovVisualizeColor" , LegitBot::config.fovVisualizeColor );
			ConfigJson::ReadBool( v , "hitboxHead" , LegitBot::config.hitboxHead );
			ConfigJson::ReadBool( v , "hitboxNeck" , LegitBot::config.hitboxNeck );
			ConfigJson::ReadBool( v , "hitboxChest" , LegitBot::config.hitboxChest );
			ConfigJson::ReadBool( v , "hitboxUpperChest" , LegitBot::config.hitboxUpperChest );
			ConfigJson::ReadBool( v , "hitboxPelvis" , LegitBot::config.hitboxPelvis );
			ConfigJson::ReadBool( v , "hitboxStomach" , LegitBot::config.hitboxStomach );
			ConfigJson::ReadBool( v , "hitboxLegs" , LegitBot::config.hitboxLegs );
			ConfigJson::ReadBool( v , "hitboxArms" , LegitBot::config.hitboxArms );
		}

		ReadWeaponConfig( root , schemaVersion );

		if ( root.HasMember( "Aimbot" ) && root["Aimbot"].IsObject() )
		{
			const auto& v = root["Aimbot"];
			ConfigJson::ReadBool( v , "enabled" , Aimbot::config.enabled );
			ConfigJson::ReadInt( v , "fovType" , Aimbot::config.fovType );
			ConfigJson::ReadFloat( v , "fov" , Aimbot::config.fov );
			ConfigJson::ReadFloat( v , "screenFov" , Aimbot::config.screenFov );
			ConfigJson::ReadInt( v , "targetHitbox" , Aimbot::config.targetHitbox );
			ConfigJson::ReadInt( v , "aimKey" , Aimbot::config.aimKey );
			ConfigJson::ReadBool( v , "autoShoot" , Aimbot::config.autoShoot );
			ConfigJson::ReadBool( v , "silentAim" , Aimbot::config.silentAim );
			ConfigJson::ReadBool( v , "teamCheck" , Aimbot::config.teamCheck );
			ConfigJson::ReadBool( v , "visCheck" , Aimbot::config.visCheck );
			ConfigJson::ReadBool( v , "penetration" , Aimbot::config.penetration );
			ConfigJson::ReadBool( v , "recoilControl" , Aimbot::config.recoilControl );
			ConfigJson::ReadInt( v , "smooth" , Aimbot::config.smooth );
			ConfigJson::ReadBool( v , "punchRandomization" , Aimbot::config.punchRandomization );
			ConfigJson::ReadFloat( v , "punchRandomX" , Aimbot::config.punchRandomX );
			ConfigJson::ReadFloat( v , "punchRandomY" , Aimbot::config.punchRandomY );
			ConfigJson::ReadInt( v , "targetSelection" , Aimbot::config.targetSelection );
			ConfigJson::ReadBool( v , "hitboxHead" , Aimbot::config.hitboxHead );
			ConfigJson::ReadBool( v , "hitboxNeck" , Aimbot::config.hitboxNeck );
			ConfigJson::ReadBool( v , "hitboxChest" , Aimbot::config.hitboxChest );
			ConfigJson::ReadBool( v , "hitboxUpperChest" , Aimbot::config.hitboxUpperChest );
			ConfigJson::ReadBool( v , "hitboxPelvis" , Aimbot::config.hitboxPelvis );
			ConfigJson::ReadBool( v , "hitboxStomach" , Aimbot::config.hitboxStomach );
			ConfigJson::ReadBool( v , "hitboxLegs" , Aimbot::config.hitboxLegs );
			ConfigJson::ReadBool( v , "hitboxArms" , Aimbot::config.hitboxArms );
			ConfigJson::ReadBool( v , "rcsShotsEnable" , Aimbot::config.rcsShotsEnable );
			ConfigJson::ReadInt( v , "rcsShots" , Aimbot::config.rcsShots );
			ConfigJson::ReadFloat( v , "rcsSmoothX" , Aimbot::config.rcsSmoothX );
			ConfigJson::ReadFloat( v , "rcsSmoothY" , Aimbot::config.rcsSmoothY );
			ConfigJson::ReadBool( v , "fovVisualize" , Aimbot::config.fovVisualize );
			ConfigJson::ReadColor( v , "fovVisualizeColor" , Aimbot::config.fovVisualizeColor );
		}

		if ( root.HasMember( "NoSpread" ) && root["NoSpread"].IsObject() )
			ConfigJson::ReadBool( root["NoSpread"] , "enabled" , NoSpread::config.enabled );

		if ( root.HasMember( "Triggerbot" ) && root["Triggerbot"].IsObject() )
		{
			const auto& v = root["Triggerbot"];
			ConfigJson::ReadBool( v , "enabled" , Triggerbot::config.enabled );
			ConfigJson::ReadInt( v , "key" , Triggerbot::config.key );
			ConfigJson::ReadBool( v , "useKey" , Triggerbot::config.useKey );
			ConfigJson::ReadBool( v , "teamCheck" , Triggerbot::config.teamCheck );
			ConfigJson::ReadBool( v , "visCheck" , Triggerbot::config.visCheck );
			ConfigJson::ReadInt( v , "hitchance" , Triggerbot::config.hitchance );
			ConfigJson::ReadInt( v , "delayMs" , Triggerbot::config.delayMs );
		}

		if ( root.HasMember( "ESP" ) && root["ESP"].IsObject() )
		{
			const auto& v = root["ESP"];
			ConfigJson::ReadBool( v , "enabled" , ESP::config.enabled );
			ConfigJson::ReadBool( v , "bBox" , ESP::config.bBox );
			ConfigJson::ReadInt( v , "boxMode" , ESP::config.boxMode );
			ConfigJson::ReadBool( v , "bSkeleton" , ESP::config.bSkeleton );
			ConfigJson::ReadBool( v , "bHealthBar" , ESP::config.bHealthBar );
			ConfigJson::ReadBool( v , "bName" , ESP::config.bName );
			ConfigJson::ReadBool( v , "bDistance" , ESP::config.bDistance );
			ConfigJson::ReadBool( v , "teamCheck" , ESP::config.teamCheck );
			ConfigJson::ReadFloat( v , "maxDistance" , ESP::config.maxDistance );
			ConfigJson::ReadBool( v , "bBombTimer" , ESP::config.bBombTimer );
			ConfigJson::ReadBool( v , "bGlow" , ESP::config.bGlow );
			ConfigJson::ReadBool( v , "bSpectators" , ESP::config.bSpectators );
			ConfigJson::ReadColor( v , "boxColor" , ESP::config.boxColor );
			ConfigJson::ReadColor( v , "skeletonColor" , ESP::config.skeletonColor );
			ConfigJson::ReadColor( v , "glowColor" , ESP::config.glowColor );
		}

		if ( root.HasMember( "EspOverlay" ) && root["EspOverlay"].IsObject() )
		{
			const auto& v = root["EspOverlay"];
			ConfigJson::ReadBool( v , "enabled" , EspOverlay::config.enabled );
			ConfigJson::ReadBool( v , "useComponents" , EspOverlay::config.useComponents );
			ConfigJson::ReadBool( v , "showBox" , EspOverlay::config.showBox );
			ConfigJson::ReadBool( v , "showHealthBar" , EspOverlay::config.showHealthBar );
			ConfigJson::ReadBool( v , "showName" , EspOverlay::config.showName );
			ConfigJson::ReadBool( v , "showDistance" , EspOverlay::config.showDistance );
			ConfigJson::ReadBool( v , "showWeaponIcon" , EspOverlay::config.showWeaponIcon );
			ConfigJson::ReadBool( v , "showAmmoBar" , EspOverlay::config.showAmmoBar );
			ConfigJson::ReadBool( v , "showHelmetFlag" , EspOverlay::config.showHelmetFlag );
			ConfigJson::ReadBool( v , "showKitFlag" , EspOverlay::config.showKitFlag );
			ConfigJson::ReadColor( v , "boxColor" , EspOverlay::config.boxColor );
			ConfigJson::ReadColor( v , "healthBarColor" , EspOverlay::config.healthBarColor );
			ConfigJson::ReadColor( v , "ammoBarColor" , EspOverlay::config.ammoBarColor );
			ConfigJson::ReadColor( v , "textColor" , EspOverlay::config.textColor );
			ConfigJson::ReadFloat( v , "boxThickness" , EspOverlay::config.boxThickness );
			ConfigJson::ReadFloat( v , "barThickness" , EspOverlay::config.barThickness );
		}

		if ( root.HasMember( "Chams" ) && root["Chams"].IsObject() )
		{
			const auto& v = root["Chams"];
			ConfigJson::ReadBool( v , "enabled" , Chams::config.enabled );
			ConfigJson::ReadBool( v , "enemy" , Chams::config.enemy );
			ConfigJson::ReadBool( v , "local" , Chams::config.local );
			ConfigJson::ReadBool( v , "teammate" , Chams::config.teammate );
			ConfigJson::ReadBool( v , "weapon" , Chams::config.weapon );
			ConfigJson::ReadBool( v , "hands" , Chams::config.hands );
			int matStyle = static_cast<int>( Chams::config.materialStyle );
			ConfigJson::ReadInt( v , "materialStyle" , matStyle );
			Chams::config.materialStyle = static_cast<Chams::MaterialStyle>( matStyle );
			ConfigJson::ReadBool( v , "ignoreZ" , Chams::config.ignoreZ );
			ConfigJson::ReadColor( v , "color" , Chams::config.color );
			ConfigJson::ReadColor( v , "ignoreZColor" , Chams::config.ignoreZColor );
		}

		if ( root.HasMember( "Movement" ) && root["Movement"].IsObject() )
		{
			const auto& v = root["Movement"];
			ConfigJson::ReadBool( v , "movementFix" , Movement::config.movementFix );
			ConfigJson::ReadBool( v , "movementCorrection" , Movement::config.movementCorrection );
			ConfigJson::ReadBool( v , "validateAngles" , Movement::config.validateAngles );
			ConfigJson::ReadBool( v , "edgeBug" , Movement::config.edgeBug );
			ConfigJson::ReadBool( v , "edgeBugUseKey" , Movement::config.edgeBugUseKey );
			int edgeKey = Movement::config.edgeBugKey;
			ConfigJson::ReadInt( v , "edgeBugKey" , edgeKey );
			Movement::config.edgeBugKey = edgeKey;
			ConfigJson::ReadFloat( v , "strafeSmooth" , Movement::config.strafeSmooth );
			ConfigJson::ReadBool( v , "strafeAssist" , Movement::config.strafeAssist );
			int mode = static_cast<int>( Movement::config.strafeMode );
			ConfigJson::ReadInt( v , "strafeMode" , mode );
			Movement::config.strafeMode = static_cast<Movement::StrafeMode>( mode );
		}

		if ( root.HasMember( "Bhop" ) && root["Bhop"].IsObject() )
		{
			const auto& v = root["Bhop"];
			ConfigJson::ReadBool( v , "enabled" , Bhop::config.enabled );
			ConfigJson::ReadBool( v , "edgeJump" , Bhop::config.edgeJump );
			ConfigJson::ReadBool( v , "jumpBug" , Bhop::config.jumpBug );
			ConfigJson::ReadBool( v , "requireSpace" , Bhop::config.requireSpace );
			ConfigJson::ReadBool( v , "autoForward" , Bhop::config.autoForward );
			ConfigJson::ReadInt( v , "hopChance" , Bhop::config.hopChance );
			bool legacyAutoStrafe = false;
			ConfigJson::ReadBool( v , "autoStrafe" , legacyAutoStrafe );
			if ( legacyAutoStrafe && Movement::config.strafeMode == Movement::StrafeMode::Off )
				Movement::config.strafeMode = Movement::StrafeMode::Legit;
		}

		if ( root.HasMember( "AntiAim" ) && root["AntiAim"].IsObject() )
		{
			const auto& v = root["AntiAim"];
			ConfigJson::ReadBool( v , "enabled" , AntiAim::config.enabled );
			ConfigJson::ReadInt( v , "mode" , AntiAim::config.mode );
			ConfigJson::ReadInt( v , "pitchType" , AntiAim::config.pitchType );
			ConfigJson::ReadInt( v , "yawType" , AntiAim::config.yawType );
			ConfigJson::ReadFloat( v , "pitch" , AntiAim::config.pitch );
			ConfigJson::ReadFloat( v , "yaw" , AntiAim::config.yaw );
			ConfigJson::ReadFloat( v , "spinSpeed" , AntiAim::config.spinSpeed );
		}

		if ( root.HasMember( "ThirdPerson" ) && root["ThirdPerson"].IsObject() )
		{
			const auto& v = root["ThirdPerson"];
			ConfigJson::ReadBool( v , "enabled" , ThirdPerson::config.enabled );
			ConfigJson::ReadBool( v , "smoothCamera" , ThirdPerson::config.smoothCamera );
			ConfigJson::ReadFloat( v , "distance" , ThirdPerson::config.distance );
			ConfigJson::ReadInt( v , "fov" , ThirdPerson::config.fov );
			ConfigJson::ReadFloat( v , "smoothSpeed" , ThirdPerson::config.smoothSpeed );
		}

		if ( root.HasMember( "WorldVisuals" ) && root["WorldVisuals"].IsObject() )
		{
			const auto& v = root["WorldVisuals"];
			ConfigJson::ReadBool( v , "nightMode" , WorldVisuals::config.nightMode );
			ConfigJson::ReadFloat( v , "nightAmbient" , WorldVisuals::config.nightAmbient );
			ConfigJson::ReadBool( v , "worldModulate" , WorldVisuals::config.worldModulate );
			ConfigJson::ReadColor( v , "modulateColor" , WorldVisuals::config.modulateColor );
			ConfigJson::ReadBool( v , "noShadow" , WorldVisuals::config.noShadow );
		}

		if ( root.HasMember( "HitboxData" ) && root["HitboxData"].IsObject() )
		{
			const auto& v = root["HitboxData"];
			int mode = static_cast<int>( HitboxData::config.mode );
			ConfigJson::ReadInt( v , "mode" , mode );
			HitboxData::config.mode = static_cast<HitboxData::Mode>( std::clamp( mode , 0 , 1 ) );
		}

		if ( root.HasMember( "LagComp" ) && root["LagComp"].IsObject() )
		{
			const auto& v = root["LagComp"];
			ConfigJson::ReadBool( v , "drawBacktrack" , LagComp::debugConfig.drawBacktrack );
			ConfigJson::ReadColor( v , "backtrackColor" , LagComp::debugConfig.color );
		}

		if ( root.HasMember( "WorldFov" ) && root["WorldFov"].IsObject() )
		{
			const auto& v = root["WorldFov"];
			ConfigJson::ReadBool( v , "enabled" , WorldFov::config.enabled );
			ConfigJson::ReadFloat( v , "amount" , WorldFov::config.amount );
		}

		if ( root.HasMember( "ViewModelFov" ) && root["ViewModelFov"].IsObject() )
		{
			const auto& v = root["ViewModelFov"];
			ConfigJson::ReadBool( v , "enabled" , g_viewModelFovConfig.enabled );
			ConfigJson::ReadFloat( v , "amount" , g_viewModelFovConfig.amount );
		}

		if ( root.HasMember( "Tracers" ) && root["Tracers"].IsObject() )
		{
			const auto& v = root["Tracers"];
			ConfigJson::ReadBool( v , "enabled" , BulletTracer::config.enabled );
			ConfigJson::ReadFloat( v , "trailLife" , BulletTracer::config.trailLife );
			ConfigJson::ReadFloat( v , "bulletSpeed" , BulletTracer::config.bulletSpeed );
			ConfigJson::ReadFloat( v , "thickness" , BulletTracer::config.thickness );
			ConfigJson::ReadFloat( v , "rayLength" , BulletTracer::config.rayLength );
		}

		if ( root.HasMember( "Menu" ) && root["Menu"].IsObject() )
		{
			int menuKey = MenuSettings::menuToggleKey;
			ConfigJson::ReadInt( root["Menu"] , "toggleKey" , menuKey );
			MenuSettings::menuToggleKey = menuKey;
		}

		if ( root.HasMember( "MenuEffects" ) && root["MenuEffects"].IsObject() )
		{
			const auto& v = root["MenuEffects"];
			ConfigJson::ReadBool( v , "watermark" , MenuEffects::config.watermark );
			ConfigJson::ReadBool( v , "particles" , MenuEffects::config.particles );
			ConfigJson::ReadBool( v , "blurPlaceholder" , MenuEffects::config.blurPlaceholder );
			ConfigJson::ReadBool( v , "menuBackgroundImage" , MenuEffects::config.menuBackgroundImage );
			ConfigJson::ReadFloat( v , "menuBackgroundAlpha" , MenuEffects::config.menuBackgroundAlpha );
			ConfigJson::ReadInt( v , "maxParticles" , MenuEffects::config.maxParticles );
			ConfigJson::ReadFloat( v , "particleLinkDistance" , MenuEffects::config.particleLinkDistance );
		}
		else if ( root.HasMember( "Misc" ) && root["Misc"].IsObject() )
		{
			const auto& v = root["Misc"];
			ConfigJson::ReadBool( v , "watermark" , MenuEffects::config.watermark );
			ConfigJson::ReadBool( v , "particles" , MenuEffects::config.particles );
			ConfigJson::ReadBool( v , "blurPlaceholder" , MenuEffects::config.blurPlaceholder );
			ConfigJson::ReadBool( v , "menuBackgroundImage" , MenuEffects::config.menuBackgroundImage );
			ConfigJson::ReadFloat( v , "menuBackgroundAlpha" , MenuEffects::config.menuBackgroundAlpha );
			ConfigJson::ReadInt( v , "maxParticles" , MenuEffects::config.maxParticles );
			ConfigJson::ReadBool( v , "plantBomb" , PlantBomb::config.enabled );
			ConfigJson::ReadBool( v , "bulletTracers" , BulletTracer::config.enabled );
		}

		if ( root.HasMember( "TextureOverride" ) && root["TextureOverride"].IsObject() )
		{
			const auto& v = root["TextureOverride"];
			ConfigJson::ReadBool( v , "enabled" , CustomTexture::enabled );
			ConfigJson::ReadString( v , "imagePath" , CustomTexture::imagePath , sizeof( CustomTexture::imagePath ) );
			ConfigJson::ReadInt( v , "filterWidth" , CustomTexture::filterWidth );
			ConfigJson::ReadInt( v , "filterHeight" , CustomTexture::filterHeight );
			ConfigJson::ReadInt( v , "minSize" , CustomTexture::minSize );
			ConfigJson::ReadBool( v , "noFilter" , CustomTexture::noFilter );
		}

		if ( root.HasMember( "Gloves" ) && root["Gloves"].IsObject() )
		{
			const auto& v = root["Gloves"];
			ConfigJson::ReadBool( v , "enabled" , Gloves::config.enabled );
			ConfigJson::ReadInt( v , "paintKit" , Gloves::config.paintKit );
			ConfigJson::ReadInt( v , "modelIndex" , Gloves::config.modelIndex );
			ConfigJson::ReadFloat( v , "wear" , Gloves::config.wear );
			ConfigJson::ReadInt( v , "seed" , Gloves::config.seed );
			ConfigJson::ReadInt( v , "team" , Gloves::config.team );
		}

		if ( root.HasMember( "Skins" ) && root["Skins"].IsObject() )
		{
			const auto& v = root["Skins"];
			ConfigJson::ReadBool( v , "knifeChangerEnabled" , SkinChanger::knifeChangerEnabled );
			ConfigJson::ReadInt( v , "selectedKnifeModel" , SkinChanger::selectedKnifeModel );

			if ( v.HasMember( "weaponSkins" ) && v["weaponSkins"].IsArray() )
			{
				std::lock_guard<std::mutex> lock( SkinChanger::configMutex );
				SkinChanger::weaponSkins.clear();

				for ( const auto& item : v["weaponSkins"].GetArray() )
				{
					if ( !item.IsObject() || !item.HasMember( "defIndex" ) || !item["defIndex"].IsInt() )
						continue;

					SkinChanger::SkinConfig cfg{};
					const int defIndex = item["defIndex"].GetInt();
					ConfigJson::ReadBool( item , "enabled" , cfg.enabled );
					ConfigJson::ReadInt( item , "paintKit" , cfg.paintKit );
					ConfigJson::ReadFloat( item , "wear" , cfg.wear );
					ConfigJson::ReadInt( item , "seed" , cfg.seed );
					ConfigJson::ReadInt( item , "statTrak" , cfg.statTrak );
					ConfigJson::ReadInt( item , "knifeModel" , cfg.knifeModel );

					if ( cfg.paintKit < 0 )
						cfg.paintKit = 0;

					SkinChanger::weaponSkins[defIndex] = cfg;
				}

				SkinChanger::forceUpdate.store( true );
			}
		}

		if ( Ragebot::config.enabled )
		{
			Aimbot::config.enabled = false;
			LegitBot::config.enabled = false;
			Triggerbot::config.enabled = false;
		}
		else
		{
			LegitBot::SyncToAimbot();
		}

		if ( schemaVersion < kSchemaVersion )
		{
			auto migrateWeaponClassIndex = []( int idx ) -> int
			{
				switch ( idx )
				{
				case 0: return static_cast<int>( WeaponConfig::WeaponClass::Pistol );
				case 1: return static_cast<int>( WeaponConfig::WeaponClass::Auto );
				case 2: return static_cast<int>( WeaponConfig::WeaponClass::AssaultRifle );
				case 3: return static_cast<int>( WeaponConfig::WeaponClass::Scout );
				default: return ConfigJson::ClampWeaponClassIndex( idx );
				}
			};

			Ragebot::config.perWeaponClass = migrateWeaponClassIndex( Ragebot::config.perWeaponClass );
			LegitBot::config.perWeaponClass = migrateWeaponClassIndex( LegitBot::config.perWeaponClass );
		}
	}
}

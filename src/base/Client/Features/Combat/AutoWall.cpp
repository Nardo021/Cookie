#include "AutoWall.hpp"

#include "AutoWallTrace.hpp"

#include <algorithm>
#include <cmath>
#include <tuple>

#include <Client/Core/CConVars.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Patterns.hpp>
#include <Client/Game/Trace.hpp>

#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/GameTrace.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace AutoWall
{
	namespace
	{
		inline constexpr int kMaxPenetrations = 4;
		inline constexpr float kMaxTraceLength = 3000.f;

		AutoWallTrace::CreateTraceFn s_createTrace = nullptr;
		AutoWallTrace::InitTraceInfoFn s_initTraceInfo = nullptr;
		AutoWallTrace::GetTraceInfoFn s_getTraceInfo = nullptr;
		AutoWallTrace::HandleBulletPenetrationFn s_handleBulletPenetration = nullptr;
		AutoWallTrace::InitTraceFilterFn s_initTraceFilter = nullptr;
		bool s_traceApiResolved = false;
		bool s_useNativePenetration = false;

		auto ResolveTraceApi() noexcept -> bool
		{
			if ( s_traceApiResolved )
				return s_useNativePenetration;

			s_traceApiResolved = true;

			if ( !Game::clientBase )
				Game::clientBase = Game::GetModuleBase( L"client.dll" );

			if ( !Game::clientBase )
				return false;

			const uintptr_t createTraceAddr = Game::FindPattern( L"client.dll" , Patterns::sig_CreateTrace );
			const uintptr_t initTraceInfoAddr = Game::FindPattern( L"client.dll" , Patterns::sig_InitTraceInfo );
			const uintptr_t getTraceInfoAddr = Game::FindPattern( L"client.dll" , Patterns::sig_GetTraceInfo );
			const uintptr_t handlePenAddr = Game::FindPattern( L"client.dll" , Patterns::sig_HandleBulletPenetration );
			const uintptr_t initFilterAddr = Game::FindPattern( L"client.dll" , Patterns::sig_InitFilter );

			if ( createTraceAddr )
				s_createTrace = reinterpret_cast<AutoWallTrace::CreateTraceFn>( createTraceAddr );
			if ( initTraceInfoAddr )
				s_initTraceInfo = reinterpret_cast<AutoWallTrace::InitTraceInfoFn>( initTraceInfoAddr );
			if ( getTraceInfoAddr )
				s_getTraceInfo = reinterpret_cast<AutoWallTrace::GetTraceInfoFn>( getTraceInfoAddr );
			if ( handlePenAddr )
				s_handleBulletPenetration = reinterpret_cast<AutoWallTrace::HandleBulletPenetrationFn>( handlePenAddr );
			if ( initFilterAddr )
				s_initTraceFilter = reinterpret_cast<AutoWallTrace::InitTraceFilterFn>( initFilterAddr );
			else if ( Trace::fnInitTraceFilter )
				s_initTraceFilter = reinterpret_cast<AutoWallTrace::InitTraceFilterFn>( Trace::fnInitTraceFilter );

			s_useNativePenetration = s_createTrace && s_initTraceInfo && s_getTraceInfo && s_handleBulletPenetration && s_initTraceFilter;
			return s_useNativePenetration;
		}

		auto GetDamageScaleCvars() noexcept -> std::tuple<float , float , float , float>
		{
			static float ctHead = 1.f;
			static float tHead = 1.f;
			static float ctBody = 1.f;
			static float tBody = 1.f;
			static bool cached = false;

			if ( !cached )
			{
				cached = true;

				if ( auto* ctHeadVar = CookieCore::CConVars::mp_damage_scale_ct_head )
					ctHead = ctHeadVar->value.fl;
				if ( auto* tHeadVar = CookieCore::CConVars::mp_damage_scale_t_head )
					tHead = tHeadVar->value.fl;
				if ( auto* ctBodyVar = CookieCore::CConVars::mp_damage_scale_ct_body )
					ctBody = ctBodyVar->value.fl;
				if ( auto* tBodyVar = CookieCore::CConVars::mp_damage_scale_t_body )
					tBody = tBodyVar->value.fl;
			}

			return { ctHead , tHead , ctBody , tBody };
		}

		auto InitPenetrationFilter( AutoWallTrace::TraceFilter& filter , C_CSPlayerPawn* localPawn ) noexcept -> void
		{
			s_initTraceFilter(
				&filter ,
				localPawn ,
				AutoWallTrace::kPenMask ,
				3 ,
				7 );
		}

		auto FireBulletNative(
			const Vector3& localPos ,
			const Vector3& targetPos ,
			C_CSPlayerPawn* localPawn ,
			C_CSPlayerPawn* targetPawn ,
			CCSWeaponBaseVData* weaponVData ,
			float& outDamage ,
			bool& outValid ) noexcept -> bool
		{
			AutoWallTrace::TraceData traceData{};
			traceData.arrPointer = traceData.arr.data();

			AutoWallTrace::TraceFilter filter{};
			InitPenetrationFilter( filter , localPawn );

			Vector3 direction = targetPos - localPos;
			const float distance = direction.Length();
			if ( distance <= 0.f )
				return false;

			direction /= distance;

			const float weaponRange = weaponVData->m_flRange();
			const Vector3 endPos = localPos + direction * weaponRange;

			s_createTrace( &traceData , localPos , endPos , filter , kMaxPenetrations );

			AutoWallTrace::HandleBulletData handleData(
				static_cast<float>( weaponVData->m_nDamage() ) ,
				weaponVData->m_flPenetration() ,
				weaponVData->m_flRangeModifier() ,
				weaponRange ,
				kMaxPenetrations ,
				false );

			float correctedDamage = static_cast<float>( weaponVData->m_nDamage() );
			float traceLength = 0.f;
			float maxRange = weaponRange;

			if ( traceData.numUpdate <= 0 )
				return false;

			for ( int i = 0; i < traceData.numUpdate; ++i )
			{
				auto* value = reinterpret_cast<AutoWallTrace::UpdateValue*>(
					reinterpret_cast<std::uintptr_t>( traceData.pointerUpdateValue )
					+ static_cast<std::uintptr_t>( i ) * sizeof( AutoWallTrace::UpdateValue ) );

				AutoWallTrace::GameTrace gameTrace{};
				s_initTraceInfo( &gameTrace );

				s_getTraceInfo(
					&traceData ,
					&gameTrace ,
					0.f ,
					reinterpret_cast<void*>(
						reinterpret_cast<std::uintptr_t>( traceData.arr.data() )
						+ sizeof( AutoWallTrace::TraceArrElement ) * ( value->handleIdx & 0x7FFF ) ) );

				maxRange -= traceLength;

				if ( gameTrace.fraction >= 1.f )
					break;

				traceLength += gameTrace.fraction * maxRange;
				correctedDamage *= std::powf( weaponVData->m_flRangeModifier() , traceLength / 500.f );

				if ( traceLength > kMaxTraceLength )
					break;

				if ( gameTrace.hitEntity == targetPawn )
				{
					const int hitGroup = gameTrace.hitboxData ? gameTrace.hitboxData->hitGroup : HITGROUP_GENERIC;
					ScaleDamage( correctedDamage , hitGroup , targetPawn , weaponVData );
					outDamage = correctedDamage;
					outValid = outDamage > 0.f;
					return outValid;
				}

				if ( s_handleBulletPenetration( &traceData , &handleData , value , nullptr , nullptr , nullptr , nullptr , nullptr , false ) )
					break;

				correctedDamage = handleData.damage;
			}

			return false;
		}

		auto FireBulletLegacy(
			const Vector3& localPos ,
			const Vector3& targetPos ,
			C_CSPlayerPawn* localPawn ,
			C_CSPlayerPawn* targetPawn ,
			CCSWeaponBaseVData* weaponVData ,
			float& outDamage ,
			bool& outValid ) noexcept -> bool
		{
			auto* physWorld = SDK::Pointers::CVPhys2World();
			if ( !physWorld )
				return false;

			Vector3 direction = targetPos - localPos;
			const float distance = direction.Length();
			if ( distance <= 0.f )
				return false;

			direction /= distance;

			const float weaponRange = weaponVData->m_flRange();
			float remainingRange = weaponRange;
			float currentDamage = static_cast<float>( weaponVData->m_nDamage() );
			Vector3 traceStart = localPos;
			Vector3 traceEnd = localPos + direction * weaponRange;

			Ray_t ray{};
			CTraceFilter filter( AutoWallTrace::kPenMask , localPawn , 4 , 15 );

			for ( int pen = 0; pen < kMaxPenetrations; ++pen )
			{
				CGameTrace gameTrace{};
				if ( !IGamePhysicsQuery_TraceShape( physWorld , ray , traceStart , traceEnd , &filter , &gameTrace ) )
					break;

				const float traveled = gameTrace.flFraction * remainingRange;
				currentDamage *= std::powf( weaponVData->m_flRangeModifier() , traveled / 500.f );

				if ( gameTrace.flFraction >= 1.f || !gameTrace.DidHit() )
					break;

				if ( gameTrace.pHitEntity == targetPawn )
				{
					outDamage = currentDamage;
					ScaleDamage( outDamage , gameTrace.GetHitGroup() , targetPawn , weaponVData );
					outValid = outDamage > 0.f;
					return outValid;
				}

				if ( gameTrace.flFraction <= 0.f )
					break;

				const float penStrength = weaponVData->m_flPenetration();
				if ( penStrength <= 0.f )
					break;

				currentDamage *= std::clamp( penStrength / 3.f , 0.1f , 1.f );
				remainingRange -= traveled;
				if ( remainingRange <= 0.f || currentDamage <= 1.f )
					break;

				traceStart = gameTrace.vecEnd + direction * 0.1f;
				traceEnd = traceStart + direction * remainingRange;
			}

			return false;
		}
	}

	auto ScaleDamage(
		float& damage ,
		int hitGroup ,
		C_CSPlayerPawn* target ,
		CCSWeaponBaseVData* weaponVData ) noexcept -> void
	{
		if ( !target || !weaponVData || damage <= 0.f )
			return;

		const auto [ctHead , tHead , ctBody , tBody] = GetDamageScaleCvars();
		const bool isCt = target->m_iTeamNum() == TEAM_CT;
		const bool isT = target->m_iTeamNum() == TEAM_TT;

		float headScale = isCt ? ctHead : isT ? tHead : 1.f;
		const float bodyScale = isCt ? ctBody : isT ? tBody : 1.f;

		auto* itemServices = target->m_pItemServices();
		if ( itemServices && itemServices->m_bHasHeavyArmor() )
			headScale *= 0.5f;

		switch ( hitGroup )
		{
		case HITGROUP_HEAD:
			damage *= weaponVData->m_flHeadshotMultiplier() * headScale;
			break;
		case HITGROUP_CHEST:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
		case HITGROUP_NECK:
			damage *= bodyScale;
			break;
		case HITGROUP_STOMACH:
			damage *= 1.25f * bodyScale;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			damage *= 0.75f * bodyScale;
			break;
		default:
			break;
		}

		if ( !target->HasArmor( hitGroup ) )
			return;

		float heavyBonus = 1.f;
		float armorBonus = 0.5f;
		float armorRatio = weaponVData->m_flArmorRatio() * 0.5f;

		if ( itemServices && itemServices->m_bHasHeavyArmor() )
		{
			heavyBonus = 0.25f;
			armorBonus = 0.33f;
			armorRatio *= 0.20f;
		}

		const float damageToHealth = damage * armorRatio;
		const float damageToArmor = ( damage - damageToHealth ) * ( heavyBonus * armorBonus );

		if ( damageToArmor > static_cast<float>( target->m_ArmorValue() ) )
			damage = damage - static_cast<float>( target->m_ArmorValue() ) / armorBonus;
		else
			damage = damageToHealth;
	}

	auto FireBullet(
		const Vector3& localPos ,
		const Vector3& targetPos ,
		C_CSPlayerPawn* localPawn ,
		C_CSPlayerPawn* targetPawn ,
		CCSWeaponBaseVData* weaponVData ,
		float& outDamage ,
		bool& outValid ) noexcept -> bool
	{
		outDamage = 0.f;
		outValid = false;

		if ( !localPawn || !targetPawn || !weaponVData )
			return false;

		if ( ResolveTraceApi() )
			return FireBulletNative( localPos , targetPos , localPawn , targetPawn , weaponVData , outDamage , outValid );

		return FireBulletLegacy( localPos , targetPos , localPawn , targetPawn , weaponVData , outDamage , outValid );
	}

	auto CanPenetrate(
		const Vector3& localPos ,
		const Vector3& targetPos ,
		C_CSPlayerPawn* targetPawn ,
		CCSWeaponBaseVData* weaponVData ) noexcept -> PenetrationResult
	{
		PenetrationResult result{};

		if ( !targetPawn )
			return result;

		if ( !weaponVData )
			weaponVData = GetCL_Weapons()->GetLocalWeaponVData();

		if ( !weaponVData )
			return result;

		auto* localPawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !localPawn )
			return result;

		float damage = 0.f;
		bool valid = false;
		if ( FireBullet( localPos , targetPos , localPawn , targetPawn , weaponVData , damage , valid ) )
		{
			result.canHit = valid;
			result.damage = damage;
		}

		return result;
	}
}

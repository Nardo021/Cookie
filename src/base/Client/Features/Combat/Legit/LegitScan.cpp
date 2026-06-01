#include "LegitScan.hpp"

#include <algorithm>
#include <cmath>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>

#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Game/Trace.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace LegitScan
{
	namespace
	{
		constexpr int kSeedMax = 255;
	}

	auto CollectHitboxes( const HitboxFilter& filter , std::vector<uint32_t>& out ) noexcept -> void
	{
		out.clear();

		if ( filter.head )
			out.push_back( HitboxData::StudioHitbox::Head );
		if ( filter.neck )
			out.push_back( HitboxData::StudioHitbox::Neck );
		if ( filter.chest )
		{
			out.push_back( HitboxData::StudioHitbox::Chest );
			out.push_back( HitboxData::StudioHitbox::RightChest );
			out.push_back( HitboxData::StudioHitbox::LeftChest );
		}
		if ( filter.upperChest )
		{
			out.push_back( HitboxData::StudioHitbox::RightChest );
			out.push_back( HitboxData::StudioHitbox::LeftChest );
		}
		if ( filter.pelvis )
			out.push_back( HitboxData::StudioHitbox::Pelvis );
		if ( filter.stomach )
		{
			out.push_back( HitboxData::StudioHitbox::Stomach );
			out.push_back( HitboxData::StudioHitbox::Center );
		}
		if ( filter.legs )
		{
			out.push_back( HitboxData::StudioHitbox::LeftLeg );
			out.push_back( HitboxData::StudioHitbox::RightLeg );
			out.push_back( HitboxData::StudioHitbox::LeftFeet );
			out.push_back( HitboxData::StudioHitbox::RightFeet );
		}
		if ( filter.arms )
		{
			out.push_back( HitboxData::StudioHitbox::LeftArm );
			out.push_back( HitboxData::StudioHitbox::RightArm );
		}

		if ( !out.empty() )
			return;

		static constexpr uint32_t kLegacyMap[] = {
			HitboxData::StudioHitbox::Head ,
			HitboxData::StudioHitbox::Neck ,
			HitboxData::StudioHitbox::Chest ,
			HitboxData::StudioHitbox::Pelvis ,
		};
		const int idx = std::clamp( filter.legacyFallback , 0 , static_cast<int>( sizeof( kLegacyMap ) / sizeof( kLegacyMap[0] ) ) - 1 );
		out.push_back( kLegacyMap[idx] );
	}

	auto IsValidTarget( C_CSPlayerPawn* pawn ) noexcept -> bool
	{
		if ( !pawn )
			return false;

		if ( pawn->m_bGunGameImmunity() )
			return false;

		if ( auto* sceneNode = pawn->m_pGameSceneNode() )
		{
			if ( sceneNode->m_bDormant() )
				return false;
		}

		return true;
	}

	auto GetHitboxPosition( C_CSPlayerPawn* pawn , uint32_t studioIndex ) noexcept -> ::Vector3
	{
		const auto sample = HitboxData::CalculateHitbox( pawn , studioIndex , false );
		return sample.valid ? sample.position : ::Vector3{};
	}

	auto CanShoot( C_CSPlayerPawn* localPawn , C_CSWeaponBaseGun* weapon ) noexcept -> bool
	{
		if ( !localPawn || !weapon )
			return false;

		if ( weapon->m_iClip1() <= 0 )
			return false;

		auto* globalVars = SDK::Pointers::GlobalVarsBase();
		if ( !globalVars )
			return true;

		const int nextAttackTick = weapon->m_nNextPrimaryAttackTick().m_Value;
		return nextAttackTick <= static_cast<int>( globalVars->m_nTickCount() );
	}

	auto IsSmokeBetween( const ::Vector3& start , const ::Vector3& end ) noexcept -> bool
	{
		if ( !LineGoesThroughSmoke )
			return false;

		return LineGoesThroughSmoke( start , end , 0 ) > 0.f;
	}

	auto CalcDrawFovRadius( float fovDegrees , float screenHeight ) noexcept -> float
	{
		if ( fovDegrees <= 0.f || screenHeight <= 0.f )
			return 0.f;

		constexpr float kDegToRad = 3.14159265358979323846f / 180.f;
		const float halfScreen = screenHeight * 0.5f;
		const float approxViewFov = 74.f;
		return halfScreen * std::tan( fovDegrees * kDegToRad ) / std::tan( approxViewFov * 0.5f * kDegToRad );
	}

	auto PassesHitchance(
		const Game::QAngle& viewAngles ,
		const Game::Vector3& targetPoint ,
		CUserCmd* cmd ,
		uintptr_t targetAddr ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		int hitchancePercent ) noexcept -> bool
	{
		(void)cmd;

		if ( hitchancePercent <= 0 )
			return true;
		if ( hitchancePercent >= 100 )
			return true;
		if ( !localPawn || !targetAddr )
			return false;

		auto* weapon = GetCL_Weapons()->GetLocalActiveWeapon();
		if ( !weapon )
			return false;

		auto* vdata = GetCL_Weapons()->GetLocalWeaponVData();
		const float weaponRange = vdata ? static_cast<float>( vdata->m_flRange() ) : 8192.f;

		float inaccuracy = 0.f;
		float spread = 0.f;
		int itemIndex = 0;
		int weaponMode = 0;
		float recoilIndex = 0.f;
		if ( !NoSpread::detail::GetWeaponSpreadData( weapon , inaccuracy , spread , itemIndex , weaponMode , recoilIndex ) )
			return false;

		const ::QAngle sdkAim( viewAngles.pitch , viewAngles.yaw , 0.f );
		const ::Vector3 start = GetCL_Players()->GetLocalEyeOrigin();
		const Game::Vector3 traceStart{ start.m_x , start.m_y , start.m_z };

		const float targetDist = start.Distance( ::Vector3( targetPoint.x , targetPoint.y , targetPoint.z ) );
		if ( targetDist <= 1.f )
			return true;
		if ( targetDist > weaponRange )
			return false;

		const size_t neededHits = static_cast<size_t>(
			std::ceil( ( hitchancePercent * static_cast<float>( kSeedMax ) ) / 100.f ) );

		size_t totalHits = 0;

		for ( int seed = 0; seed <= kSeedMax; ++seed )
		{
			float spreadX = 0.f;
			float spreadY = 0.f;
			NoSpread::detail::CalcSpreadValues(
				seed ,
				inaccuracy ,
				spread ,
				itemIndex ,
				weaponMode ,
				recoilIndex ,
				spreadX ,
				spreadY );

			const ::QAngle spreadView = NoSpread::detail::ApplySpread( sdkAim , spreadX , spreadY );
			::Vector3 spreadDir{};
			Math::AngleVectors( spreadView , spreadDir , nullptr , nullptr );
			spreadDir.Normalize();

			const ::Vector3 end = start + spreadDir * weaponRange;
			const Game::Vector3 traceEnd{ end.m_x , end.m_y , end.m_z };

			float fraction = 1.f;
			uintptr_t hitEntity = 0;
			if ( Trace::RunTrace(
				traceStart ,
				traceEnd ,
				localPawn ,
				localHandle ,
				Trace::kShotFilterProfile ,
				fraction ,
				nullptr ,
				&hitEntity ) && hitEntity == targetAddr )
			{
				++totalHits;
			}

			if ( totalHits >= neededHits )
				return true;

			if ( ( kSeedMax - seed + totalHits ) < neededHits )
				return false;
		}

		return totalHits >= neededHits;
	}
}

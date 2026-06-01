#include "RageScan.hpp"

#include <algorithm>
#include <cmath>

#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>
#include <CS2/SDK/TIER0/TIER0_Functions.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Types/CBaseTypes.hpp>

#include <Client/Features/Combat/Legit/Aimbot.hpp>
#include <Client/Features/Combat/AutoWall.hpp>
#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/Features/Combat/NoSpread.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Game/Trace.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace RageScan
{
	namespace
	{
		uintptr_t s_lastTarget = 0;
	}

	auto ResetTarget() noexcept -> void
	{
		s_lastTarget = 0;
	}

	auto GetLastTarget() noexcept -> uintptr_t
	{
		return s_lastTarget;
	}

	namespace
	{
		using Game::QAngle;
		using Game::Vector3;

		constexpr float kMaxScanDistance = 8192.f;
		constexpr float kDefaultWeaponDamage = 30.f;
		constexpr int   kSeedMax = 255;

		auto HitboxToHitGroup( uint32_t studioIndex ) noexcept -> int
		{
			return HitboxData::StudioToHitGroup( studioIndex );
		}

		auto ResolveBacktrackPoint(
			C_CSPlayerPawn* pawn ,
			const Vector3& eye ,
			const ::Vector3& point ,
			uint32_t studioIndex ,
			bool enabled ,
			PointResult& out ) noexcept -> ::Vector3
		{
			::Vector3 resolved = point;
			if ( !enabled || !pawn )
				return resolved;

			const ::Vector3 segStart( eye.x , eye.y , eye.z );
			const LagComp::BacktrackSelection selection = LagComp::FindBestBacktrack(
				pawn ,
				segStart ,
				point ,
				LagComp::GetLastValidSimTime() );

			if ( !selection.valid )
				return resolved;

			const ::Vector3 backtrackPos = LagComp::GetBacktrackHitbox( pawn , selection , studioIndex );
			if ( !backtrackPos.IsZero() )
				resolved = backtrackPos;

			out.usedBacktrack = true;
			out.backtrackTick = selection.tick;
			out.backtrackFraction = selection.tickFraction;
			out.backtrackSimTime = selection.simulationTime;
			return resolved;
		}

		auto IsValidTarget( C_CSPlayerPawn* pawn , uintptr_t pawnAddr ) noexcept -> bool
		{
			if ( !pawn || !pawnAddr )
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

		auto PassesSafePoint(
			const Game::Vector3& eye ,
			const HitboxData::Sample& hitbox ,
			int scale ,
			uintptr_t localPawn ,
			uint32_t localHandle ,
			uintptr_t targetAddr ) noexcept -> bool
		{
			std::vector<::Vector3> points;
			HitboxData::BuildMultipoints( hitbox , scale , points );
			if ( points.size() <= 1 )
				return Trace::IsVisible(
					{ eye.x , eye.y , eye.z } ,
					{ points[0].m_x , points[0].m_y , points[0].m_z } ,
					localPawn ,
					localHandle ,
					targetAddr );

			int visibleCount = 0;
			for ( const auto& sample : points )
			{
				if ( Trace::IsVisible(
					{ eye.x , eye.y , eye.z } ,
					{ sample.m_x , sample.m_y , sample.m_z } ,
					localPawn ,
					localHandle ,
					targetAddr ) )
				{
					++visibleCount;
				}
			}

			const int required = static_cast<int>( points.size() ) >= 4 ? 3 : 2;
			return visibleCount >= required;
		}

		auto GetEffectiveMinDamage( int targetHealth , const ScanSettings& settings ) noexcept -> int
		{
			const int base = std::max( 1 , settings.minDamage );

			switch ( settings.hitscanMode )
			{
			case HitscanMode::Lethal:
			case HitscanMode::Lethal2:
				return std::min( base , targetHealth );
			case HitscanMode::Prefer:
				return std::max( 1 , std::min( base , targetHealth + 10 ) );
			default:
				return base;
			}
		}

		auto PassesMinDamage( float damage , int targetHealth , const ScanSettings& settings ) noexcept -> bool
		{
			if ( damage >= static_cast<float>( targetHealth ) )
				return true;

			return damage >= static_cast<float>( GetEffectiveMinDamage( targetHealth , settings ) );
		}

		auto IsLethalShot( float damage , int targetHealth , const ScanSettings& settings ) noexcept -> bool
		{
			if ( damage < static_cast<float>( targetHealth ) )
				return false;

			switch ( settings.hitscanMode )
			{
			case HitscanMode::Lethal:
			case HitscanMode::Lethal2:
			case HitscanMode::Prefer:
				return true;
			default:
				return false;
			}
		}

		auto ScoreCandidate( const PointResult& candidate , const ScanSettings& settings , TargetSelect select ) noexcept -> float
		{
			float score = candidate.damage;

			if ( candidate.lethal && settings.hitscanMode != HitscanMode::Normal )
				score += 1000.f;

			if ( candidate.safe && settings.safePoint )
				score += 50.f;

			switch ( select )
			{
			case TargetSelect::Fov:
				return score - candidate.fov * 2.f;
			case TargetSelect::Distance:
				return score - candidate.distance * 0.05f;
			default:
				return score;
			}
		}

		auto CollectHitboxes( const ScanSettings& settings , std::vector<uint32_t>& out ) noexcept -> void
		{
			out.clear();
			if ( settings.hitboxHead )
				out.push_back( HitboxData::StudioHitbox::Head );
			if ( settings.hitboxNeck )
				out.push_back( HitboxData::StudioHitbox::Neck );
			if ( settings.hitboxChest || settings.hitboxUpperChest )
			{
				out.push_back( HitboxData::StudioHitbox::Chest );
				out.push_back( HitboxData::StudioHitbox::RightChest );
				out.push_back( HitboxData::StudioHitbox::LeftChest );
			}
			if ( settings.hitboxPelvis )
				out.push_back( HitboxData::StudioHitbox::Pelvis );
			if ( settings.hitboxStomach )
			{
				out.push_back( HitboxData::StudioHitbox::Stomach );
				out.push_back( HitboxData::StudioHitbox::Center );
			}
			if ( settings.hitboxLegs )
			{
				out.push_back( HitboxData::StudioHitbox::LeftLeg );
				out.push_back( HitboxData::StudioHitbox::RightLeg );
			}
			if ( settings.hitboxFeet )
			{
				out.push_back( HitboxData::StudioHitbox::LeftFeet );
				out.push_back( HitboxData::StudioHitbox::RightFeet );
			}
			if ( settings.hitboxArms )
			{
				out.push_back( HitboxData::StudioHitbox::LeftArm );
				out.push_back( HitboxData::StudioHitbox::RightArm );
			}
			if ( out.empty() )
				out.push_back( HitboxData::StudioHitbox::Head );
		}
	}

	auto ApplyAdaptiveWeapon( ScanSettings& settings ) noexcept -> void
	{
		auto* vdata = GetCL_Weapons()->GetLocalWeaponVData();
		const int defIndex = GetCL_Weapons()->GetLocalWeaponDefinitionIndex();

		if ( !vdata )
			return;

		const auto weaponType = vdata->m_WeaponType().m_Type;

		if ( defIndex == 9 ) // AWP
		{
			settings.hitchance = std::max( settings.hitchance , 60 );
			settings.minDamage = std::max( settings.minDamage , 80 );
			settings.hitboxHead = true;
			settings.hitboxChest = false;
		}
		else if ( defIndex == 40 ) // SSG08
		{
			settings.hitchance = std::max( settings.hitchance , 55 );
			settings.hitboxNeck = true;
			settings.hitboxChest = true;
		}
		else if ( defIndex == 1 || defIndex == 64 ) // Deagle / R8
		{
			settings.hitchance = std::max( settings.hitchance , 45 );
			settings.hitboxHead = true;
			settings.hitboxChest = true;
		}
		else if ( weaponType == CSWeaponType_t::WEAPONTYPE_PISTOL )
		{
			settings.multipointScale = std::min( settings.multipointScale , 60 );
		}
		else if ( weaponType == CSWeaponType_t::WEAPONTYPE_SNIPER_RIFLE )
		{
			settings.hitchance = std::max( settings.hitchance , 50 );
		}
		else if ( weaponType == CSWeaponType_t::WEAPONTYPE_RIFLE )
		{
			settings.hitboxChest = true;
			settings.hitboxHead = true;
		}
	}

	auto EvaluatePoint(
		const ::Vector3& eye ,
		const ::Vector3& point ,
		uint32_t studioHitbox ,
		int targetHealth ,
		C_CSPlayerPawn* targetPawn ,
		uintptr_t targetAddr ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		const ScanSettings& settings ,
		PointResult& out ) noexcept -> bool
	{
		const Vector3 sdkEye( eye.m_x , eye.m_y , eye.m_z );
		::Vector3 sdkPoint( point.m_x , point.m_y , point.m_z );

		out.usedBacktrack = false;
		out.backtrackTick = 0;
		out.backtrackFraction = 0.f;
		out.backtrackSimTime = 0.f;

		sdkPoint = ResolveBacktrackPoint( targetPawn , sdkEye , sdkPoint , studioHitbox , settings.backtrack , out );

		AutoWall::PenetrationResult pen{};
		const bool visible = Trace::IsVisible(
			{ sdkEye.x , sdkEye.y , sdkEye.z } ,
			{ sdkPoint.m_x , sdkPoint.m_y , sdkPoint.m_z } ,
			localPawn ,
			localHandle ,
			targetAddr );

		if ( settings.penetration )
		{
			pen = AutoWall::CanPenetrate(
				{ sdkEye.x , sdkEye.y , sdkEye.z } ,
				{ sdkPoint.m_x , sdkPoint.m_y , sdkPoint.m_z } ,
				targetPawn ,
				nullptr );
		}
		else if ( visible )
		{
			pen.canHit = true;
			auto* wpnData = GetCL_Weapons()->GetLocalWeaponVData();
			pen.damage = wpnData ? static_cast<float>( wpnData->m_nDamage() ) : kDefaultWeaponDamage;
			if ( wpnData )
			{
				float scaled = pen.damage;
				AutoWall::ScaleDamage( scaled , HitboxToHitGroup( studioHitbox ) , targetPawn , wpnData );
				pen.damage = scaled;
			}
		}

		if ( !pen.canHit || !PassesMinDamage( pen.damage , targetHealth , settings ) )
			return false;

		if ( settings.safePoint && !visible )
			return false;

		const ::Vector3 finalPos = sdkPoint;
		const auto aim = Aimbot::CalcAngle( sdkEye , Vector3( finalPos.m_x , finalPos.m_y , finalPos.m_z ) );
		out.pawn = targetAddr;
		out.pos = { finalPos.m_x , finalPos.m_y , finalPos.m_z };
		out.angle = aim;
		out.damage = pen.damage;
		out.hitbox = static_cast<int>( studioHitbox );
		out.studioHitbox = studioHitbox;
		out.safe = settings.safePoint
			? PassesSafePoint(
				sdkEye ,
				HitboxData::Sample{ finalPos , 4.f , studioHitbox , true } ,
				settings.multipointScale ,
				localPawn ,
				localHandle ,
				targetAddr )
			: visible;
		out.lethal = IsLethalShot( pen.damage , targetHealth , settings );
		return out.safe || !settings.safePoint;
	}

	auto ScanBestTarget(
		const ::Vector3& eye ,
		const Game::QAngle& viewAngles ,
		const ScanSettings& settings ,
		bool teamCheck ,
		TargetSelect targetSelect ,
		PointResult& best ) noexcept -> bool
	{
		uintptr_t localCtrl = Game::Read<uintptr_t>( Game::clientBase + Offsets::dwLocalPlayerController );
		if ( !localCtrl )
			return false;

		uint32_t localHandle = Game::Read<uint32_t>( localCtrl + Offsets::m_hPlayerPawn );
		uintptr_t localPawn = Game::GetEntityByHandle( localHandle );
		if ( !localPawn )
			return false;

		const int localTeam = Game::Read<uint8_t>( localPawn + Offsets::m_iTeamNum );
		uintptr_t entList = Game::Read<uintptr_t>( Game::clientBase + Offsets::dwEntityList );
		if ( !entList )
			return false;

		std::vector<uint32_t> hitboxes;
		CollectHitboxes( settings , hitboxes );

		float bestScore = -1.f;
		bool found = false;
		PointResult stickyResult{};
		float stickyScore = -1.f;
		bool stickyValid = false;

		for ( int i = 1; i <= 64; ++i )
		{
			uintptr_t listEntry = Game::Read<uintptr_t>( entList + 0x8 * ( i >> 9 ) + 0x10 );
			if ( !listEntry )
				continue;

			uintptr_t ctrl = Game::Read<uintptr_t>( listEntry + Game::ENTITY_IDENTITY_SIZE * ( i & 0x1FF ) );
			if ( !ctrl )
				continue;

			uint32_t pH = Game::Read<uint32_t>( ctrl + Offsets::m_hPlayerPawn );
			if ( !pH || pH == 0xFFFFFFFF )
				continue;

			uintptr_t pawn = Game::GetEntityByHandle( pH );
			if ( !pawn || pawn == localPawn )
				continue;

			const int health = Game::Read<int32_t>( pawn + Offsets::m_iHealth );
			const uint8_t lifeState = Game::Read<uint8_t>( pawn + Offsets::m_lifeState );
			if ( health <= 0 || lifeState != 0 )
				continue;

			const int team = Game::Read<uint8_t>( pawn + Offsets::m_iTeamNum );
			if ( teamCheck && team == localTeam )
				continue;

			auto* sdkPawn = reinterpret_cast<C_CSPlayerPawn*>( pawn );
			if ( !IsValidTarget( sdkPawn , pawn ) )
				continue;

			for ( uint32_t studioHitbox : hitboxes )
			{
				const HitboxData::Sample hitbox = HitboxData::CalculateHitbox( sdkPawn , studioHitbox , false );
				if ( !hitbox.valid )
					continue;

				std::vector<::Vector3> points;
				if ( settings.scanMode == ScanMode::Adaptive && settings.multipointScale > 0 )
					HitboxData::BuildMultipoints( hitbox , settings.multipointScale , points );
				else
					points.push_back( hitbox.position );

				for ( const auto& sample : points )
				{
					const Vector3 bonePos{ sample.m_x , sample.m_y , sample.m_z };
					const Vector3 delta = bonePos - Vector3( eye.m_x , eye.m_y , eye.m_z );
					const float distSq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
					if ( distSq > kMaxScanDistance * kMaxScanDistance )
						continue;

					PointResult candidate{};
					if ( !EvaluatePoint(
						eye ,
						sample ,
						studioHitbox ,
						health ,
						sdkPawn ,
						pawn ,
						localPawn ,
						localHandle ,
						settings ,
						candidate ) )
					{
						continue;
					}

					candidate.distance = std::sqrt( distSq );
					candidate.fov = Aimbot::GetFOV( viewAngles , candidate.angle );

					const float score = ScoreCandidate( candidate , settings , targetSelect );
					if ( pawn == s_lastTarget )
					{
						stickyResult = candidate;
						stickyScore = score + 0.01f;
						stickyValid = true;
					}

					if ( score > bestScore )
					{
						bestScore = score;
						best = candidate;
						found = true;
					}
				}
			}
		}

		if ( stickyValid && stickyScore >= bestScore * 0.85f )
		{
			best = stickyResult;
			s_lastTarget = stickyResult.pawn;
			return true;
		}

		if ( found )
			s_lastTarget = best.pawn;
		else
			s_lastTarget = 0;

		return found;
	}

	auto PassesHitchance(
		const Game::QAngle& aimAngles ,
		const Game::Vector3& targetPoint ,
		CUserCmd* cmd ,
		C_CSPlayerPawn* targetPawn ,
		uintptr_t targetAddr ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		int hitchancePercent ) noexcept -> bool
	{
		(void)targetPawn;
		(void)cmd;

		if ( hitchancePercent <= 0 )
			return true;
		if ( hitchancePercent >= 100 )
			return true;

		auto* weapon = GetCL_Weapons()->GetLocalActiveWeapon();
		if ( !weapon || !localPawn || !targetAddr )
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

		::QAngle sdkAim( aimAngles.pitch , aimAngles.yaw , 0.f );

		const ::Vector3 start = GetCL_Players()->GetLocalEyeOrigin();
		const Game::Vector3 traceStart{ start.m_x , start.m_y , start.m_z };

		const float targetDist = start.Distance( ::Vector3( targetPoint.x , targetPoint.y , targetPoint.z ) );
		if ( targetDist <= 1.f || targetDist > weaponRange )
			return targetDist <= 1.f;

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
			Math::AngleVectors( spreadView , spreadDir );
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

#include "Triggerbot.hpp"

#include <chrono>

#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>

#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Features/Combat/Legit/LegitScan.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Game/Trace.hpp>
#include <Client/Utils/KeyBindUtils.hpp>

#include <GameClient/CL_Bypass.hpp>
#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Weapons.hpp>

namespace Triggerbot
{
	namespace
	{
		std::chrono::steady_clock::time_point s_lastActionTime{};
		bool s_wasShooting = false;

		auto ResolveCrosshairPawn( int crosshairId ) -> uintptr_t
		{
			if ( crosshairId <= 0 )
				return 0;

			const uintptr_t crosshairEnt = Game::GetEntityByHandle( crosshairId );
			if ( !crosshairEnt )
				return 0;

			uintptr_t crosshairPawn = crosshairEnt;
			const uint32_t pawnHandle = Game::Read<uint32_t>( crosshairEnt + Offsets::m_hPlayerPawn );
			if ( pawnHandle != 0 && pawnHandle != 0xFFFFFFFF )
			{
				const uintptr_t actualPawn = Game::GetEntityByHandle( pawnHandle );
				if ( actualPawn )
					crosshairPawn = actualPawn;
			}

			return crosshairPawn;
		}

		auto GetTriggerPoint( C_CSPlayerPawn* pawn ) -> Game::Vector3
		{
			if ( !pawn )
				return {};

			const ::Vector3 head = LegitScan::GetHitboxPosition( pawn , HitboxData::StudioHitbox::Head );
			if ( !head.IsZero() )
				return { head.m_x , head.m_y , head.m_z };

			const ::Vector3 chest = LegitScan::GetHitboxPosition( pawn , HitboxData::StudioHitbox::Chest );
			if ( !chest.IsZero() )
				return { chest.m_x , chest.m_y , chest.m_z };

			const auto origin = pawn->GetOrigin();
			return { origin.m_x , origin.m_y , origin.m_z + 64.f };
		}
	}

	auto Tick(
		CUserCmd* cmd ,
		uintptr_t localPawn ,
		uint32_t localHandle ,
		const Game::QAngle& viewAngles ) -> void
	{
		if ( !config.enabled || !cmd || !localPawn || !Game::clientBase )
		{
			s_wasShooting = false;
			return;
		}

		bool shouldFire = false;
		uintptr_t targetPawn = 0;
		Game::Vector3 targetPoint{};

		if ( !config.useKey || KeyBindUtils::IsActive( static_cast<unsigned int>( config.key ) ) )
		{
			const int crosshairId = Game::Read<int>( localPawn + Offsets::m_iIDEntIndex );
			targetPawn = ResolveCrosshairPawn( crosshairId );
			if ( targetPawn && targetPawn != localPawn )
			{
				const int health = Game::Read<int32_t>( targetPawn + Offsets::m_iHealth );
				const uint8_t lifeState = Game::Read<uint8_t>( targetPawn + Offsets::m_lifeState );
				if ( health > 0 && lifeState == 0 )
				{
					bool isEnemy = true;
					if ( config.teamCheck )
					{
						const int myTeam = Game::Read<uint8_t>( localPawn + Offsets::m_iTeamNum );
						const int theirTeam = Game::Read<uint8_t>( targetPawn + Offsets::m_iTeamNum );
						if ( myTeam == theirTeam )
							isEnemy = false;
					}

					auto* sdkPawn = reinterpret_cast<C_CSPlayerPawn*>( targetPawn );
					if ( isEnemy && LegitScan::IsValidTarget( sdkPawn ) )
					{
						const ::Vector3 sdkEyePos = GetCL_Players()->GetLocalEyeOrigin();
						const Game::Vector3 eye{ sdkEyePos.m_x , sdkEyePos.m_y , sdkEyePos.m_z };
						targetPoint = GetTriggerPoint( sdkPawn );

						bool visible = true;
						if ( config.visCheck )
						{
							const ::Vector3 sdkEye( eye.x , eye.y , eye.z );
							const ::Vector3 sdkTarget( targetPoint.x , targetPoint.y , targetPoint.z );
							visible = !LegitScan::IsSmokeBetween( sdkEye , sdkTarget )
								&& Trace::IsVisible( eye , targetPoint , localPawn , localHandle , targetPawn );
						}

						if ( visible )
						{
							if ( config.hitchance <= 0
								|| LegitScan::PassesHitchance(
									viewAngles ,
									targetPoint ,
									cmd ,
									targetPawn ,
									localPawn ,
									localHandle ,
									config.hitchance ) )
							{
								shouldFire = true;
							}
						}
					}
				}
			}
		}

		const auto now = std::chrono::steady_clock::now();

		if ( shouldFire
			&& std::chrono::duration_cast<std::chrono::milliseconds>( now - s_lastActionTime ).count() > config.delayMs )
		{
			auto* localSdkPawn = GetCL_Players()->GetLocalPlayerPawn();
			auto* weapon = GetCL_Weapons()->GetLocalActiveWeapon();
			if ( LegitScan::CanShoot( localSdkPawn , weapon ) )
			{
				GetCL_Bypass()->SetAttack( cmd , true );
				s_lastActionTime = now;
				s_wasShooting = true;
			}
			else if ( s_wasShooting )
			{
				GetCL_Bypass()->SetDontAttack( cmd , true );
				s_wasShooting = false;
			}
		}
		else if ( s_wasShooting && !shouldFire )
		{
			GetCL_Bypass()->SetDontAttack( cmd , true );
			s_wasShooting = false;
		}
	}
}

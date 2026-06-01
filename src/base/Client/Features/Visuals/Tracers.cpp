#include "Tracers.hpp"

#include <algorithm>

#include <CS2/SDK/SDK.hpp>

#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Game/Trace.hpp>

#include <GameClient/CL_Players.hpp>

namespace BulletTracer
{
	namespace
	{
		auto ResolveTraceEnd(
			const Vector3& start ,
			const float dir[3] ,
			uintptr_t localPawn ,
			uint32_t localHandle ) noexcept -> Vector3
		{
			const float range = config.rayLength > 0.f ? config.rayLength : 8192.f;
			const Game::Vector3 traceStart{ start.m_x , start.m_y , start.m_z };
			const Game::Vector3 traceEnd{
				start.m_x + dir[0] * range ,
				start.m_y + dir[1] * range ,
				start.m_z + dir[2] * range ,
			};

			float fraction = 1.f;
			if ( localPawn && ::Trace::RunTrace(
				traceStart ,
				traceEnd ,
				localPawn ,
				localHandle ,
				::Trace::kShotFilterProfile ,
				fraction ) )
			{
				const float hitDist = range * std::clamp( fraction , 0.f , 1.f );
				return Vector3{
					start.m_x + dir[0] * hitDist ,
					start.m_y + dir[1] * hitDist ,
					start.m_z + dir[2] * hitDist ,
				};
			}

			return Vector3{
				traceEnd.x ,
				traceEnd.y ,
				traceEnd.z ,
			};
		}
	}

	auto AddTrace( const Vector3& start , const Vector3& end ) noexcept -> void
	{
		if ( !config.enabled )
			return;

		Trace trace{};
		trace.startPos[0] = start.m_x;
		trace.startPos[1] = start.m_y;
		trace.startPos[2] = start.m_z;
		trace.endPos[0] = end.m_x;
		trace.endPos[1] = end.m_y;
		trace.endPos[2] = end.m_z;
		trace.spawnTime = GetTime();

		const float dx = end.m_x - start.m_x;
		const float dy = end.m_y - start.m_y;
		const float dz = end.m_z - start.m_z;
		trace.totalDist = std::sqrt( dx * dx + dy * dy + dz * dz );

		std::lock_guard<std::mutex> lock( traceMutex );
		traces.push_back( trace );
	}

	auto OnCreateMove() noexcept -> void
	{
		if ( !config.enabled || !Game::clientBase )
			return;

		if ( !DetectShot() )
			return;

		auto* controller = GetCL_Players()->GetLocalPlayerController();
		auto* pawn = GetCL_Players()->GetLocalPlayerPawn();
		if ( !controller || !pawn )
			return;

		const Vector3 eye = GetCL_Players()->GetLocalEyeOrigin();
		if ( eye.IsZero() )
			return;

		const Game::QAngle viewAngles = Game::Read<Game::QAngle>( Game::clientBase + Offsets::dwViewAngles );
		float dir[3]{};
		AngleToDir( viewAngles.pitch , viewAngles.yaw , dir );

		uint32_t localHandle = 0;
		if ( controller->pEntityIdentity() )
			localHandle = controller->pEntityIdentity()->Handle().m_Index;

		const Vector3 end = ResolveTraceEnd(
			eye ,
			dir ,
			reinterpret_cast<uintptr_t>( pawn ) ,
			localHandle );

		AddTrace( eye , end );
	}
}

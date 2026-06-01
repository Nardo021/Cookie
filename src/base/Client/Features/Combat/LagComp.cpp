#include "LagComp.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <limits>
#include <unordered_map>

#include <ImGui/imgui.h>

#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>
#include <CS2/SDK/SDK.hpp>

#include <Client/Core/CConVars.hpp>
#include <Client/Features/Combat/HitboxData.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>

#include <GameClient/CL_Players.hpp>

namespace LagComp
{
	namespace
	{
		using RecordDeque = std::deque<LagRecord>;

		std::unordered_map<uint32_t , RecordDeque> s_recordsByHandle;

		struct BoneConnection { int bone1; int bone2; };
		constexpr BoneConnection kDebugBones[] = {
			{ 6 , 5 } , { 5 , 4 } , { 4 , 0 } ,
			{ 5 , 8 } , { 8 , 9 } , { 9 , 10 } ,
			{ 5 , 13 } , { 13 , 14 } , { 14 , 15 } ,
			{ 0 , 22 } , { 22 , 23 } , { 23 , 24 } ,
			{ 0 , 25 } , { 25 , 26 } , { 26 , 27 } ,
		};

		auto PushRecord( uint32_t handle , LagRecord record ) noexcept -> void
		{
			auto& deque = s_recordsByHandle[handle];
			deque.push_front( record );

			const int maxRecords = GetMaxRecordCount();
			while ( static_cast<int>( deque.size() ) > maxRecords )
				deque.pop_back();

			const float minSimTime = GetLastValidSimTime();
			while ( !deque.empty() && deque.back().simulationTime < minSimTime )
				deque.pop_back();
		}

		auto ResolveHandle( C_CSPlayerPawn* pawn ) noexcept -> uint32_t
		{
			if ( !pawn || !pawn->pEntityIdentity() )
				return 0;

			return pawn->pEntityIdentity()->Handle().m_Index;
		}

		auto FindRecordDeque( C_CSPlayerPawn* pawn ) -> RecordDeque*
		{
			const uint32_t handle = ResolveHandle( pawn );
			if ( !handle )
				return nullptr;

			const auto it = s_recordsByHandle.find( handle );
			if ( it == s_recordsByHandle.end() || it->second.empty() )
				return nullptr;

			return &it->second;
		}

		auto FindRecordDequeByHandle( uint32_t handle ) -> RecordDeque*
		{
			if ( !handle )
				return nullptr;

			const auto it = s_recordsByHandle.find( handle );
			if ( it == s_recordsByHandle.end() || it->second.empty() )
				return nullptr;

			return &it->second;
		}

		auto FindBracketingRecords( const RecordDeque& deque , int tick , const LagRecord*& older , const LagRecord*& newer , float& fraction ) noexcept -> bool
		{
			older = nullptr;
			newer = nullptr;

			for ( const auto& record : deque )
			{
				if ( record.tick <= tick && ( !older || record.tick > older->tick ) )
					older = &record;

				if ( record.tick >= tick && ( !newer || record.tick < newer->tick ) )
					newer = &record;
			}

			if ( !older && !newer )
				return false;

			if ( !older )
			{
				older = newer;
				fraction = 0.f;
				return true;
			}

			if ( !newer || older == newer )
			{
				newer = older;
				fraction = 0.f;
				return true;
			}

			const int span = newer->tick - older->tick;
			fraction = span > 0
				? static_cast<float>( tick - older->tick ) / static_cast<float>( span )
				: 0.f;

			fraction = std::clamp( fraction , 0.f , 1.f );
			return true;
		}

		auto LerpVector( const Vector3& a , const Vector3& b , float t ) noexcept -> Vector3
		{
			return Vector3{
				a.m_x + ( b.m_x - a.m_x ) * t ,
				a.m_y + ( b.m_y - a.m_y ) * t ,
				a.m_z + ( b.m_z - a.m_z ) * t ,
			};
		}

		auto GetTickInterval() noexcept -> float
		{
			if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
			{
				const float interval = globalVars->m_flIntervalPerSubTick();
				if ( interval > 0.f )
					return interval;
			}

			return 1.f / 64.f;
		}

		inline constexpr uint32_t kBacktrackHitboxes[] = {
			HitboxData::StudioHitbox::Head ,
			HitboxData::StudioHitbox::Neck ,
			HitboxData::StudioHitbox::Chest ,
			HitboxData::StudioHitbox::RightChest ,
			HitboxData::StudioHitbox::LeftChest ,
			HitboxData::StudioHitbox::Stomach ,
			HitboxData::StudioHitbox::Pelvis ,
			HitboxData::StudioHitbox::Center ,
			HitboxData::StudioHitbox::LeftLeg ,
			HitboxData::StudioHitbox::RightLeg ,
			HitboxData::StudioHitbox::LeftFeet ,
			HitboxData::StudioHitbox::RightFeet ,
			HitboxData::StudioHitbox::LeftArm ,
			HitboxData::StudioHitbox::RightArm ,
		};

		inline constexpr int kBacktrackHitboxCount = static_cast<int>( sizeof( kBacktrackHitboxes ) / sizeof( kBacktrackHitboxes[0] ) );
		inline constexpr float kMaxBacktrackSegmentDist = 100.f;

		auto GetRecordHitboxPos( const LagRecord& record , uint32_t studioIndex ) noexcept -> Vector3
		{
			if ( studioIndex < HitboxData::kMaxStudioHitboxes && record.hitboxValid[studioIndex] )
				return record.hitboxPositions[studioIndex];

			return {};
		}

		auto RecordSegmentDist(
			const Vector3& segStart ,
			const Vector3& segEnd ,
			const LagRecord& record ,
			uint32_t studioIndex ) noexcept -> float
		{
			const Vector3 hitPos = GetRecordHitboxPos( record , studioIndex );
			if ( hitPos.IsZero() )
				return std::numeric_limits<float>::max();

			const uint32_t parentIndex = record.hitboxParentValid[studioIndex]
				? record.hitboxParent[studioIndex]
				: HitboxData::GetParentStudioIndex( studioIndex );
			const Vector3 parentPos = GetRecordHitboxPos( record , parentIndex );
			if ( parentPos.IsZero() )
				return HitboxData::SegmentDistToPoint( segStart , segEnd , hitPos );

			return HitboxData::SegmentDistToSegment( segStart , segEnd , hitPos , parentPos );
		}

		auto PingFallbackLatency() noexcept -> float
		{
			auto* controller = GetCL_Players()->GetLocalPlayerController();
			if ( !controller )
				return 0.f;

			return static_cast<float>( controller->m_iPing() ) / 1000.f;
		}
	}

	auto GetInterpTime() noexcept -> float
	{
		float interp = kDefaultInterp;

		if ( CookieCore::CConVars::cl_interp )
			interp = CookieCore::CConVars::cl_interp->value.fl;

		if ( CookieCore::CConVars::cl_interp_ratio )
		{
			const float ratio = CookieCore::CConVars::cl_interp_ratio->value.fl;
			const float updaterate = CookieCore::CConVars::cl_updaterate
				? CookieCore::CConVars::cl_updaterate->value.fl
				: 64.f;

			if ( updaterate > 0.f )
				interp = std::max( interp , ratio / updaterate );
		}

		return interp;
	}

	auto GetMaxUnlagTime() noexcept -> float
	{
		if ( CookieCore::CConVars::sv_maxunlag )
		{
			const float unlag = CookieCore::CConVars::sv_maxunlag->value.fl;
			if ( unlag > 0.f )
				return unlag;
		}

		return kFallbackMaxUnlag;
	}

	auto GetNetworkLatency() noexcept -> float
	{
		auto* engine = SDK::Interfaces::EngineToClient();
		if ( !engine )
			return PingFallbackLatency();

		auto* netChannel = engine->GetNetChannelInfo( 0 );
		if ( !netChannel )
			return PingFallbackLatency();

		const float latency = netChannel->GetLatency( NetFlow::Outgoing )
			+ netChannel->GetLatency( NetFlow::Incoming );

		if ( latency > 0.f )
			return latency;

		return PingFallbackLatency();
	}

	auto GetLatencyTime() noexcept -> float
	{
		return GetNetworkLatency();
	}

	auto GetLastValidSimTime() noexcept -> float
	{
		auto* globalVars = SDK::Pointers::GlobalVarsBase();
		if ( !globalVars )
			return 0.f;

		const float svMaxUnlag = GetMaxUnlagTime();
		const float latency = GetNetworkLatency();
		const float correct = std::clamp( latency , 0.f , svMaxUnlag );
		const float maxDelta = std::min( svMaxUnlag - correct , kFallbackMaxUnlag );

		return globalVars->m_flCurtime() - maxDelta;
	}

	auto GetMaxRecordCount() noexcept -> int
	{
		const float interval = GetTickInterval();
		if ( interval <= 0.f )
			return kMaxRecords;

		const int ticks = static_cast<int>( std::ceil( GetMaxUnlagTime() / interval ) );
		return std::clamp( ticks , 1 , kMaxRecords );
	}

	auto GetBacktrackTicks() noexcept -> int
	{
		const float totalTime = std::min( GetNetworkLatency() + GetInterpTime() , GetMaxUnlagTime() );
		const float interval = GetTickInterval();
		if ( interval <= 0.f )
			return 0;

		const int ticks = static_cast<int>( std::floor( totalTime / interval + 0.5f ) );
		return std::clamp( ticks , 0 , GetMaxRecordCount() - 1 );
	}

	auto GetBacktrackTick( int currentTick ) noexcept -> int
	{
		return std::max( 0 , currentTick - GetBacktrackTicks() );
	}

	auto RecordPlayers() noexcept -> void
	{
		if ( !Game::clientBase )
			return;

		auto* globalVars = SDK::Pointers::GlobalVarsBase();
		if ( !globalVars )
			return;

		uintptr_t localCtrl = Game::Read<uintptr_t>( Game::clientBase + Offsets::dwLocalPlayerController );
		if ( !localCtrl )
			return;

		uint32_t localHandle = Game::Read<uint32_t>( localCtrl + Offsets::m_hPlayerPawn );
		uintptr_t localPawn = Game::GetEntityByHandle( localHandle );

		uintptr_t entityList = Game::GetEntityList();
		if ( !entityList )
			return;

		const int tick = globalVars->m_nTickCount();

		for ( int i = 1; i <= 64; ++i )
		{
			uintptr_t listEntry = Game::Read<uintptr_t>( entityList + ( 8 * ( i >> 9 ) ) + 0x10 );
			if ( !listEntry )
				continue;

			uintptr_t controller = Game::Read<uintptr_t>( listEntry + Game::ENTITY_IDENTITY_SIZE * ( i & 0x1FF ) );
			if ( !controller || controller == localCtrl )
				continue;

			if ( !Game::Read<bool>( controller + Offsets::m_bPawnIsAlive ) )
				continue;

			uint32_t pawnHandle = Game::Read<uint32_t>( controller + Offsets::m_hPlayerPawn );
			if ( !pawnHandle )
				continue;

			uintptr_t pawnAddr = Game::GetEntityByHandle( pawnHandle );
			if ( !pawnAddr || pawnAddr == localPawn )
				continue;

			auto* pawn = reinterpret_cast<C_CSPlayerPawn*>( pawnAddr );
			if ( !pawn->IsAlive() )
				continue;

			LagRecord record{};
			record.tick = tick;
			record.simulationTime = Game::Read<float>( pawnAddr + Offsets::m_flSimulationTime );
			record.origin = pawn->GetOrigin();
			record.hasBones = false;

			for ( int bone = 0; bone < 128; ++bone )
			{
				const Game::Vector3 bonePos = Game::GetBonePosition( pawnAddr , bone );
				if ( bonePos.IsZero() )
					continue;

				record.bones[bone] = Vector3( bonePos.x , bonePos.y , bonePos.z );
				record.hasBones = true;
			}

			for ( int hb = 0; hb < kBacktrackHitboxCount; ++hb )
			{
				const uint32_t studioIndex = kBacktrackHitboxes[hb];
				const auto sample = HitboxData::CalculateHitbox( pawn , studioIndex , false );
				if ( !sample.valid || studioIndex >= HitboxData::kMaxStudioHitboxes )
					continue;

				record.hitboxPositions[studioIndex] = sample.position;
				record.hitboxValid[studioIndex] = true;
				record.hitboxParent[studioIndex] = HitboxData::GetParentStudioIndex( pawn , studioIndex );
				record.hitboxParentValid[studioIndex] = true;
				record.hasHitboxes = true;
			}

			const uint32_t recordHandle = ResolveHandle( pawn );
			if ( !recordHandle )
				continue;

			PushRecord( recordHandle , record );
		}
	}

	auto GetBacktrackBoneInterpolated( C_CSPlayerPawn* pawn , int tick , int boneIndex ) noexcept -> Vector3
	{
		if ( !pawn || boneIndex < 0 || boneIndex >= 128 )
			return {};

		auto* deque = FindRecordDeque( pawn );
		if ( !deque )
			return {};

		const LagRecord* older = nullptr;
		const LagRecord* newer = nullptr;
		float fraction = 0.f;

		if ( !FindBracketingRecords( *deque , tick , older , newer , fraction ) )
			return {};

		if ( !older->hasBones && !newer->hasBones )
			return GetBacktrackOriginInterpolated( pawn , tick );

		if ( !older->hasBones )
			return newer->bones[boneIndex];

		if ( !newer->hasBones || older == newer )
			return older->bones[boneIndex];

		return LerpVector( older->bones[boneIndex] , newer->bones[boneIndex] , fraction );
	}

	auto GetBacktrackOriginInterpolated( C_CSPlayerPawn* pawn , int tick ) noexcept -> Vector3
	{
		if ( !pawn )
			return {};

		auto* deque = FindRecordDeque( pawn );
		if ( !deque )
			return pawn->GetOrigin();

		const LagRecord* older = nullptr;
		const LagRecord* newer = nullptr;
		float fraction = 0.f;

		if ( !FindBracketingRecords( *deque , tick , older , newer , fraction ) )
			return pawn->GetOrigin();

		if ( older == newer )
			return older->origin;

		return LerpVector( older->origin , newer->origin , fraction );
	}

	auto GetBacktrackPos( C_CSPlayerPawn* pawn , int tick ) noexcept -> Vector3
	{
		if ( !pawn )
			return {};

		return GetBacktrackOriginInterpolated( pawn , tick );
	}

	auto GetBacktrackBone( C_CSPlayerPawn* pawn , int tick , int boneIndex ) noexcept -> Vector3
	{
		const Vector3 interpolated = GetBacktrackBoneInterpolated( pawn , tick , boneIndex );
		if ( interpolated.m_x != 0.f || interpolated.m_y != 0.f || interpolated.m_z != 0.f )
			return interpolated;

		return {};
	}

	auto GetBacktrackHitboxInterpolated( C_CSPlayerPawn* pawn , int tick , uint32_t studioIndex ) noexcept -> Vector3
	{
		if ( !pawn || studioIndex >= HitboxData::kMaxStudioHitboxes )
			return {};

		auto* deque = FindRecordDeque( pawn );
		if ( !deque )
			return {};

		const LagRecord* older = nullptr;
		const LagRecord* newer = nullptr;
		float fraction = 0.f;

		if ( !FindBracketingRecords( *deque , tick , older , newer , fraction ) )
			return {};

		if ( !older->hitboxValid[studioIndex] && !newer->hitboxValid[studioIndex] )
			return GetBacktrackBoneInterpolated( pawn , tick , pawn->GetBoneIdByName( "head_0" ) );

		if ( !older->hitboxValid[studioIndex] )
			return newer->hitboxPositions[studioIndex];

		if ( !newer->hitboxValid[studioIndex] || older == newer )
			return older->hitboxPositions[studioIndex];

		return LerpVector( older->hitboxPositions[studioIndex] , newer->hitboxPositions[studioIndex] , fraction );
	}

	auto FindBestBacktrack(
		C_CSPlayerPawn* pawn ,
		const Vector3& segStart ,
		const Vector3& segEnd ,
		float minSimTime ) noexcept -> BacktrackSelection
	{
		BacktrackSelection result{};

		auto* deque = FindRecordDeque( pawn );
		if ( !deque || deque->empty() )
			return result;

		int bestIndex = -1;
		uint32_t bestHitbox = HitboxData::StudioHitbox::Head;
		float bestDist = std::numeric_limits<float>::max();

		for ( int i = 0; i < static_cast<int>( deque->size() ); ++i )
		{
			const auto& rec = ( *deque )[static_cast<std::size_t>( i )];
			if ( rec.simulationTime <= minSimTime )
				continue;

			for ( int hb = 0; hb < kBacktrackHitboxCount; ++hb )
			{
				const uint32_t studioIndex = kBacktrackHitboxes[hb];
				const float dist = RecordSegmentDist( segStart , segEnd , rec , studioIndex );
				if ( dist < bestDist )
				{
					bestDist = dist;
					bestIndex = i;
					bestHitbox = studioIndex;
				}
			}
		}

		if ( bestIndex < 0 || bestDist > kMaxBacktrackSegmentDist )
			return result;

		const int prevIndex = bestIndex + 1 < static_cast<int>( deque->size() ) ? bestIndex + 1 : bestIndex;
		const int nextIndex = bestIndex > 0 ? bestIndex - 1 : bestIndex;

		const LagRecord& bestRec = ( *deque )[static_cast<std::size_t>( bestIndex )];
		const LagRecord& prevRec = ( *deque )[static_cast<std::size_t>( prevIndex )];
		const LagRecord& nextRec = ( *deque )[static_cast<std::size_t>( nextIndex )];

		const float prevDist = RecordSegmentDist( segStart , segEnd , prevRec , bestHitbox );
		const float nextDist = RecordSegmentDist( segStart , segEnd , nextRec , bestHitbox );

		const bool prevIsBest = prevDist < nextDist;
		result.recordA = prevIsBest ? &prevRec : &bestRec;
		result.recordB = prevIsBest ? &bestRec : &nextRec;

		if ( result.recordA == result.recordB )
			result.fraction = 0.f;
		else
		{
			const float fullDist = ( prevIsBest ? prevDist : nextDist ) + bestDist;
			result.fraction = fullDist > 0.f
				? ( prevIsBest ? prevDist / fullDist : bestDist / fullDist )
				: 0.f;
		}

		const float interval = GetTickInterval();
		result.simulationTime = result.recordA->simulationTime +
			( result.recordB->simulationTime - result.recordA->simulationTime ) * result.fraction;

		if ( interval > 0.f )
		{
			const float tickTime = result.simulationTime / interval;
			result.tick = static_cast<int>( std::floor( tickTime ) );
			result.tickFraction = tickTime - static_cast<float>( result.tick );
			result.tick = std::max( 0 , result.tick - 1 );
		}
		else
		{
			result.tick = std::max( 0 , bestRec.tick - 1 );
			result.tickFraction = 0.f;
		}

		result.valid = true;
		return result;
	}

	auto GetBacktrackHitbox(
		C_CSPlayerPawn* pawn ,
		const BacktrackSelection& selection ,
		uint32_t studioIndex ) noexcept -> Vector3
	{
		if ( !selection.valid || !selection.recordA || studioIndex >= HitboxData::kMaxStudioHitboxes )
			return {};

		if ( !selection.recordA->hitboxValid[studioIndex] )
			return GetBacktrackHitboxInterpolated( pawn , selection.tick , studioIndex );

		if ( selection.recordB && selection.recordB->hitboxValid[studioIndex] && selection.recordA != selection.recordB )
		{
			return LerpVector(
				selection.recordA->hitboxPositions[studioIndex] ,
				selection.recordB->hitboxPositions[studioIndex] ,
				selection.fraction );
		}

		return selection.recordA->hitboxPositions[studioIndex];
	}

	auto RenderDebug() noexcept -> void
	{
		if ( !debugConfig.drawBacktrack || !Game::clientBase )
			return;

		auto* engine = SDK::Interfaces::EngineToClient();
		if ( !engine || !engine->IsInGame() )
			return;

		auto* globalVars = SDK::Pointers::GlobalVarsBase();
		if ( !globalVars )
			return;

		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		if ( !draw )
			return;

		int screenW = 0;
		int screenH = 0;
		engine->GetScreenSize( screenW , screenH );
		if ( screenW <= 0 || screenH <= 0 )
			return;

		const int targetTick = GetBacktrackTick( globalVars->m_nTickCount() );
		const ImU32 lineColor = IM_COL32(
			static_cast<int>( debugConfig.color[0] * 255.f ) ,
			static_cast<int>( debugConfig.color[1] * 255.f ) ,
			static_cast<int>( debugConfig.color[2] * 255.f ) ,
			static_cast<int>( debugConfig.color[3] * 255.f ) );

		uintptr_t localCtrl = Game::Read<uintptr_t>( Game::clientBase + Offsets::dwLocalPlayerController );
		uint32_t localHandle = localCtrl ? Game::Read<uint32_t>( localCtrl + Offsets::m_hPlayerPawn ) : 0;
		uintptr_t localPawn = localHandle ? Game::GetEntityByHandle( localHandle ) : 0;
		int localTeam = localPawn ? Game::Read<uint8_t>( localPawn + Offsets::m_iTeamNum ) : 0;

		uintptr_t entityList = Game::GetEntityList();
		if ( !entityList )
			return;

		for ( int i = 1; i <= 64; ++i )
		{
			uintptr_t listEntry = Game::Read<uintptr_t>( entityList + ( 8 * ( i >> 9 ) ) + 0x10 );
			if ( !listEntry )
				continue;

			uintptr_t controller = Game::Read<uintptr_t>( listEntry + Game::ENTITY_IDENTITY_SIZE * ( i & 0x1FF ) );
			if ( !controller || controller == localCtrl )
				continue;

			if ( !Game::Read<bool>( controller + Offsets::m_bPawnIsAlive ) )
				continue;

			uint32_t pawnHandle = Game::Read<uint32_t>( controller + Offsets::m_hPlayerPawn );
			if ( !pawnHandle || pawnHandle == localHandle )
				continue;

			uintptr_t pawnAddr = Game::GetEntityByHandle( pawnHandle );
			if ( !pawnAddr )
				continue;

			const int team = Game::Read<uint8_t>( pawnAddr + Offsets::m_iTeamNum );
			if ( team == localTeam )
				continue;

			auto* pawn = reinterpret_cast<C_CSPlayerPawn*>( pawnAddr );
			if ( !pawn->IsAlive() )
				continue;

			auto* deque = FindRecordDeque( pawn );
			if ( !deque )
				continue;

			for ( const auto& connection : kDebugBones )
			{
				const Vector3 boneA = GetBacktrackBoneInterpolated( pawn , targetTick , connection.bone1 );
				const Vector3 boneB = GetBacktrackBoneInterpolated( pawn , targetTick , connection.bone2 );
				if ( boneA.IsZero() || boneB.IsZero() )
					continue;

				float ax = 0.f;
				float ay = 0.f;
				float bx = 0.f;
				float by = 0.f;
				const float posA[3] = { boneA.m_x , boneA.m_y , boneA.m_z };
				const float posB[3] = { boneB.m_x , boneB.m_y , boneB.m_z };

				if ( !Game::WorldToScreen( posA , ax , ay , static_cast<float>( screenW ) , static_cast<float>( screenH ) ) )
					continue;
				if ( !Game::WorldToScreen( posB , bx , by , static_cast<float>( screenW ) , static_cast<float>( screenH ) ) )
					continue;

				draw->AddLine( ImVec2( ax , ay ) , ImVec2( bx , by ) , lineColor , 1.5f );
			}
		}
	}

	auto Clear() noexcept -> void
	{
		s_recordsByHandle.clear();
	}
}

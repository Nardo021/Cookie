#pragma once

#include <CS2/SDK/Math/Vector3.hpp>

#include <Client/Features/Combat/HitboxData.hpp>

class C_CSPlayerPawn;

namespace LagComp
{
	struct LagRecord
	{
		int tick = 0;
		float simulationTime = 0.f;
		Vector3 origin{};
		Vector3 bones[128]{};
		Vector3 hitboxPositions[HitboxData::kMaxStudioHitboxes]{};
		bool hitboxValid[HitboxData::kMaxStudioHitboxes]{};
		uint32_t hitboxParent[HitboxData::kMaxStudioHitboxes]{};
		bool hitboxParentValid[HitboxData::kMaxStudioHitboxes]{};
		bool hasBones = false;
		bool hasHitboxes = false;
	};

	struct BacktrackSelection
	{
		const LagRecord* recordA = nullptr;
		const LagRecord* recordB = nullptr;
		float fraction = 0.f;
		int tick = 0;
		float tickFraction = 0.f;
		float simulationTime = 0.f;
		bool valid = false;
	};

	struct DebugConfig
	{
		bool  drawBacktrack = false;
		float color[4] = { 0.2f , 1.f , 0.55f , 0.85f };
	};

	inline constexpr int kMaxRecords = 32;
	inline constexpr float kDefaultInterp = 0.03125f;
	inline constexpr float kFallbackMaxUnlag = 0.2f;

	inline DebugConfig debugConfig;

	auto RecordPlayers() noexcept -> void;

	auto GetInterpTime() noexcept -> float;
	auto GetNetworkLatency() noexcept -> float;
	auto GetLatencyTime() noexcept -> float;
	auto GetMaxUnlagTime() noexcept -> float;
	auto GetLastValidSimTime() noexcept -> float;
	auto GetMaxRecordCount() noexcept -> int;
	auto GetBacktrackTicks() noexcept -> int;
	auto GetBacktrackTick( int currentTick ) noexcept -> int;

	auto GetBacktrackPos( C_CSPlayerPawn* pawn , int tick ) noexcept -> Vector3;
	auto GetBacktrackBone( C_CSPlayerPawn* pawn , int tick , int boneIndex ) noexcept -> Vector3;
	auto GetBacktrackBoneInterpolated( C_CSPlayerPawn* pawn , int tick , int boneIndex ) noexcept -> Vector3;
	auto GetBacktrackOriginInterpolated( C_CSPlayerPawn* pawn , int tick ) noexcept -> Vector3;

	auto FindBestBacktrack(
		C_CSPlayerPawn* pawn ,
		const Vector3& segStart ,
		const Vector3& segEnd ,
		float minSimTime ) noexcept -> BacktrackSelection;

	auto GetBacktrackHitbox(
		C_CSPlayerPawn* pawn ,
		const BacktrackSelection& selection ,
		uint32_t studioIndex ) noexcept -> Vector3;

	auto GetBacktrackHitboxInterpolated(
		C_CSPlayerPawn* pawn ,
		int tick ,
		uint32_t studioIndex ) noexcept -> Vector3;

	auto RenderDebug() noexcept -> void;
	auto Clear() noexcept -> void;
}

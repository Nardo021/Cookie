#pragma once

#include <cstddef>
#include <cstdint>

#include "Game.hpp"

namespace Trace
{
	struct Ray_t
	{
		Game::Vector3 start;
		Game::Vector3 end;
		Game::Vector3 mins;
		Game::Vector3 maxs;
	};

	using TraceShapeFn = bool ( __fastcall* )(
		void* traceMgr ,
		Ray_t* ray ,
		Game::Vector3* start ,
		Game::Vector3* end ,
		void* filter ,
		void* trace );

	using InitTraceFilterFn = void* ( __fastcall* )(
		void* filter ,
		void* skipEntity ,
		uint64_t mask ,
		int layer ,
		int16_t uk );

	extern TraceShapeFn fnTraceShape;
	extern InitTraceFilterFn fnInitTraceFilter;
	extern void** pTraceManager;
	extern bool ready;

	inline constexpr uint64_t MASK_SHOT = 0x1C3003;
	inline constexpr uint64_t MASK_PLAYERSOLID_BRUSHONLY = 0x1C300B;
	inline constexpr float kVisibleFraction = 0.97f;
	inline constexpr float kEdgeJumpMaxGroundFraction = 0.95f;
	inline constexpr size_t kFilterSize = 0x40;
	inline constexpr size_t kTraceSize = 0xB8;
	inline constexpr std::ptrdiff_t kTraceHitEntity = 0x90;
	inline constexpr std::ptrdiff_t kTraceFraction = 0xAC;
	inline constexpr std::ptrdiff_t kTraceNormalZ = 0x98;

	struct FilterProfile
	{
		uint64_t mask;
		int layer;
		int16_t uk;
		uint8_t collisionGroup;
	};

	inline constexpr FilterProfile kShotFilterProfile{ MASK_SHOT , 4 , 15 , 4 };
	inline constexpr FilterProfile kGroundFilterProfile{ MASK_PLAYERSOLID_BRUSHONLY , 3 , 15 , 3 };

	auto Init() -> bool;

	auto RunTrace(
		const Game::Vector3& start ,
		const Game::Vector3& end ,
		uintptr_t localPawn ,
		uint32_t localPawnHandle ,
		const FilterProfile& profile ,
		float& outFraction ,
		float* outNormalZ = nullptr ,
		uintptr_t* outHitEntity = nullptr ) -> bool;

	inline auto TraceSegment(
		const Game::Vector3& start ,
		const Game::Vector3& end ,
		uintptr_t localPawn ,
		uint32_t localPawnHandle ,
		float& outFraction ,
		float& outNormalZ ) -> bool
	{
		outNormalZ = 0.f;
		return RunTrace( start , end , localPawn , localPawnHandle , kShotFilterProfile , outFraction , &outNormalZ );
	}

	inline auto TraceGround(
		const Game::Vector3& start ,
		const Game::Vector3& end ,
		uintptr_t localPawn ,
		uint32_t localPawnHandle ,
		float& outFraction ) -> bool
	{
		if ( !localPawn )
			return false;

		if ( !RunTrace( start , end , localPawn , localPawnHandle , kGroundFilterProfile , outFraction ) )
			return false;

		return outFraction < 1.f;
	}

	inline auto IsVisible(
		const Game::Vector3& eyePos ,
		const Game::Vector3& targetPos ,
		uintptr_t localPawn ,
		uint32_t localPawnHandle ,
		uintptr_t targetPawn ) -> bool
	{
		float fraction = 1.f;
		uintptr_t hitEntity = 0;

		if ( !RunTrace( eyePos , targetPos , localPawn , localPawnHandle , kShotFilterProfile , fraction , nullptr , &hitEntity ) )
			return false;

		return hitEntity == targetPawn && fraction >= kVisibleFraction;
	}
}

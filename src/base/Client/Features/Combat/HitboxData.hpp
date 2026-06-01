#pragma once

#include <cstdint>
#include <vector>

#include <CS2/SDK/Math/Vector3.hpp>

class C_CSPlayerPawn;

namespace HitboxData
{
	enum class Mode : int
	{
		V1_Bones = 0 ,
		V2_Native = 1 ,
	};

	enum StudioHitbox : uint32_t
	{
		Center = 1 ,
		Pelvis = 2 ,
		Stomach = 3 ,
		Chest = 4 ,
		Neck = 5 ,
		Head = 6 ,
		RightChest = 8 ,
		LeftChest = 13 ,
		LeftLeg = 23 ,
		LeftFeet = 24 ,
		RightLeg = 26 ,
		RightFeet = 27 ,
		LeftArm = 28 ,
		RightArm = 29 ,
	};

	struct Sample
	{
		Vector3 position{};
		float scale = 0.f;
		uint32_t studioIndex = 0;
		bool valid = false;
	};

	struct Config
	{
		Mode mode = Mode::V1_Bones;
	};

	inline Config config;

	inline constexpr uint32_t kTrackedHitboxes[] = {
		StudioHitbox::Head ,
		StudioHitbox::Neck ,
		StudioHitbox::Chest ,
		StudioHitbox::RightChest ,
		StudioHitbox::LeftChest ,
		StudioHitbox::Stomach ,
		StudioHitbox::Pelvis ,
		StudioHitbox::Center ,
		StudioHitbox::LeftLeg ,
		StudioHitbox::RightLeg ,
		StudioHitbox::LeftFeet ,
		StudioHitbox::RightFeet ,
	};

	inline constexpr int kTrackedHitboxCount = static_cast<int>( sizeof( kTrackedHitboxes ) / sizeof( kTrackedHitboxes[0] ) );
	inline constexpr int kMaxStudioHitboxes = 32;

	auto InitNative() noexcept -> void;
	auto IsNativeReady() noexcept -> bool;
	auto GetActiveMode() noexcept -> Mode;

	auto CalculateHitbox( C_CSPlayerPawn* pawn , uint32_t studioIndex , bool predict = false ) noexcept -> Sample;
	auto GetParentStudioIndex( uint32_t studioIndex ) noexcept -> uint32_t;
	auto GetParentStudioIndex( C_CSPlayerPawn* pawn , uint32_t studioIndex ) noexcept -> uint32_t;
	auto StudioToHitGroup( uint32_t studioIndex ) noexcept -> int;
	auto BuildMultipoints( const Sample& hitbox , int scalePercent , std::vector<Vector3>& out ) noexcept -> void;
	auto SegmentDistToPoint( const Vector3& segStart , const Vector3& segEnd , const Vector3& point ) noexcept -> float;
	auto SegmentDistToSegment(
		const Vector3& rayStart ,
		const Vector3& rayEnd ,
		const Vector3& capStart ,
		const Vector3& capEnd ) noexcept -> float;

} // namespace HitboxData

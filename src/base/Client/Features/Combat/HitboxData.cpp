#include "HitboxData.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include <CS2/SDK/Types/CBaseTypes.hpp>
#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>
#include <CS2/SDK/SDK.hpp>

#include <Client/Features/Combat/AutoWall.hpp>
#include <Client/Game/Game.hpp>

namespace HitboxData
{
	namespace
	{
		using FnGetHitboxFlags = uint32_t ( __fastcall* )( void* model , uint32_t index );
		using FnGetHitboxParent = uint32_t ( __fastcall* )( void* model , uint32_t index );
		using FnHitboxToWorldTransforms = void ( __fastcall* )( C_BaseEntity* entity , uint32_t mask );

		FnGetHitboxFlags s_fnGetHitboxFlags = nullptr;
		FnGetHitboxParent s_fnGetHitboxParent = nullptr;
		FnHitboxToWorldTransforms s_fnHitboxToWorldTransforms = nullptr;
		bool s_nativeInitDone = false;
		bool s_nativeReady = false;

		auto ApplyVelocityPredict( C_CSPlayerPawn* pawn , Vector3& position ) noexcept -> void
		{
			if ( !pawn )
				return;

			if ( auto* globalVars = SDK::Pointers::GlobalVarsBase() )
			{
				const Vector3 velocity = pawn->m_vecAbsVelocity();
				const float tickInterval = globalVars->m_flIntervalPerSubTick();
				position.m_x += velocity.m_x * tickInterval;
				position.m_y += velocity.m_y * tickInterval;
				position.m_z += velocity.m_z * tickInterval;
			}
		}

		auto GetModelFromPawn( C_CSPlayerPawn* pawn ) noexcept -> CModel*
		{
			if ( !pawn )
				return nullptr;

			auto* sceneNode = pawn->m_pGameSceneNode();
			if ( !sceneNode )
				return nullptr;

			auto* skeleton = sceneNode->GetSkeletonInstance();
			if ( !skeleton )
				return nullptr;

			auto& modelState = skeleton->m_modelState();
			if ( !modelState.m_hModel().is_valid() )
				return nullptr;

			return static_cast<CModel*>( modelState.m_hModel() );
		}

		auto HasNativeHitbox( CModel* model , uint32_t studioIndex ) noexcept -> bool
		{
			if ( !model || !s_fnGetHitboxFlags || !s_fnGetHitboxParent )
				return false;

			if ( !( s_fnGetHitboxFlags( model , studioIndex ) & FLAG_HITBOX ) )
				return false;

			const int parentIndex = static_cast<int>( s_fnGetHitboxParent( model , studioIndex ) );
			return parentIndex != -1;
		}

		auto ReadHitboxSampleV1( C_CSPlayerPawn* pawn , uint32_t studioIndex , bool predict ) noexcept -> Sample
		{
			Sample sample{};
			sample.studioIndex = studioIndex;

			if ( !pawn || studioIndex >= kMaxStudioHitboxes )
				return sample;

			auto* sceneNode = pawn->m_pGameSceneNode();
			if ( !sceneNode )
				return sample;

			auto* skeleton = sceneNode->GetSkeletonInstance();
			if ( !skeleton )
				return sample;

			skeleton->CalcWorldSpaceBones( FLAG_HITBOX );

			auto& modelState = skeleton->m_modelState();
			if ( !modelState.m_hModel().is_valid() )
				return sample;

			CBoneData* hitboxData = modelState.m_pBones;
			if ( !hitboxData )
				return sample;

			const CBoneData& bone = hitboxData[studioIndex];
			if ( bone.position.IsZero() )
				return sample;

			sample.position = bone.position;
			sample.scale = bone.scale > 0.f ? bone.scale : 4.f;

			if ( predict )
				ApplyVelocityPredict( pawn , sample.position );

			sample.valid = true;
			return sample;
		}

		auto ReadBoneSampleV1( C_CSPlayerPawn* pawn , int boneIndex , uint32_t studioIndex , bool predict ) noexcept -> Sample
		{
			Sample sample{};
			sample.studioIndex = studioIndex;

			if ( !pawn || boneIndex < 0 || boneIndex >= static_cast<int>( kMaxStudioHitboxes ) )
				return sample;

			auto* sceneNode = pawn->m_pGameSceneNode();
			if ( !sceneNode )
				return sample;

			auto* skeleton = sceneNode->GetSkeletonInstance();
			if ( !skeleton )
				return sample;

			skeleton->CalcWorldSpaceBones( FLAG_HITBOX );

			auto& modelState = skeleton->m_modelState();
			if ( !modelState.m_hModel().is_valid() )
				return sample;

			CBoneData* hitboxData = modelState.m_pBones;
			if ( !hitboxData )
				return sample;

			const CBoneData& bone = hitboxData[boneIndex];
			if ( bone.position.IsZero() )
				return sample;

			sample.position = bone.position;
			sample.scale = bone.scale > 0.f ? bone.scale : 4.f;

			if ( predict )
				ApplyVelocityPredict( pawn , sample.position );

			sample.valid = true;
			return sample;
		}

		auto ReadHitboxSampleV2( C_CSPlayerPawn* pawn , uint32_t studioIndex , bool predict ) noexcept -> Sample
		{
			Sample sample{};
			sample.studioIndex = studioIndex;

			if ( !pawn || studioIndex >= kMaxStudioHitboxes || !s_nativeReady )
				return sample;

			auto* sceneNode = pawn->m_pGameSceneNode();
			if ( !sceneNode )
				return sample;

			auto* skeleton = sceneNode->GetSkeletonInstance();
			if ( !skeleton )
				return sample;

			CModel* model = GetModelFromPawn( pawn );
			if ( !model || !HasNativeHitbox( model , studioIndex ) )
				return sample;

			if ( s_fnHitboxToWorldTransforms )
				s_fnHitboxToWorldTransforms( pawn , FLAG_HITBOX );
			else
				skeleton->CalcWorldSpaceBones( FLAG_HITBOX );

			auto& modelState = skeleton->m_modelState();
			CBoneData* hitboxData = modelState.m_pBones;
			if ( !hitboxData )
				return sample;

			const CBoneData& bone = hitboxData[studioIndex];
			if ( bone.position.IsZero() )
				return sample;

			sample.position = bone.position;
			sample.scale = bone.scale > 0.f ? bone.scale : 4.f;

			if ( predict )
				ApplyVelocityPredict( pawn , sample.position );

			sample.valid = true;
			return sample;
		}

		auto CalculateHitboxV1( C_CSPlayerPawn* pawn , uint32_t studioIndex , bool predict ) noexcept -> Sample
		{
			if ( studioIndex == StudioHitbox::LeftArm )
			{
				const int boneIndex = pawn ? pawn->GetBoneIdByName( "arm_lower_l" ) : -1;
				return ReadBoneSampleV1( pawn , boneIndex , studioIndex , predict );
			}

			if ( studioIndex == StudioHitbox::RightArm )
			{
				const int boneIndex = pawn ? pawn->GetBoneIdByName( "arm_lower_r" ) : -1;
				return ReadBoneSampleV1( pawn , boneIndex , studioIndex , predict );
			}

			return ReadHitboxSampleV1( pawn , studioIndex , predict );
		}
	}

	auto InitNative() noexcept -> void
	{
		if ( s_nativeInitDone )
			return;

		s_nativeInitDone = true;

		if ( !Game::clientBase )
			Game::clientBase = Game::GetModuleBase( L"client.dll" );

		if ( !Game::clientBase )
			return;

		const uintptr_t flagsAddr = Game::FindPattern( L"client.dll" , "85 D2 78 16 3B 91" );
		const uintptr_t parentAddr = Game::FindPattern( L"client.dll" , "85 D2 78 17 3B 91 78" );
		const uintptr_t hitboxToWorldCall = Game::FindPattern( L"client.dll" , "E8 ? ? ? ? 4C 8B A3" );

		if ( flagsAddr )
			s_fnGetHitboxFlags = reinterpret_cast<FnGetHitboxFlags>( flagsAddr );
		if ( parentAddr )
			s_fnGetHitboxParent = reinterpret_cast<FnGetHitboxParent>( parentAddr );
		if ( hitboxToWorldCall )
		{
			const uintptr_t resolved = Game::ResolveRelativeAddress( hitboxToWorldCall , 1 , 5 );
			if ( resolved )
				s_fnHitboxToWorldTransforms = reinterpret_cast<FnHitboxToWorldTransforms>( resolved );
		}

		s_nativeReady = s_fnGetHitboxFlags && s_fnGetHitboxParent;
	}

	auto IsNativeReady() noexcept -> bool
	{
		if ( !s_nativeInitDone )
			InitNative();

		return s_nativeReady;
	}

	auto GetActiveMode() noexcept -> Mode
	{
		if ( config.mode == Mode::V2_Native && IsNativeReady() )
			return Mode::V2_Native;

		return Mode::V1_Bones;
	}

	auto CalculateHitbox( C_CSPlayerPawn* pawn , uint32_t studioIndex , bool predict ) noexcept -> Sample
	{
		if ( GetActiveMode() == Mode::V2_Native )
			return ReadHitboxSampleV2( pawn , studioIndex , predict );

		return CalculateHitboxV1( pawn , studioIndex , predict );
	}

	auto GetParentStudioIndex( uint32_t studioIndex ) noexcept -> uint32_t
	{
		switch ( studioIndex )
		{
		case StudioHitbox::Head: return StudioHitbox::Neck;
		case StudioHitbox::Neck: return StudioHitbox::Chest;
		case StudioHitbox::Chest:
		case StudioHitbox::RightChest:
		case StudioHitbox::LeftChest:
			return StudioHitbox::Pelvis;
		case StudioHitbox::Stomach: return StudioHitbox::Pelvis;
		case StudioHitbox::Pelvis: return StudioHitbox::Center;
		case StudioHitbox::LeftLeg:
		case StudioHitbox::RightLeg:
			return StudioHitbox::Pelvis;
		case StudioHitbox::LeftFeet: return StudioHitbox::LeftLeg;
		case StudioHitbox::RightFeet: return StudioHitbox::RightLeg;
		case StudioHitbox::LeftArm: return StudioHitbox::LeftChest;
		case StudioHitbox::RightArm: return StudioHitbox::RightChest;
		default: return StudioHitbox::Center;
		}
	}

	auto GetParentStudioIndex( C_CSPlayerPawn* pawn , uint32_t studioIndex ) noexcept -> uint32_t
	{
		if ( GetActiveMode() == Mode::V2_Native && pawn && s_fnGetHitboxParent )
		{
			if ( CModel* model = GetModelFromPawn( pawn ) )
			{
				if ( HasNativeHitbox( model , studioIndex ) )
				{
					const int parentIndex = static_cast<int>( s_fnGetHitboxParent( model , studioIndex ) );
					if ( parentIndex >= 0 && parentIndex < kMaxStudioHitboxes )
						return static_cast<uint32_t>( parentIndex );
				}
			}
		}

		return GetParentStudioIndex( studioIndex );
	}

	auto StudioToHitGroup( uint32_t studioIndex ) noexcept -> int
	{
		switch ( studioIndex )
		{
		case StudioHitbox::Head:
		case StudioHitbox::Neck:
			return AutoWall::HITGROUP_HEAD;
		case StudioHitbox::Chest:
		case StudioHitbox::RightChest:
		case StudioHitbox::LeftChest:
		case StudioHitbox::Pelvis:
		case StudioHitbox::Center:
			return AutoWall::HITGROUP_CHEST;
		case StudioHitbox::Stomach:
			return AutoWall::HITGROUP_STOMACH;
		case StudioHitbox::LeftLeg:
		case StudioHitbox::LeftFeet:
			return AutoWall::HITGROUP_LEFTLEG;
		case StudioHitbox::RightLeg:
		case StudioHitbox::RightFeet:
			return AutoWall::HITGROUP_RIGHTLEG;
		case StudioHitbox::LeftArm:
			return AutoWall::HITGROUP_LEFTARM;
		case StudioHitbox::RightArm:
			return AutoWall::HITGROUP_RIGHTARM;
		default:
			return AutoWall::HITGROUP_CHEST;
		}
	}

	auto BuildMultipoints( const Sample& hitbox , int scalePercent , std::vector<Vector3>& out ) noexcept -> void
	{
		out.clear();
		if ( !hitbox.valid )
			return;

		out.push_back( hitbox.position );
		if ( scalePercent <= 0 )
			return;

		const float factor = std::clamp( scalePercent / 100.f , 0.f , 1.f );
		const float radius = std::max( hitbox.scale * factor , 2.f * factor );

		out.push_back( { hitbox.position.m_x , hitbox.position.m_y , hitbox.position.m_z + radius } );
		out.push_back( { hitbox.position.m_x , hitbox.position.m_y + radius , hitbox.position.m_z } );
		out.push_back( { hitbox.position.m_x , hitbox.position.m_y - radius , hitbox.position.m_z } );
		out.push_back( { hitbox.position.m_x + radius , hitbox.position.m_y , hitbox.position.m_z } );
		out.push_back( { hitbox.position.m_x - radius , hitbox.position.m_y , hitbox.position.m_z } );
	}

	auto SegmentDistToPoint( const Vector3& segStart , const Vector3& segEnd , const Vector3& point ) noexcept -> float
	{
		const Vector3 ab{
			segEnd.m_x - segStart.m_x ,
			segEnd.m_y - segStart.m_y ,
			segEnd.m_z - segStart.m_z ,
		};

		const float abLenSq = ab.m_x * ab.m_x + ab.m_y * ab.m_y + ab.m_z * ab.m_z;
		if ( abLenSq <= 0.0001f )
		{
			const float dx = point.m_x - segStart.m_x;
			const float dy = point.m_y - segStart.m_y;
			const float dz = point.m_z - segStart.m_z;
			return std::sqrt( dx * dx + dy * dy + dz * dz );
		}

		float t = (
			( point.m_x - segStart.m_x ) * ab.m_x +
			( point.m_y - segStart.m_y ) * ab.m_y +
			( point.m_z - segStart.m_z ) * ab.m_z ) / abLenSq;

		t = std::clamp( t , 0.f , 1.f );

		const float cx = segStart.m_x + ab.m_x * t;
		const float cy = segStart.m_y + ab.m_y * t;
		const float cz = segStart.m_z + ab.m_z * t;

		const float dx = point.m_x - cx;
		const float dy = point.m_y - cy;
		const float dz = point.m_z - cz;
		return std::sqrt( dx * dx + dy * dy + dz * dz );
	}

	auto SegmentDistToSegment(
		const Vector3& rayStart ,
		const Vector3& rayEnd ,
		const Vector3& capStart ,
		const Vector3& capEnd ) noexcept -> float
	{
		const float rayDist = SegmentDistToPoint( rayStart , rayEnd , capStart );
		const float rayDistEnd = SegmentDistToPoint( rayStart , rayEnd , capEnd );
		const float capDist = SegmentDistToPoint( capStart , capEnd , rayStart );
		const float capDistEnd = SegmentDistToPoint( capStart , capEnd , rayEnd );

		return std::min( { rayDist , rayDistEnd , capDist , capDistEnd } );
	}
}

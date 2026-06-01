#include "EspBBoxCache.hpp"

#include <mutex>
#include <unordered_map>

#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Math/Matrix.hpp>
#include <CS2/SDK/SDK.hpp>

#include <Client/Features/Visuals/Esp.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Utils/CDraw.hpp>

namespace EspBBoxCache
{
	namespace
	{
		std::mutex s_mutex;
		std::unordered_map<uint32_t , Entry> s_boxes;
		Game::ViewMatrix s_viewMatrix{};
		bool s_hasViewMatrix = false;

		auto ProjectWithCachedMatrix( const float* worldPos , float& outX , float& outY , float screenW , float screenH ) noexcept -> bool
		{
			if ( !s_hasViewMatrix || !worldPos )
				return false;

			const Game::ViewMatrix& vm = s_viewMatrix;
			const float w = vm.m[3][0] * worldPos[0] + vm.m[3][1] * worldPos[1] + vm.m[3][2] * worldPos[2] + vm.m[3][3];
			if ( w < 0.001f )
				return false;

			const float invW = 1.0f / w;
			const float x = vm.m[0][0] * worldPos[0] + vm.m[0][1] * worldPos[1] + vm.m[0][2] * worldPos[2] + vm.m[0][3];
			const float y = vm.m[1][0] * worldPos[0] + vm.m[1][1] * worldPos[1] + vm.m[1][2] * worldPos[2] + vm.m[1][3];

			outX = ( screenW * 0.5f ) + ( x * invW * screenW * 0.5f );
			outY = ( screenH * 0.5f ) - ( y * invW * screenH * 0.5f );
			return true;
		}

		auto ProjectPlayerBoundsCached( uintptr_t pawn , ImVec4& outBox ) noexcept -> bool
		{
			if ( !pawn || !s_hasViewMatrix )
				return false;

			const Game::Vector3 origin = Game::GetEntityOrigin( pawn );
			if ( origin.IsZero() )
				return false;

			int screenW = 0;
			int screenH = 0;
			if ( auto* engine = SDK::Interfaces::EngineToClient() )
				engine->GetScreenSize( screenW , screenH );
			if ( screenW <= 0 || screenH <= 0 )
				return false;

			const float originPos[3] = { origin.x , origin.y , origin.z };
			float orgX = 0.f;
			float orgY = 0.f;
			if ( !ProjectWithCachedMatrix( originPos , orgX , orgY , static_cast<float>( screenW ) , static_cast<float>( screenH ) ) )
				return false;

			Game::Vector3 headPos = Game::GetBonePosition( pawn , 6 );
			if ( headPos.IsZero() )
				headPos = { origin.x , origin.y , origin.z + 72.f };
			else
				headPos.z += 8.f;

			const float headPosArr[3] = { headPos.x , headPos.y , headPos.z };
			float headX = 0.f;
			float headY = 0.f;
			if ( !ProjectWithCachedMatrix( headPosArr , headX , headY , static_cast<float>( screenW ) , static_cast<float>( screenH ) ) )
				return false;

			const float height = orgY - headY;
			if ( height < 4.f )
				return false;

			const float width = height * 0.45f;
			outBox.x = orgX - width * 0.5f;
			outBox.y = headY;
			outBox.z = orgX + width * 0.5f;
			outBox.w = orgY;
			return true;
		}
	}

	auto UpdateViewMatrix( const VMatrix* worldToProjection ) noexcept -> void
	{
		if ( !worldToProjection )
			return;

		std::lock_guard lock( s_mutex );

		for ( int row = 0; row < 4; ++row )
		{
			for ( int col = 0; col < 4; ++col )
				s_viewMatrix.m[row][col] = worldToProjection->m_data[row][col];
		}

		s_hasViewMatrix = true;
		Game::g_cachedViewMatrix = s_viewMatrix;
		Game::g_viewMatrixCached = true;
	}

	auto CalculateBoundingBoxes() noexcept -> void
	{
		auto* engine = SDK::Interfaces::EngineToClient();
		if ( !engine || !engine->IsInGame() || !Game::clientBase || !s_hasViewMatrix )
			return;

		std::unordered_map<uint32_t , Entry> nextBoxes;

		uintptr_t localCtrl = Game::Read<uintptr_t>( Game::clientBase + Offsets::dwLocalPlayerController );
		if ( !localCtrl )
			return;

		uint32_t localHandle = Game::Read<uint32_t>( localCtrl + Offsets::m_hPlayerPawn );
		uintptr_t localPawn = Game::GetEntityByHandle( localHandle );
		if ( !localPawn )
			return;

		const int localTeam = Game::Read<uint8_t>( localPawn + Offsets::m_iTeamNum );
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

			const uint32_t pawnHandle = Game::Read<uint32_t>( controller + Offsets::m_hPlayerPawn );
			if ( !pawnHandle )
				continue;

			uintptr_t pawn = Game::GetEntityByHandle( pawnHandle );
			if ( !pawn || pawn == localPawn )
				continue;

			if ( Game::Read<int32_t>( pawn + Offsets::m_iHealth ) <= 0 )
				continue;

			if ( ESP::config.teamCheck )
			{
				const int team = Game::Read<uint8_t>( pawn + Offsets::m_iTeamNum );
				if ( team == localTeam )
					continue;
			}

			ImVec4 box{};
			Entry entry;
			entry.visible = ProjectPlayerBoundsCached( pawn , box );
			if ( entry.visible )
				entry.box = box;

			nextBoxes[pawnHandle] = entry;
		}

		std::lock_guard lock( s_mutex );
		s_boxes.swap( nextBoxes );
	}

	auto TryGetBox( uint32_t pawnHandle , ImVec4& outBox ) noexcept -> bool
	{
		if ( !pawnHandle )
			return false;

		std::lock_guard lock( s_mutex );
		const auto it = s_boxes.find( pawnHandle );
		if ( it == s_boxes.end() || !it->second.visible )
			return false;

		outBox = it->second.box;
		return true;
	}
}

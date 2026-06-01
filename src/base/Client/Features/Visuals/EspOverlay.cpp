#include "EspOverlay.hpp"

#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cstdio>

#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/SDK.hpp>

#include <Client/Features/Visuals/EspBBoxCache.hpp>
#include <Client/Features/Inventory/WeaponIcons.hpp>
#include <Client/Game/Game.hpp>
#include <Client/Game/Offsets.hpp>
#include <Client/Utils/CDraw.hpp>

namespace EspOverlay
{
	namespace
	{
		constexpr float kSideSpacing = 2.f;

		auto ColorFromConfig( const float color[4] ) -> ImU32
		{
			return IM_COL32(
				static_cast<int>( color[0] * 255.f ) ,
				static_cast<int>( color[1] * 255.f ) ,
				static_cast<int>( color[2] * 255.f ) ,
				static_cast<int>( color[3] * 255.f ) );
		}

		auto WorldToScreenBox(
			uintptr_t pawn ,
			ImVec4& outBox ) -> bool
		{
			return GetDraw()->ProjectPlayerBounds( pawn , outBox );
		}

		auto DrawSkeleton( ImDrawList* draw , uintptr_t pawn , float screenW , float screenH ) -> void
		{
			if ( !draw || !pawn || !ESP::config.bSkeleton )
				return;

			const ImU32 col = ImGui::ColorConvertFloat4ToU32( ImVec4(
				ESP::config.skeletonColor[0] ,
				ESP::config.skeletonColor[1] ,
				ESP::config.skeletonColor[2] ,
				ESP::config.skeletonColor[3] ) );

			for ( const auto& conn : ESP::skeletonBones )
			{
				const Game::Vector3 b1 = Game::GetBonePosition( pawn , conn.bone1 );
				const Game::Vector3 b2 = Game::GetBonePosition( pawn , conn.bone2 );
				if ( b1.IsZero() || b2.IsZero() )
					continue;

				const float p1[3] = { b1.x , b1.y , b1.z };
				const float p2[3] = { b2.x , b2.y , b2.z };
				float s1X = 0.f;
				float s1Y = 0.f;
				float s2X = 0.f;
				float s2Y = 0.f;

				if ( Game::WorldToScreenCached( p1 , s1X , s1Y , screenW , screenH ) &&
					Game::WorldToScreenCached( p2 , s2X , s2Y , screenW , screenH ) )
				{
					draw->AddLine( ImVec2( s1X , s1Y ) , ImVec2( s2X , s2Y ) , col , 1.f );
				}
			}
		}
	}

	void OverlayContext::AddBar( const BarComponent& bar )
	{
		bars_.push_back( bar );
	}

	void OverlayContext::AddText( const TextComponent& text )
	{
		texts_.push_back( text );
	}

	void OverlayContext::AddBox( const BoxComponent& box )
	{
		box_ = box;
		hasBox_ = true;
	}

	void OverlayContext::Clear()
	{
		bars_.clear();
		texts_.clear();
		hasBox_ = false;
		for ( auto& pad : sidePadding_ )
			pad = 0.f;
	}

	void OverlayContext::Render( ImDrawList* draw , const ImVec4& box ) const
	{
		if ( !draw )
			return;

		if ( hasBox_ )
		{
			const ImVec2 min( box.x , box.y );
			const ImVec2 max( box.z , box.w );
			if ( box_.filled )
				draw->AddRectFilled( min , max , box_.fillColor );

			draw->AddRect( min , max , box_.outlineColor , 0.f , 0 , box_.thickness + 1.f );
			draw->AddRect( min , max , box_.color , 0.f , 0 , box_.thickness );
		}

		for ( const auto& bar : bars_ )
		{
			const bool horizontal = bar.side == AlignSide::Top || bar.side == AlignSide::Bottom;
			const float barLen = horizontal ? ( box.z - box.x ) : ( box.w - box.y );
			const float fillLen = barLen * std::clamp( bar.progress , 0.f , 1.f );

			ImVec2 min , max;
			switch ( bar.side )
			{
			case AlignSide::Left:
				min = ImVec2( box.x - bar.thickness - 3.f , box.y + ( barLen - fillLen ) );
				max = ImVec2( box.x - 3.f , box.w );
				break;
			case AlignSide::Right:
				min = ImVec2( box.z + 3.f , box.y + ( barLen - fillLen ) );
				max = ImVec2( box.z + bar.thickness + 3.f , box.w );
				break;
			case AlignSide::Top:
				min = ImVec2( box.x , box.y - bar.thickness - 3.f );
				max = ImVec2( box.x + fillLen , box.y - 3.f );
				break;
			default:
				min = ImVec2( box.x , box.w + 3.f );
				max = ImVec2( box.x + fillLen , box.w + bar.thickness + 3.f );
				break;
			}

			draw->AddRectFilled( min , max , bar.color );
		}

		float topOffset = 0.f;
		float bottomOffset = 0.f;

		for ( const auto& text : texts_ )
		{
			const ImVec2 textSize = ImGui::CalcTextSize( text.text.c_str() );
			ImVec2 pos;

			switch ( text.side )
			{
			case AlignSide::Top:
				pos = ImVec2( ( box.x + box.z ) * 0.5f - textSize.x * 0.5f , box.y - textSize.y - 4.f - topOffset );
				topOffset += textSize.y + kSideSpacing;
				break;
			case AlignSide::Bottom:
				pos = ImVec2( ( box.x + box.z ) * 0.5f - textSize.x * 0.5f , box.w + 4.f + bottomOffset );
				bottomOffset += textSize.y + kSideSpacing;
				break;
			case AlignSide::Left:
				pos = ImVec2( box.x - textSize.x - 4.f , box.y );
				break;
			default:
				pos = ImVec2( box.z + 4.f , box.y );
				break;
			}

			draw->AddText( ImVec2( pos.x + 1.f , pos.y + 1.f ) , IM_COL32( 0 , 0 , 0 , 255 ) , text.text.c_str() );
			draw->AddText( pos , text.color , text.text.c_str() );
		}
	}

	auto RenderPlayer(
		uintptr_t controller ,
		uintptr_t pawn ,
		const ImVec4& box ,
		int health ,
		const char* name ,
		float distanceMeters ,
		int weaponDefIndex ) -> void
	{
		(void)controller;

		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		if ( !draw || !config.useComponents )
			return;

		OverlayContext ctx;

		if ( config.showBox )
		{
			BoxComponent boxComp;
			boxComp.color = ColorFromConfig( config.boxColor );
			boxComp.thickness = config.boxThickness;
			ctx.AddBox( boxComp );
		}

		if ( config.showHealthBar && health > 0 )
		{
			BarComponent bar;
			bar.side = AlignSide::Left;
			bar.progress = static_cast<float>( health ) / 100.f;
			bar.color = ColorFromConfig( config.healthBarColor );
			bar.thickness = config.barThickness;
			ctx.AddBar( bar );
		}

		if ( config.showName && name && name[0] != '\0' )
		{
			TextComponent text;
			text.side = AlignSide::Top;
			text.text = name;
			text.color = ColorFromConfig( config.textColor );
			ctx.AddText( text );
		}

		if ( config.showDistance && distanceMeters > 0.f )
		{
			char distBuf[32];
			snprintf( distBuf , sizeof( distBuf ) , "[ %.0fm ]" , distanceMeters );
			TextComponent text;
			text.side = AlignSide::Bottom;
			text.text = distBuf;
			text.color = ColorFromConfig( config.textColor );
			ctx.AddText( text );
		}

				if ( config.showHelmetFlag || config.showKitFlag )
		{
			auto* sdkPawn = reinterpret_cast<C_CSPlayerPawn*>( pawn );
			if ( sdkPawn && sdkPawn->m_pItemServices() )
			{
				auto* items = sdkPawn->m_pItemServices();
				if ( config.showHelmetFlag )
				{
					const bool hasHelmet = items->m_bHasHelmet();
					const bool hasArmor = sdkPawn->m_ArmorValue() > 0;
					if ( hasHelmet )
					{
						TextComponent hk;
						hk.side = AlignSide::Right;
						hk.text = "HK";
						hk.color = IM_COL32( 120 , 200 , 255 , 255 );
						ctx.AddText( hk );
					}
					else if ( hasArmor )
					{
						TextComponent armor;
						armor.side = AlignSide::Right;
						armor.text = "K";
						armor.color = IM_COL32( 200 , 200 , 200 , 255 );
						ctx.AddText( armor );
					}
				}
				if ( config.showKitFlag && items->m_bHasDefuser() )
				{
					TextComponent kit;
					kit.side = AlignSide::Right;
					kit.text = "KIT";
					kit.color = IM_COL32( 100 , 255 , 140 , 255 );
					ctx.AddText( kit );
				}
			}
		}

		if ( config.showAmmoBar )
		{
			if ( const uintptr_t weaponAddr = Game::GetActiveWeapon( pawn ) )
			{
				auto* weapon = reinterpret_cast<C_CSWeaponBase*>( weaponAddr );
				int maxClip = 0;
				if ( auto* attrMgr = weapon->m_AttributeManager() )
				{
					if ( auto* item = attrMgr->m_Item() )
					{
						if ( auto* vdata = item->GetBasePlayerWeaponVData() )
							maxClip = vdata->m_iMaxClip1();
					}
				}

				const int clip = weapon->m_iClip1();
				if ( maxClip > 0 && clip >= 0 )
				{
					BarComponent ammo;
					ammo.side = AlignSide::Bottom;
					ammo.progress = static_cast<float>( clip ) / static_cast<float>( maxClip );
					ammo.color = ColorFromConfig( config.ammoBarColor );
					ctx.AddBar( ammo );
				}
			}
		}

		ctx.Render( draw , box );

		if ( config.showWeaponIcon && weaponDefIndex > 0 )
		{
			ImFont* iconFont = WeaponIcons::GetFont();
			const bool isKnife = weaponDefIndex >= 500 || ( weaponDefIndex >= 59 && weaponDefIndex <= 63 );
			if ( isKnife && WeaponIcons::GetKnifeFont() )
				iconFont = WeaponIcons::GetKnifeFont();

			if ( iconFont )
			{
				char iconUtf8[8]{};
				ImTextCharToUtf8( iconUtf8 , WeaponIcons::GetWeaponIconChar( weaponDefIndex ) );
				const ImVec2 iconSize = iconFont->CalcTextSizeA( iconFont->FontSize , FLT_MAX , 0.f , iconUtf8 );
				const ImVec2 pos(
					( box.x + box.z ) * 0.5f - iconSize.x * 0.5f ,
					box.w + 6.f );
				draw->AddText( iconFont , iconFont->FontSize , ImVec2( pos.x + 1.f , pos.y + 1.f ) , IM_COL32( 0 , 0 , 0 , 255 ) , iconUtf8 );
				draw->AddText( iconFont , iconFont->FontSize , pos , IM_COL32( 255 , 255 , 255 , 255 ) , iconUtf8 );
			}
		}
	}

	auto RenderAll() -> void
	{
		if ( !config.enabled )
		{
			ESP::skipPlayerRendering = false;
			ESP::Render();
			return;
		}

		if ( !config.useComponents )
		{
			ESP::skipPlayerRendering = false;
			ESP::Render();
			return;
		}

		ESP::skipPlayerRendering = true;
		if ( ESP::config.bBombTimer || ESP::config.bSpectators || ESP::config.bGlow )
			ESP::Render();
		ESP::skipPlayerRendering = false;

		if ( !Game::clientBase )
			return;

		auto* engine = SDK::Interfaces::EngineToClient();
		if ( !engine || !engine->IsInGame() )
			return;

		uintptr_t localCtrl = Game::Read<uintptr_t>( Game::clientBase + Offsets::dwLocalPlayerController );
		if ( !localCtrl )
			return;

		uint32_t localHandle = Game::Read<uint32_t>( localCtrl + Offsets::m_hPlayerPawn );
		uintptr_t localPawn = localHandle ? Game::GetEntityByHandle( localHandle ) : 0;

		int localTeam = 0;
		Game::Vector3 localOrigin;
		if ( localPawn )
		{
			localTeam = Game::Read<uint8_t>( localPawn + Offsets::m_iTeamNum );
			localOrigin = Game::GetEntityOrigin( localPawn );
		}

		const float maxDistSq = ESP::config.maxDistance > 0.f
			? ESP::config.maxDistance * ESP::config.maxDistance
			: 0.f;

		ImVec2 displaySize = ImGui::GetIO().DisplaySize;
		const float screenW = displaySize.x;
		const float screenH = displaySize.y;

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
			if ( !pawnHandle )
				continue;

			uintptr_t pawn = Game::GetEntityByHandle( pawnHandle );
			if ( !pawn || pawn == localPawn )
				continue;

			const int health = Game::Read<int32_t>( pawn + Offsets::m_iHealth );
			if ( health <= 0 )
				continue;

			if ( ESP::config.teamCheck )
			{
				const int team = Game::Read<uint8_t>( pawn + Offsets::m_iTeamNum );
				if ( team == localTeam )
					continue;
			}

			const Game::Vector3 origin = Game::GetEntityOrigin( pawn );
			if ( maxDistSq > 0.f && !localOrigin.IsZero() && !origin.IsZero() )
			{
				const float dx = origin.x - localOrigin.x;
				const float dy = origin.y - localOrigin.y;
				const float dz = origin.z - localOrigin.z;
				if ( dx * dx + dy * dy + dz * dz > maxDistSq )
					continue;
			}

			ImVec4 box;
			if ( !EspBBoxCache::TryGetBox( pawnHandle , box ) && !WorldToScreenBox( pawn , box ) )
				continue;

			float distanceMeters = 0.f;
			if ( !localOrigin.IsZero() && !origin.IsZero() )
			{
				const float distUnits = ( origin - localOrigin ).Length2D();
				distanceMeters = distUnits * 0.0254f;
			}

			char nameBuf[32]{};
			uintptr_t namePtr = Game::Read<uintptr_t>( controller + Offsets::m_sSanitizedPlayerName );
			if ( namePtr )
			{
				struct NameBuf { char data[32]; };
				NameBuf buf = Game::Read<NameBuf>( namePtr );
				buf.data[31] = '\0';
				snprintf( nameBuf , sizeof( nameBuf ) , "%s" , buf.data );
			}

			int weaponDefIndex = 0;
			if ( const uintptr_t weapon = Game::GetActiveWeapon( pawn ) )
			{
				weaponDefIndex = Game::Read<uint16_t>(
					weapon + Offsets::m_AttributeManager + Offsets::m_Item + Offsets::m_iItemDefinitionIndex );
			}

			RenderPlayer( controller , pawn , box , health , nameBuf , distanceMeters , weaponDefIndex );
			DrawSkeleton( ImGui::GetBackgroundDrawList() , pawn , screenW , screenH );
		}
	}
}

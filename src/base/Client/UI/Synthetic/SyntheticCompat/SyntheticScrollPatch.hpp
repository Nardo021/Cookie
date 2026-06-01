#pragma once

#include <Client/UI/Synthetic/SyntheticCompat/begin_imgui_compat.h>
#include <framework/settings/functions.h>

namespace SyntheticScroll
{
	inline constexpr float kClampThreshold = 20.f;
	inline constexpr float kInaccuracy = 2.f;

	inline auto EdgeSnap( float target , float snapMin , float snapMax , float snapThreshold , float centerRatio ) -> float
	{
		if ( target <= snapMin + snapThreshold )
			return ImLerp( snapMin , target , centerRatio );
		if ( target >= snapMax - snapThreshold )
			return ImLerp( target , snapMax , centerRatio );
		return target;
	}

	inline auto CalcNextScrollFromTargetAndClamp( ImGuiWindow* window , bool clamp ) -> ImVec2
	{
		ImVec2 scroll = window->Scroll;
		if ( window->ScrollTarget.x < FLT_MAX )
		{
			const float decorationTotalWidth = window->ScrollbarSizes.x;
			const float centerXRatio = window->ScrollTargetCenterRatio.x;
			float scrollTargetX = window->ScrollTarget.x;
			if ( window->ScrollTargetEdgeSnapDist.x > 0.f )
			{
				const float snapXMin = 0.f;
				const float snapXMax = window->ScrollMax.x + window->SizeFull.x - decorationTotalWidth;
				scrollTargetX = EdgeSnap( scrollTargetX , snapXMin , snapXMax , window->ScrollTargetEdgeSnapDist.x , centerXRatio );
			}
			scroll.x = scrollTargetX - centerXRatio * ( window->SizeFull.x - decorationTotalWidth );
		}
		if ( window->ScrollTarget.y < FLT_MAX )
		{
			const float decorationTotalHeight = window->TitleBarHeight + window->MenuBarHeight + window->ScrollbarSizes.y;
			const float centerYRatio = window->ScrollTargetCenterRatio.y;
			float scrollTargetY = window->ScrollTarget.y;
			if ( window->ScrollTargetEdgeSnapDist.y > 0.f )
			{
				const float snapYMin = 0.f;
				const float snapYMax = window->ScrollMax.y + window->SizeFull.y - decorationTotalHeight;
				scrollTargetY = EdgeSnap( scrollTargetY , snapYMin , snapYMax , window->ScrollTargetEdgeSnapDist.y , centerYRatio );
			}
			scroll.y = scrollTargetY - centerYRatio * ( window->SizeFull.y - decorationTotalHeight );
		}
		scroll.x = IM_FLOOR( ImMax( scroll.x , 0.f ) );
		scroll.y = IM_FLOOR( ImMax( scroll.y , 0.f ) );
		if ( !window->Collapsed && !window->SkipItems )
		{
			scroll.x = ImMin( scroll.x , window->ScrollMax.x );
			if ( !clamp )
				scroll.y = ImClamp( scroll.y , -kClampThreshold , window->ScrollMax.y + kClampThreshold );
			else
				scroll.y = ImMin( scroll.y , window->ScrollMax.y );
		}
		return scroll;
	}

	inline auto ApplyAnimatedWindowScroll( ImGuiWindow* window , ImGuiID scrollAnimId , ImGuiWindowFlags flags ) -> void
	{
		ImGuiContext& g = *GImGui;

		struct scroll_state
		{
			float scroll = 0.f;
			bool active_clamp = true;
		};

		scroll_state* scrollAnim = gui->anim_container( &scrollAnim , scrollAnimId );

		const float wheelY = g.IO.MouseWheel;
		const bool wheelScrolling = wheelY != 0.f;
		const bool atScrollTop = window->Scroll.y <= kInaccuracy;
		const bool atScrollBottom = window->ScrollMax.y > 0.f
			&& window->Scroll.y >= window->ScrollMax.y - kInaccuracy;
		if ( wheelScrolling
			&& g.ActiveId != GetWindowScrollbarID( window , ImGuiAxis_Y )
			&& IsWindowHovered()
			&& window->ScrollMax.y != 0.f
			&& ( ( wheelY > 0.f && atScrollTop ) || ( wheelY < 0.f && atScrollBottom ) ) )
			scrollAnim->active_clamp = false;
		else if ( window->Scroll.y == -kClampThreshold || window->Scroll.y == window->ScrollMax.y + kClampThreshold )
			scrollAnim->active_clamp = true;

		if ( window->Scroll.y > 0.1f && window->Scroll.y < window->ScrollMax.y - kInaccuracy )
			scrollAnim->active_clamp = true;

		const float neededScrollClamp = scrollAnim->active_clamp
			? CalcNextScrollFromTargetAndClamp( window , true ).y
			: CalcNextScrollFromTargetAndClamp( window , false ).y;

		scrollAnim->scroll = ImLerp( scrollAnim->scroll , neededScrollClamp , gui->fixed_speed( 16.f ) );

		if ( g.ActiveId != GetWindowScrollbarID( window , ImGuiAxis_Y ) )
		{
			if ( !( flags & ImGuiWindowFlags_NoScrollWithMouse ) && window->Scroll.y != neededScrollClamp )
				window->Scroll.y = std::round( scrollAnim->scroll );
		}
		else
		{
			window->Scroll.y = neededScrollClamp;
			window->ScrollTarget = ImVec2( FLT_MAX , FLT_MAX );
		}
	}
}

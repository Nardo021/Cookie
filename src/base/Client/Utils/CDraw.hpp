#pragma once

#include <Common/Common.hpp>
#include <ImGui/imgui.h>

#include <CS2/SDK/Math/Vector3.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

enum ERectRenderFlags : unsigned int
{
	DRAW_RECT_NONE = 0,
	DRAW_RECT_OUTLINE = ( 1 << 0 ),
	DRAW_RECT_BORDER = ( 1 << 1 ),
	DRAW_RECT_FILLED = ( 1 << 2 )
};

enum ECircleRenderFlags : unsigned int
{
	DRAW_CIRCLE_NONE = 0,
	DRAW_CIRCLE_OUTLINE = ( 1 << 0 ),
	DRAW_CIRCLE_FILLED = ( 1 << 1 )
};

enum ETextRenderFlags : unsigned int
{
	DRAW_TEXT_NONE = 0,
	DRAW_TEXT_DROPSHADOW = ( 1 << 0 ),
	DRAW_TEXT_OUTLINE = ( 1 << 1 )
};

struct BBox_t
{
	BBox_t()
	{
		Invalidate();
	}

	auto Invalidate() -> void
	{
		static constexpr auto kMaxFloat = std::numeric_limits<float>::max();

		m_Mins.x = m_Mins.y = kMaxFloat;
		m_Maxs.x = m_Maxs.y = -kMaxFloat;

		std::fill( std::begin( m_Vertices ) , std::end( m_Vertices ) , ImVec2{ -kMaxFloat , -kMaxFloat } );
	}

	ImVec2 m_Mins{};
	ImVec2 m_Maxs{};
	ImVec2 m_Vertices[8]{};
};

using EasingFunction_t = double ( * )( double );

struct AnimationHandler_t
{
	AnimationHandler_t( EasingFunction_t fnIn = nullptr , EasingFunction_t fnOut = nullptr ) :
		fnEaseIn( fnIn ) , fnEaseOut( fnOut ) , bSwitch( false ) , bLastSwitch( false ) , flElapsedTime( 0.f ) , flValue( 0.1f )
	{
	}

	auto Update( float flDeltaTime , float flDuration ) -> void;

	[[nodiscard]] auto GetValue( float flScale = 1.0f ) const -> float
	{
		return flValue * flScale;
	}

	[[nodiscard]] auto GetSwitch() const -> bool
	{
		return bSwitch;
	}

	auto Switch() -> void
	{
		bSwitch = !bSwitch;
	}

	auto SetSwitch( bool bState ) -> void
	{
		bSwitch = bState;
	}

private:
	bool bSwitch = false;
	bool bLastSwitch = false;
	float flElapsedTime = 0.f;
	float flValue = 0.1f;
	EasingFunction_t fnEaseIn = nullptr;
	EasingFunction_t fnEaseOut = nullptr;
};

class CDraw final
{
public:
	[[nodiscard]] static auto GetDrawList() -> ImDrawList*;

	auto WorldToScreen( const Vector3& origin , ImVec2& out ) const -> bool;

	auto DrawRect( ImDrawList* drawList , const ImVec2& min , const ImVec2& max , ImU32 color ,
		unsigned int flags = DRAW_RECT_NONE , ImU32 outlineColor = IM_COL32( 0 , 0 , 0 , 255 ) ,
		float rounding = 0.f , ImDrawFlags roundingCorners = ImDrawFlags_RoundCornersAll ,
		float thickness = 1.f , float outlineThickness = 1.f ) const -> void;

	auto DrawCircle( ImDrawList* drawList , const ImVec2& center , float radius , ImU32 color , int segments = 32 ,
		unsigned int flags = DRAW_CIRCLE_NONE , ImU32 outlineColor = IM_COL32( 0 , 0 , 0 , 255 ) ,
		float thickness = 1.f , float outlineThickness = 1.f ) const -> void;

	auto DrawText( ImDrawList* drawList , const ImFont* font , const ImVec2& position , const char* text , ImU32 color ,
		unsigned int flags = DRAW_TEXT_NONE , ImU32 outlineColor = IM_COL32( 0 , 0 , 0 , 255 ) ,
		float thickness = 1.f ) const -> void;

	auto ProjectPlayerBounds( uintptr_t pawn , ImVec4& outBox ) const -> bool;

	auto DrawGlowRect( ImDrawList* drawList , const ImVec2& min , const ImVec2& max , ImU32 color , float thickness = 1.f ) const -> void;
};

auto GetDraw() -> CDraw*;

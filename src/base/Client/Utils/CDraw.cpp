#include "CDraw.hpp"

#include <Client/Core/CEasing.hpp>
#include <Client/Game/Game.hpp>

#include <CS2/SDK/FunctionListSDK.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/SDK.hpp>

static CDraw g_CDraw{};

auto AnimationHandler_t::Update( float flDeltaTime , float flDuration ) -> void
{
	if ( fnEaseIn == nullptr )
		fnEaseIn = &CEasing::InSine;

	if ( fnEaseOut == nullptr )
		fnEaseOut = &CEasing::OutSine;

	if ( bSwitch != bLastSwitch )
		flElapsedTime = 0.f;

	flElapsedTime = std::clamp( flElapsedTime , 0.f , flDuration );

	const float flTime = flDuration > 0.f ? flElapsedTime / flDuration : 1.f;
	const float flInitialValue = bSwitch ? 0.1f : flValue;
	const float flTargetValue = bSwitch ? 1.0f : 0.1f;
	const EasingFunction_t fnCurrentEase = bSwitch ? fnEaseIn : fnEaseOut;

	flValue = ( flInitialValue + ( flTargetValue - flInitialValue ) ) * static_cast<float>( fnCurrentEase( flTime ) );
	flValue = std::clamp( flValue , 0.1f , 1.0f );

	flElapsedTime += flDeltaTime;
	bLastSwitch = bSwitch;
}

auto CDraw::GetDrawList() -> ImDrawList*
{
	return ImGui::GetBackgroundDrawList();
}

auto CDraw::WorldToScreen( const Vector3& origin , ImVec2& out ) const -> bool
{
	if ( !ImGui::GetCurrentContext() )
		return false;

	Vector3 screen{};

	if ( ScreenTransform( origin , screen ) )
		return false;

	int screenWidth = 0;
	int screenHeight = 0;

	SDK::Interfaces::EngineToClient()->GetScreenSize( screenWidth , screenHeight );

	out.x = ( ( screen.m_x + 1.0f ) * 0.5f ) * static_cast<float>( screenWidth );
	out.y = static_cast<float>( screenHeight ) - ( ( ( screen.m_y + 1.0f ) * 0.5f ) * static_cast<float>( screenHeight ) );

	return true;
}

auto CDraw::DrawRect( ImDrawList* drawList , const ImVec2& min , const ImVec2& max , ImU32 color ,
	unsigned int flags , ImU32 outlineColor , float rounding , ImDrawFlags roundingCorners ,
	float thickness , float outlineThickness ) const -> void
{
	if ( drawList == nullptr )
		drawList = GetDrawList();

	if ( drawList == nullptr )
		return;

	if ( flags & DRAW_RECT_FILLED )
		drawList->AddRectFilled( min , max , color , rounding , roundingCorners );
	else
	{
		drawList->AddRect( min , max , color , rounding , roundingCorners , thickness );
		thickness *= 0.5f;
	}

	const float flHalfOutlineThickness = outlineThickness * 0.5f;
	const ImVec2 thicknessOffset{ thickness + flHalfOutlineThickness , thickness + flHalfOutlineThickness };

	if ( flags & DRAW_RECT_BORDER )
		drawList->AddRect( min + thicknessOffset , max - thicknessOffset , outlineColor , rounding , roundingCorners , outlineThickness );

	if ( flags & DRAW_RECT_OUTLINE )
		drawList->AddRect( min - thicknessOffset , max + thicknessOffset , outlineColor , rounding , roundingCorners , outlineThickness );
}

auto CDraw::DrawCircle( ImDrawList* drawList , const ImVec2& center , float radius , ImU32 color , int segments ,
	unsigned int flags , ImU32 outlineColor , float thickness , float outlineThickness ) const -> void
{
	if ( drawList == nullptr )
		drawList = GetDrawList();

	if ( drawList == nullptr )
		return;

	if ( flags & DRAW_CIRCLE_FILLED )
	{
		drawList->AddCircleFilled( center , radius , color , segments );
		thickness = 0.f;
	}
	else
	{
		drawList->AddCircle( center , radius , color , segments , thickness );
	}

	if ( flags & DRAW_CIRCLE_OUTLINE )
		drawList->AddCircle( center , radius + outlineThickness , outlineColor , segments , thickness + outlineThickness );
}

auto CDraw::DrawText( ImDrawList* drawList , const ImFont* font , const ImVec2& position , const char* text , ImU32 color ,
	unsigned int flags , ImU32 outlineColor , float thickness ) const -> void
{
	if ( drawList == nullptr )
		drawList = GetDrawList();

	if ( drawList == nullptr || text == nullptr || text[0] == '\0' )
		return;

	if ( font == nullptr )
		font = ImGui::GetFont();

	drawList->PushTextureID( font->ContainerAtlas->TexID );

	if ( flags & DRAW_TEXT_DROPSHADOW )
		drawList->AddText( font , font->FontSize , position + ImVec2( thickness , thickness ) , outlineColor , text );
	else if ( flags & DRAW_TEXT_OUTLINE )
	{
		drawList->AddText( font , font->FontSize , position + ImVec2( thickness , -thickness ) , outlineColor , text );
		drawList->AddText( font , font->FontSize , position + ImVec2( -thickness , thickness ) , outlineColor , text );
	}

	drawList->AddText( font , font->FontSize , position , color , text );
	drawList->PopTextureID();
}

auto CDraw::ProjectPlayerBounds( uintptr_t pawn , ImVec4& outBox ) const -> bool
{
	if ( !pawn )
		return false;

	const Game::Vector3 origin = Game::GetEntityOrigin( pawn );
	if ( origin.IsZero() )
		return false;

	ImVec2 orgScreen;
	if ( !WorldToScreen( Vector3( origin.x , origin.y , origin.z ) , orgScreen ) )
		return false;

	Game::Vector3 headPos = Game::GetBonePosition( pawn , 6 );
	if ( headPos.IsZero() )
		return false;

	headPos.z += 8.f;
	ImVec2 headScreen;
	if ( !WorldToScreen( Vector3( headPos.x , headPos.y , headPos.z ) , headScreen ) )
		return false;

	const float boxHeight = orgScreen.y - headScreen.y;
	const float boxWidth = boxHeight * 0.5f;
	outBox = ImVec4(
		headScreen.x - boxWidth * 0.5f ,
		headScreen.y ,
		headScreen.x + boxWidth * 0.5f ,
		headScreen.y + boxHeight );

	return boxHeight > 1.f;
}

auto CDraw::DrawGlowRect( ImDrawList* drawList , const ImVec2& min , const ImVec2& max , ImU32 color , float thickness ) const -> void
{
	if ( drawList == nullptr )
		drawList = GetDrawList();

	if ( drawList == nullptr )
		return;

	for ( int i = 1; i <= 3; ++i )
	{
		const float expand = static_cast<float>( i );
		drawList->AddRect(
			ImVec2( min.x - expand , min.y - expand ) ,
			ImVec2( max.x + expand , max.y + expand ) ,
			( color & 0x00FFFFFF ) | ( ( ( color >> 24 ) & 0xFF ) / ( i + 1 ) ) << 24 ,
			0.f ,
			0 ,
			thickness );
	}

	drawList->AddRect( min , max , color , 0.f , 0 , thickness );
}

auto GetDraw() -> CDraw*
{
	return &g_CDraw;
}

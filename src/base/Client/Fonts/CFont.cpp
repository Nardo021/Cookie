#include "CFont.hpp"

#include <cstdio>

auto CFont::InitFont( std::wstring Name , float FontSize ) -> void
{
	m_FontSize = FontSize;
	m_FontName = std::move( Name );
}

auto CFont::DrawString( int x , int y , ImColor Color , int Flags , const char* fmt , ... ) -> void
{
	char Buffer[g_BufferSize] = { 0 };

	va_list va_alist;
	va_start( va_alist , fmt );
	vsnprintf( Buffer , sizeof( Buffer ) , fmt , va_alist );
	va_end( va_alist );

	ImDrawList* draw = ImGui::GetBackgroundDrawList();
	if ( !draw )
		return;

	ImVec2 pos( static_cast<float>( x ) , static_cast<float>( y ) );
	const ImVec2 textSize = ImGui::CalcTextSize( Buffer );

	if ( Flags & FONT_ALIGN_CENTER )
		pos.x -= textSize.x * 0.5f;
	else if ( Flags & FONT_ALIGN_RIGHT )
		pos.x -= textSize.x;

	const ImU32 shadow = IM_COL32( 0 , 0 , 0 , static_cast<int>( Color.Value.w * 255.f ) );
	const ImU32 color = Color;

	draw->AddText( ImVec2( pos.x + 1.f , pos.y + 1.f ) , shadow , Buffer );
	draw->AddText( pos , color , Buffer );
}

auto CFont::DrawString( ImVec2 Pos , ImColor Color , int Flags , const char* fmt , ... ) -> void
{
	char Buffer[g_BufferSize] = { 0 };

	va_list va_alist;
	va_start( va_alist , fmt );
	vsnprintf( Buffer , sizeof( Buffer ) , fmt , va_alist );
	va_end( va_alist );

	DrawString( static_cast<int>( Pos.x ) , static_cast<int>( Pos.y ) , Color , Flags , Buffer );
}

auto CFont::GetStringLayoutRect( int Flags , FontLayoutRect* pRect , const char* fmt , ... ) -> FontLayoutRect
{
	char Buffer[g_BufferSize] = { 0 };

	va_list va_alist;
	va_start( va_alist , fmt );
	vsnprintf( Buffer , sizeof( Buffer ) , fmt , va_alist );
	va_end( va_alist );

	const ImVec2 textSize = ImGui::CalcTextSize( Buffer );
	FontLayoutRect rect{};

	if ( pRect )
	{
		rect = *pRect;
	}

	if ( Flags & FONT_ALIGN_CENTER )
	{
		const float width = textSize.x;
		const float center = ( rect.Left + rect.Right ) * 0.5f;
		rect.Left = center - width * 0.5f;
		rect.Right = center + width * 0.5f;
	}

	rect.Bottom = rect.Top + textSize.y;
	return rect;
}

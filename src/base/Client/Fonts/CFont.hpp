#pragma once

#include <Common/Common.hpp>

#include <ImGui/imgui.h>

struct FontLayoutRect
{
	float Left = 0.f;
	float Top = 0.f;
	float Right = 0.f;
	float Bottom = 0.f;
};

enum FontAlignFlag : int
{
	FONT_ALIGN_LEFT = 0x0,
	FONT_ALIGN_CENTER = 0x1,
	FONT_ALIGN_RIGHT = 0x2,
};

class CFont final
{
public:
	auto InitFont( std::wstring Name , float FontSize = 12.f ) -> void;

public:
	auto DrawString( int x , int y , ImColor Color , int Flags , const char* fmt , ... ) -> void;
	auto DrawString( ImVec2 Pos , ImColor Color , int Flags , const char* fmt , ... ) -> void;
	auto GetStringLayoutRect( int Flags , FontLayoutRect* pRect , const char* fmt , ... ) -> FontLayoutRect;

public:
	inline auto GetFontSize() -> float
	{
		return m_FontSize;
	}

private:
	std::wstring m_FontName;
	float m_FontSize = 0.f;

private:
	static constexpr auto g_BufferSize = 64;
};

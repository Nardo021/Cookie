#include "CFontManager.hpp"

static CFontManager g_CFontManager{};

auto CFontManager::FirstInitFonts() -> void
{
	if ( m_bInit )
		return;

	m_VerdanaFont.InitFont( L"Verdana" , 11.f );
	m_bInit = true;
}

auto GetFontManager() -> CFontManager*
{
	return &g_CFontManager;
}

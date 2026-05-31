#include "Hook_DrawGlow.hpp"

auto Hook_DrawGlow( CGlowProperty* pCGlowProperty ) -> void*
{
	return DrawGlow_o( pCGlowProperty );
}

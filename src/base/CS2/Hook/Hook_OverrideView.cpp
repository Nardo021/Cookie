#include "Hook_OverrideView.hpp"

#include <Client/Features/Visuals/ThirdPerson.hpp>
#include <CS2/SDK/Update/CViewSetup.hpp>
#include <CS2/Hook/Hook_GetRenderFov.hpp>

auto Hook_OverrideView( void* rcx , void* pSetup ) -> void
{
	OverrideView_o( rcx , pSetup );

	if ( pSetup )
	{
		ThirdPerson::ApplyView( reinterpret_cast<CViewSetup*>( pSetup ) );

		if ( ThirdPerson::config.enabled )
			g_fovOverride = static_cast<float>( ThirdPerson::config.fov );
		else
			g_fovOverride = 0.f;
	}
}

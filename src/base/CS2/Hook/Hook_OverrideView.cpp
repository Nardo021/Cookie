#include "Hook_OverrideView.hpp"

#include <Client/Features/Cookie/ThirdPerson.hpp>
#include <CS2/SDK/Update/CViewSetup.hpp>

auto Hook_OverrideView( void* rcx , void* pSetup ) -> void
{
	OverrideView_o( rcx , pSetup );

	if ( pSetup )
		ThirdPerson::ApplyView( reinterpret_cast<CViewSetup*>( pSetup ) );
}

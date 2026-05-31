#include "Hook_OverrideView.hpp"

#include <Client/Features/Cookie/ThirdPerson.hpp>

auto Hook_OverrideView( void* rcx , void* pSetup ) -> void
{
	OverrideView_o( rcx , pSetup );

	if ( pSetup )
		ThirdPerson::Apply( pSetup );
}

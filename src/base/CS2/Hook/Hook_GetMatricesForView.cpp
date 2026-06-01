#include "Hook_GetMatricesForView.hpp"

#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Math/Matrix.hpp>
#include <CS2/SDK/SDK.hpp>

#include <Client/Features/Visuals/EspBBoxCache.hpp>

auto Hook_GetMatricesForView( void* rcx , void* view ,
                              VMatrix* pWorldToView ,
                              VMatrix* pViewToProjection ,
                              VMatrix* pWorldToProjection ,
                              VMatrix* pWorldToPixels ) -> void
{
	GetMatricesForView_o( rcx , view , pWorldToView , pViewToProjection , pWorldToProjection , pWorldToPixels );

	auto* engine = SDK::Interfaces::EngineToClient();
	if ( !engine || !engine->IsInGame() )
		return;

	if ( pWorldToProjection )
		EspBBoxCache::UpdateViewMatrix( pWorldToProjection );

	EspBBoxCache::CalculateBoundingBoxes();
}

#include "Hook_GetRenderFov.hpp"

#include <CS2/SDK/Types/CEntityData.hpp>
#include <Client/Features/Visuals/WorldFov.hpp>

#include <GameClient/CL_Players.hpp>

auto Hook_GetRenderFov( void* rcx ) -> float
{
	const float original = GetRenderFov_o( rcx );

	if ( g_fovOverride > 0.f )
		return g_fovOverride;

	bool scoped = false;
	if ( auto* pawn = GetCL_Players()->GetLocalPlayerPawn() )
		scoped = pawn->m_bIsScoped();

	return WorldFov::ApplyOverride( original , scoped );
}

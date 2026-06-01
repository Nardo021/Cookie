#include "Hook_SetViewModelFov.hpp"

auto Hook_SetViewModelFov( void* rcx ) -> float
{
	const float original = SetViewModelFov_o( rcx );

	if ( g_viewModelFovConfig.enabled && g_viewModelFovConfig.amount > 0.f )
		return g_viewModelFovConfig.amount;

	return original;
}

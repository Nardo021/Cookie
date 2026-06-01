#include "WorldFov.hpp"

namespace WorldFov
{
	auto ApplyOverride( float original , bool scoped ) noexcept -> float
	{
		if ( !config.enabled || scoped || config.amount <= 0.f )
			return original;

		return config.amount;
	}
}

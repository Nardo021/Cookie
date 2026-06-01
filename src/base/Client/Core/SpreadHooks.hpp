#pragma once

#include <Client/Core/CSigScan.hpp>

namespace CookieCore
{
	namespace SpreadHooks
	{
		inline CSigScan* s_GetSpread = nullptr;
		inline CSigScan* s_GetInaccuracy = nullptr;

		inline auto Register( CSigScan& spread , CSigScan& inaccuracy ) noexcept -> void
		{
			s_GetSpread = &spread;
			s_GetInaccuracy = &inaccuracy;
		}

		inline auto IsReady() noexcept -> bool
		{
			return s_GetSpread && s_GetSpread->GetFunction()
				&& s_GetInaccuracy && s_GetInaccuracy->GetFunction();
		}

		auto SetZeroSpreadActive( bool active ) noexcept -> void;
		auto ShouldZeroSpread() noexcept -> bool;
		auto Install() noexcept -> bool;
		auto IsHooked() noexcept -> bool;
	}
}

#pragma once

#include <Client/Utils/CInputSystem.hpp>

namespace KeyBindUtils
{
	inline auto IsActive( unsigned int key , bool useKey = true ) -> bool
	{
		if ( !useKey )
			return true;

		if ( key == 0U )
			return false;

		KeyBind_t bind{ nullptr , key , EKeyBindMode::HOLD };
		return GetInputSystem()->GetBindState( bind );
	}
}

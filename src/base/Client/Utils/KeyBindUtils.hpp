#pragma once

#include <Client/Utils/CInputSystem.hpp>

namespace KeyBindUtils
{
	struct KeyBindSlot
	{
		KeyBind_t bind{};
		unsigned int cachedKey = 0U;
		bool         cachedHold = true;
	};

	inline auto IsActive( unsigned int key , bool useKey , bool holdMode , KeyBindSlot& slot ) -> bool
	{
		if ( !useKey )
			return true;

		if ( key == 0U )
			return false;

		const auto mode = holdMode ? EKeyBindMode::HOLD : EKeyBindMode::TOGGLE;

		if ( slot.cachedKey != key || slot.cachedHold != holdMode || slot.bind.nMode != mode )
		{
			slot.cachedKey = key;
			slot.cachedHold = holdMode;
			slot.bind = KeyBind_t{ nullptr , key , mode };
			if ( !holdMode )
				slot.bind.bEnable = false;
		}
		else
		{
			slot.bind.uKey = key;
			slot.bind.nMode = mode;
		}

		return GetInputSystem()->GetBindState( slot.bind );
	}

	inline auto IsActive( unsigned int key , bool useKey = true ) -> bool
	{
		static KeyBindSlot s_legacy{};
		return IsActive( key , useKey , true , s_legacy );
	}
}

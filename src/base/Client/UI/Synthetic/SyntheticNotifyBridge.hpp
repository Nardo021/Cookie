#pragma once

#include <Client/Utils/CNotify.hpp>

namespace SyntheticNotifyBridge
{
	auto Push( ENotificationType type , const char* text ) noexcept -> void;
}

#pragma once

class IGameEvent;

namespace GameEvents
{
	auto Dispatch( IGameEvent* event ) noexcept -> void;
	auto OnRoundStart() noexcept -> void;
	auto OnPlayerDeath( IGameEvent* event ) noexcept -> void;

} // namespace GameEvents

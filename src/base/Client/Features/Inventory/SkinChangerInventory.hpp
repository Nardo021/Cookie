#pragma once

#include <cstdint>

class IGameEvent;

namespace SkinChangerInventory
{
	auto AddEconItemId( uint64_t itemId ) noexcept -> void;
	auto IsManagedItem( uint64_t itemId ) noexcept -> bool;

	auto SyncFromConfig() noexcept -> void;
	auto OnFrameStageNotify( int frameStage ) noexcept -> void;
	auto OnEquipItemInLoadout( int team , int slot , uint64_t itemId ) noexcept -> void;

	auto RequestNetworkFullUpdate() noexcept -> void;
	auto ProcessNetworkFullUpdate() noexcept -> void;
	auto OnPlayerDeathKillfeed( IGameEvent* event ) noexcept -> void;

	auto ResetRound() noexcept -> void;
	auto Shutdown() noexcept -> void;

	auto UsesInventoryPath() noexcept -> bool;
}

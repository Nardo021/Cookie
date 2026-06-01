#pragma once

#include <cstdint>
#include <vector>

class CCSPlayerInventory;
class C_CSPlayerPawn;
class C_CSGOViewModel;
class C_EconItemView;
class CEconItemDefinition;

namespace Gloves
{
	enum material_magic_number : uint32_t
	{
		kMaterialMagicGloves = 0xF143B82A ,
	};

	struct Config
	{
		bool  enabled = false;
		int   modelIndex = 0;
		int   paintKit = 0;
		float wear = 0.001f;
		int   seed = 0;
		int   team = 3;
	};

	extern Config config;

	auto OnFrameStageNotify( int frameStage ) noexcept -> void;
	auto OnEquipItemInLoadout( int team , int slot , uint64_t itemId ) noexcept -> void;
	auto ApplyFromConfig() noexcept -> bool;
	auto ResetRound() noexcept -> void;
	auto Shutdown() noexcept -> void;
	auto AddEconItemId( uint64_t itemId ) noexcept -> void;

	auto OnGlove(
		CCSPlayerInventory* inventory ,
		C_CSPlayerPawn* localPawn ,
		C_CSGOViewModel* viewModel ,
		C_EconItemView* glovesItem ,
		CEconItemDefinition* glovesDefinition ) noexcept -> void;
}

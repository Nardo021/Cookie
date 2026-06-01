#include "PlantBomb.hpp"

#include <CS2/SDK/Types/CEntityData.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <GameClient/CL_Players.hpp>

namespace PlantBomb
{
	namespace
	{
		constexpr int kFrameNetUpdateEnd = 3;

		auto IsHoldingC4( C_CSPlayerPawn* pawn ) noexcept -> bool
		{
			if ( !pawn )
				return false;

			const auto* weaponServices = pawn->m_pWeaponServices();
			if ( !weaponServices )
				return false;

			auto* weapon = weaponServices->m_hActiveWeapon().Get<C_CSWeaponBase>();
			if ( !weapon )
				return false;

			return reinterpret_cast<C_BaseEntity*>( weapon )->IsC4();
		}

		auto ApplyBombZoneSpoof() noexcept -> void
		{
			if ( !config.enabled )
				return;

			auto* pawn = GetCL_Players()->GetLocalPlayerPawn();
			if ( !pawn || !pawn->IsAlive() )
				return;

			if ( !IsHoldingC4( pawn ) )
				return;

			pawn->m_bInBombZone() = true;
		}
	}

	auto Process( CCSGOInput* /*input*/ , CUserCmd* /*cmd*/ ) noexcept -> void
	{
		ApplyBombZoneSpoof();
	}

	auto OnFrameStageNotify( int frameStage ) noexcept -> void
	{
		if ( frameStage != kFrameNetUpdateEnd )
			return;

		ApplyBombZoneSpoof();
	}
}

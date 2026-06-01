#include "Hook_FrameStageNotify.hpp"

#include <Client/Features/Inventory/Gloves.hpp>
#include <Client/Features/Combat/LagComp.hpp>
#include <Client/Features/Misc/PlantBomb.hpp>
#include <Client/Features/Inventory/SkinChangerInventory.hpp>
#include <Client/Features/Visuals/WorldVisuals.hpp>

auto Hook_FrameStageNotify( CSource2Client* pCSource2Client , int FrameStage ) -> void
{
	Gloves::OnFrameStageNotify( FrameStage );
	SkinChangerInventory::OnFrameStageNotify( FrameStage );
	PlantBomb::OnFrameStageNotify( FrameStage );
	WorldVisuals::OnFrameStageNotify( FrameStage );

	if ( FrameStage == 3 )
		LagComp::RecordPlayers();

	FrameStageNotify_o( pCSource2Client , FrameStage );
}

#include "Hook_FrameStageNotify.hpp"

auto Hook_FrameStageNotify( CSource2Client* pCSource2Client , int FrameStage ) -> void
{
	return FrameStageNotify_o( pCSource2Client , FrameStage );
}

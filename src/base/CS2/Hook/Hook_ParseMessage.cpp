#include "Hook_ParseMessage.hpp"

auto Hook_CDemoRecorder( CDemoRecorder* pDemoRecorder , CNetworkSerializerPB* pSerializer , CNetMessagePB* pNetMessage ) -> bool
{
	return CDemoRecorder_o( pDemoRecorder , pSerializer , pNetMessage );
}

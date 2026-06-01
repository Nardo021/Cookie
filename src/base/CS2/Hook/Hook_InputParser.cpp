#include "Hook_InputParser.hpp"

#include <CS2/Protobuf/cs_usercmd.pb.h>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <Client/Features/Combat/Rage/RageSubTick.hpp>

auto Hook_InputParser(
	void* inputMessage ,
	CSGOInputHistoryEntryPB* historyEntry ,
	char a3 ,
	void* a4 ,
	void* a5 ,
	void* a6 ) -> void*
{
	void* result = InputParser_o( inputMessage , historyEntry , a3 , a4 , a5 , a6 );

	if ( g_inputParserCmd && inputMessage && historyEntry )
		RageSubTick::ApplyInputParser( g_inputParserCmd , inputMessage , historyEntry );

	return result;
}

#pragma once

#include <Common/Common.hpp>

class CUserCmd;
class CSGOInputHistoryEntryPB;

auto Hook_InputParser(
	void* inputMessage ,
	CSGOInputHistoryEntryPB* historyEntry ,
	char a3 ,
	void* a4 ,
	void* a5 ,
	void* a6 ) -> void*;

using InputParser_t = decltype( &Hook_InputParser );
inline InputParser_t InputParser_o = nullptr;

inline CUserCmd* g_inputParserCmd = nullptr;

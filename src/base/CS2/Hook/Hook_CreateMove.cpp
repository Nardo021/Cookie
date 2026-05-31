#include "Hook_CreateMove.hpp"

#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <Client/CCookieClient.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Bypass.hpp>

namespace
{
	thread_local int g_BypassSerializeDepth = 0;

	struct BypassSerializeGuard
	{
		BypassSerializeGuard() { ++g_BypassSerializeDepth; }
		~BypassSerializeGuard() { --g_BypassSerializeDepth; }
	};
}

auto Hook_CreateMove( CCSGOInput* pCCSGOInput , uint32_t split_screen_index , bool frame_active ) -> bool
{
	const auto Result = CreateMove_o( pCCSGOInput , split_screen_index , frame_active );

	BypassSerializeGuard guard;
	if ( g_BypassSerializeDepth > 1 )
		return Result;

	if ( !frame_active || !pCCSGOInput || !SDK::Interfaces::EngineToClient()->IsInGame() )
		return Result;

	if ( !GetCookieClient()->IsInitialized() )
		return Result;

	auto* pLocalPlayerController = GetCL_Players()->GetLocalPlayerController();
	if ( !pLocalPlayerController || !GetCL_Players()->GetLocalPlayerPawn() )
		return Result;

	if ( !GetCL_Players()->IsLocalPlayerAlive() )
		return Result;

	const auto pUserCmd = pCCSGOInput->GetUserCmd( pLocalPlayerController );
	if ( !pUserCmd )
		return Result;

	GetCL_Bypass()->PreClientCreateMove( pUserCmd );
	GetCookieClient()->OnCreateMove( pCCSGOInput , split_screen_index , pUserCmd );
	GetCL_Bypass()->PostClientCreateMove( pCCSGOInput , pUserCmd );

	return Result;
}

auto Hook_MessageLite_SerializePartialToArray( google::protobuf::Message* pMsg , void* out_buffer , int size ) -> bool
{
	BypassSerializeGuard guard;

#if DISABLE_PROTOBUF == 0
	if ( g_BypassSerializeDepth <= 1 && pMsg )
	{
		const google::protobuf::Descriptor* descriptor = pMsg->GetDescriptor();
		if ( descriptor && descriptor->name() == XorStr( "CBaseUserCmdPB" ) )
			GetCL_Bypass()->OnCBaseUserCmdPB( reinterpret_cast<CBaseUserCmdPB*>( pMsg ) );
	}
#endif

	return ProtobufSerializePartialToArrayOriginal( pMsg , out_buffer , size );
}
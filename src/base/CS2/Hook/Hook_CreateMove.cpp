#include "Hook_CreateMove.hpp"

#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Interface/IEngineToClient.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

#include <Client/CCookieClient.hpp>

#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Bypass.hpp>

auto Hook_CreateMove( CCSGOInput* pCCSGOInput , uint32_t split_screen_index , bool frame_active ) -> bool
{
	const auto Result = CreateMove_o( pCCSGOInput , split_screen_index , frame_active );

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
#if DISABLE_PROTOBUF == 0
	const google::protobuf::Descriptor* descriptor = pMsg->GetDescriptor();
	if ( !descriptor )
		return MessageLite_SerializePartialToArray_o( pMsg , out_buffer , size );

	const std::string message_name = descriptor->name();

	if ( message_name == XorStr( "CBaseUserCmdPB" ) )
	{
		GetCL_Bypass()->OnCBaseUserCmdPB( reinterpret_cast<CBaseUserCmdPB*>( pMsg ) );
	}
#endif

	return MessageLite_SerializePartialToArray_o( pMsg , out_buffer , size );
}

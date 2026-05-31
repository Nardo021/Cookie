#include "CL_Bypass.hpp"

#include <Common/Base64.h>

#include <CS2/SDK/Math/Math.hpp>

#include <CS2/SDK/Update/CUserCmd.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <CS2/SDK/Update/CGlobalVarsBase.hpp>

#include <CS2/SDK/Interface/CInputSystem.hpp>
#include <CS2/SDK/FunctionListSDK.hpp>

static CL_Bypass g_CL_Bypass{};

auto CL_Bypass::PreClientCreateMove( CUserCmd* pUserCmd ) -> void
{
	m_InternalSubTickList.clear();
	m_needsPostProcess = false;
}

auto CL_Bypass::PostClientCreateMove( CCSGOInput* pCCSGOInput , CUserCmd* pUserCmd ) -> void
{
	if ( !pUserCmd || !m_backupReady || !m_needsPostProcess )
		return;

	int EncodeLen = 0;

	const auto OriginalCrc = pUserCmd->cmd.base().move_crc();
	const auto OriginalCrcBase64 = base64( (unsigned char*)OriginalCrc.c_str() , OriginalCrc.size() , &EncodeLen );

	const auto SpoofedOriginalCrc = SpoofCrc();
	const auto SpoofedOriginalCrcBase64 = base64( (unsigned char*)SpoofedOriginalCrc.c_str() , SpoofedOriginalCrc.size() , &EncodeLen );

	if ( std::string( OriginalCrcBase64 ) != std::string( SpoofedOriginalCrcBase64 ) )
	{
		DEV_LOG( "[error] spoof move_crc\n" );
		free( OriginalCrcBase64 );
		free( SpoofedOriginalCrcBase64 );
		m_backupReady = false;
		return;
	}

	free( OriginalCrcBase64 );
	free( SpoofedOriginalCrcBase64 );

	for ( auto& SubTick : m_InternalSubTickList )
		AddSubtickMoveStep( pUserCmd , SubTick.m_Button , SubTick.m_Pressed , SubTick.m_flWhen );

	m_Backup.mutable_buttons_pb()->set_buttonstate1( pUserCmd->button_states.buttonstate1 );
	m_Backup.mutable_buttons_pb()->set_buttonstate2( pUserCmd->button_states.buttonstate2 );
	m_Backup.mutable_buttons_pb()->set_buttonstate3( pUserCmd->button_states.buttonstate3 );

	m_Backup.mutable_viewangles()->set_x( pUserCmd->cmd.base().viewangles().x() );
	m_Backup.mutable_viewangles()->set_y( pUserCmd->cmd.base().viewangles().y() );

	SyncModifiedCmdToBackup( pUserCmd );

	const auto SpoofedCrc = SpoofCrc();

	pUserCmd->cmd.mutable_base()->set_move_crc( SpoofedCrc );
}

auto CL_Bypass::SetViewAngles( QAngle* ViewAngles , CCSGOInput* pInput , CUserCmd* pUserCmd , bool AddSetViewAngles , bool OnlyInputHistory ) -> void
{
	Math::NormalizeAngles( *ViewAngles );
	Math::ClampAngles( *ViewAngles );

	const auto ViewAngleX = ( *ViewAngles ).m_x;
	const auto ViewAngleY = ( *ViewAngles ).m_y;

	if ( AddSetViewAngles && !OnlyInputHistory )
		CCSGOInput_SetViewAngles( pInput , 0 , *ViewAngles );

	m_needsPostProcess = true;

#if DISABLE_PROTOBUF == 0

	if ( !OnlyInputHistory )
	{
		pUserCmd->cmd.mutable_base()->mutable_viewangles()->set_x( ViewAngleX );
		pUserCmd->cmd.mutable_base()->mutable_viewangles()->set_y( ViewAngleY );
	}
	else
	{
		for ( auto i = 0; i < pUserCmd->cmd.input_history_size(); i++ )
		{
			auto pInputHistory = pUserCmd->cmd.mutable_input_history( i );

			if ( pInputHistory )
			{
				pInputHistory->mutable_view_angles()->set_x( ViewAngleX );
				pInputHistory->mutable_view_angles()->set_y( ViewAngleY );
			}
		}
	}

#endif
}

auto CL_Bypass::SetAttack( CUserCmd* pUserCmd , bool AddSubTIck ) -> void
{
	pUserCmd->button_states.buttonstate1 |= IN_ATTACK;
	pUserCmd->button_states.buttonstate2 |= IN_ATTACK;
	pUserCmd->button_states.buttonstate3 |= IN_ATTACK;

	m_needsPostProcess = true;

#if DISABLE_PROTOBUF == 0

	auto* buttonsPb = pUserCmd->cmd.mutable_base()->mutable_buttons_pb();
	buttonsPb->set_buttonstate1( pUserCmd->button_states.buttonstate1 );
	buttonsPb->set_buttonstate2( pUserCmd->button_states.buttonstate2 );
	buttonsPb->set_buttonstate3( pUserCmd->button_states.buttonstate3 );

	int historyIndex = 0;
	if ( pUserCmd->cmd.input_history_size() > 0 )
		historyIndex = pUserCmd->cmd.input_history_size() - 1;

	pUserCmd->cmd.set_attack1_start_history_index( historyIndex );

	if ( AddSubTIck )
	{
		AddProcessSubTick( IN_ATTACK , true );
		AddSubtickMoveStep( pUserCmd , IN_ATTACK , true , 0.f );
	}

#endif
}

auto CL_Bypass::SetDontAttack( CUserCmd* pUserCmd , bool AddSubTIck ) -> void
{
	pUserCmd->button_states.buttonstate1 &= ~IN_ATTACK;
	pUserCmd->button_states.buttonstate2 &= ~IN_ATTACK;
	pUserCmd->button_states.buttonstate3 &= ~IN_ATTACK;

	m_needsPostProcess = true;

#if DISABLE_PROTOBUF == 0

	auto* buttonsPb = pUserCmd->cmd.mutable_base()->mutable_buttons_pb();
	buttonsPb->set_buttonstate1( pUserCmd->button_states.buttonstate1 );
	buttonsPb->set_buttonstate2( pUserCmd->button_states.buttonstate2 );
	buttonsPb->set_buttonstate3( pUserCmd->button_states.buttonstate3 );

	if ( AddSubTIck )
	{
		AddProcessSubTick( IN_ATTACK , false );
		AddSubtickMoveStep( pUserCmd , IN_ATTACK , false , 0.f );
	}

#endif
}

auto CL_Bypass::SetJump( CUserCmd* pUserCmd , bool pressed , bool addSubTick , float when ) -> void
{
	if ( !pUserCmd )
		return;

	if ( pressed )
	{
		pUserCmd->button_states.buttonstate1 |= IN_JUMP;
		pUserCmd->button_states.buttonstate2 |= IN_JUMP;
		pUserCmd->button_states.buttonstate3 |= IN_JUMP;
	}
	else
	{
		pUserCmd->button_states.buttonstate1 &= ~IN_JUMP;
		pUserCmd->button_states.buttonstate2 &= ~IN_JUMP;
		pUserCmd->button_states.buttonstate3 &= ~IN_JUMP;
	}

	m_needsPostProcess = true;

#if DISABLE_PROTOBUF == 0

	auto* buttonsPb = pUserCmd->cmd.mutable_base()->mutable_buttons_pb();
	buttonsPb->set_buttonstate1( pUserCmd->button_states.buttonstate1 );
	buttonsPb->set_buttonstate2( pUserCmd->button_states.buttonstate2 );
	buttonsPb->set_buttonstate3( pUserCmd->button_states.buttonstate3 );

	if ( addSubTick )
	{
		AddProcessSubTick( IN_JUMP , pressed );
		AddSubtickMoveStep( pUserCmd , IN_JUMP , pressed , when );
	}

#endif
}

auto CL_Bypass::AddProcessSubTick( const uint64_t Button , const bool Pressed ) -> void
{
	m_needsPostProcess = true;
	m_InternalSubTickList.emplace_back( Button , Pressed , CalculateWhenValue() );
}

auto CL_Bypass::AddSubTickMove( CUserCmd* pUserCmd , const uint64_t button , const bool pressed , const float when ) -> void
{
	m_needsPostProcess = true;
	AddSubtickMoveStep( pUserCmd , button , pressed , when );
}

auto CL_Bypass::OnCBaseUserCmdPB( CBaseUserCmdPB* pBaseUserCmdPB ) -> void
{
	m_Backup = *pBaseUserCmdPB;
	m_backupReady = true;
}

auto CL_Bypass::CalculateWhenValue() -> float
{
	return 0.99f;
}

auto CL_Bypass::AddSubtickMoveStep( CUserCmd* pUserCmd , const uint64_t button , const bool pressed , const float when ) -> void
{
#if DISABLE_PROTOBUF == 0
	if ( !pUserCmd )
		return;

	auto* base = pUserCmd->cmd.mutable_base();
	if ( !base )
		return;

	CSubtickMoveStep* pSubtickMoveStep = base->add_subtick_moves();
	if ( !pSubtickMoveStep )
		return;

	pSubtickMoveStep->set_button( button );
	pSubtickMoveStep->set_pressed( pressed );
	pSubtickMoveStep->set_when( when );
#endif
}

auto CL_Bypass::SyncModifiedCmdToBackup( CUserCmd* pUserCmd ) -> void
{
#if DISABLE_PROTOBUF == 0
	if ( !pUserCmd )
		return;

	const auto& cmdBase = pUserCmd->cmd.base();

	m_Backup.set_forwardmove( cmdBase.forwardmove() );
	m_Backup.set_leftmove( cmdBase.leftmove() );

	m_Backup.mutable_subtick_moves()->Clear();
	for ( int i = 0; i < cmdBase.subtick_moves_size(); ++i )
		*m_Backup.add_subtick_moves() = cmdBase.subtick_moves( i );
#endif
}

auto CL_Bypass::SpoofCrc() -> std::string
{
	const auto size = static_cast<int>( m_Backup.ByteSizeLong() );

	std::vector<uint8_t> bytes;
	bytes.resize( size );

	if ( !m_Backup.SerializePartialToArray( bytes.data() , size ) )
		DEV_LOG( "SpoofCrc: #1\n" );

	return std::string( bytes.begin() , bytes.end() );
}

auto GetCL_Bypass() -> CL_Bypass*
{
	return &g_CL_Bypass;
}

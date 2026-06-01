#include "RageSubTick.hpp"

#include <cmath>

#include <buttons.hpp>

#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/TIER0/TIER0_Functions.hpp>
#include <CS2/Protobuf/cs_usercmd.pb.h>

#include <GameClient/CL_Bypass.hpp>

namespace RageSubTick
{
	namespace
	{
		struct CCSInputMessageLayout
		{
			int32_t  frameTickCount;
			float    frameTickFraction;
			int32_t  playerTickCount;
			float    playerTickFraction;
			Vector3  viewAngles;
		};

		auto ClampQAngle( QAngle& ang ) noexcept -> void
		{
			if ( ang.m_x > 89.f )
				ang.m_x = 89.f;
			if ( ang.m_x < -89.f )
				ang.m_x = -89.f;
			ang.m_y = Math::AngleNormalize( ang.m_y );
			ang.m_z = 0.f;
		}
	}

	auto BuildSeedTable() noexcept -> void
	{
		s_computedSeeds.clear();
		s_computedSeeds.reserve( 256 );

		auto* tier0 = GetTIER0Functions();
		if ( !tier0 || !tier0->RandomSeed_o || !tier0->RandomFloat_o )
			return;

		for ( int i = 0; i <= 255; ++i )
		{
			tier0->RandomSeed_o( static_cast<uint32_t>( i + 1 ) );
			const float spread = tier0->RandomFloat_o( 0.f , 1.f );
			const float angle = tier0->RandomFloat_o( 0.f , 6.28318530718f );
			s_computedSeeds.emplace_back( spread , angle );
		}
	}

	auto ResetTick() noexcept -> void
	{
		s_state.Reset();
	}

	auto SetSilentAim( const QAngle& angles ) noexcept -> void
	{
		s_state.bestPoint = angles;
		s_state.bestPointVec = { angles.m_x , angles.m_y , angles.m_z };
		s_state.command = CommandMsg::Silent;
	}

	auto SetRapidFire( const QAngle& angles ) noexcept -> void
	{
		s_state.bestPoint = angles;
		s_state.bestPointVec = { angles.m_x , angles.m_y , angles.m_z };
		s_state.command = CommandMsg::RapidFire;
	}

	auto SetBacktrack( int tick , float fraction ) noexcept -> void
	{
		s_state.useBacktrack = tick >= 0;
		s_state.backtrackTick = tick;
		s_state.backtrackFraction = fraction;
	}

	namespace
	{
		auto ApplyBacktrackInterp( CSGOInputHistoryEntryPB* historyEntry ) noexcept -> void
		{
			if ( !historyEntry || !s_state.useBacktrack )
				return;

			const int srcTick = s_state.backtrackTick;
			const int dstTick = s_state.backtrackTick + 1;
			const float fraction = s_state.backtrackFraction;

			if ( historyEntry->has_player_tick_count() )
				historyEntry->set_player_tick_count( srcTick );
			if ( historyEntry->has_player_tick_fraction() )
				historyEntry->set_player_tick_fraction( fraction );

			if ( historyEntry->has_cl_interp() )
			{
				auto* clInterp = historyEntry->mutable_cl_interp();
				if ( clInterp )
					clInterp->set_frac( fraction );
			}

			if ( historyEntry->has_sv_interp0() )
			{
				auto* sv0 = historyEntry->mutable_sv_interp0();
				if ( sv0 )
				{
					sv0->set_src_tick( srcTick );
					sv0->set_dst_tick( dstTick );
					sv0->set_frac( 0.f );
				}
			}

			if ( historyEntry->has_sv_interp1() )
			{
				auto* sv1 = historyEntry->mutable_sv_interp1();
				if ( sv1 )
				{
					sv1->set_src_tick( dstTick );
					sv1->set_dst_tick( dstTick + 1 );
					sv1->set_frac( 0.f );
				}
			}

			if ( historyEntry->has_player_interp() )
			{
				auto* playerInterp = historyEntry->mutable_player_interp();
				if ( playerInterp )
				{
					playerInterp->set_src_tick( srcTick );
					playerInterp->set_dst_tick( dstTick );
					playerInterp->set_frac( fraction );
				}
			}
		}
	}

	auto ApplyInputParser(
		CUserCmd* cmd ,
		void* inputMessage ,
		CSGOInputHistoryEntryPB* historyEntry ) noexcept -> void
	{
#if DISABLE_PROTOBUF == 0
		if ( !cmd || !inputMessage || !historyEntry )
			return;

		auto* input = reinterpret_cast<CCSInputMessageLayout*>( inputMessage );

		if ( s_state.command == CommandMsg::Silent )
		{
			input->viewAngles = s_state.bestPointVec;
			ClampQAngle( s_state.bestPoint );

			if ( historyEntry->has_view_angles() )
			{
				historyEntry->mutable_view_angles()->set_x( s_state.bestPoint.m_x );
				historyEntry->mutable_view_angles()->set_y( s_state.bestPoint.m_y );
				historyEntry->mutable_view_angles()->set_z( 0.f );
			}

			ApplyBacktrackInterp( historyEntry );
			s_state.response = ResponseMsg::ValidatedViewAngles;
		}
		else
		{
			s_state.response = ResponseMsg::Empty;
		}

		if ( s_state.command == CommandMsg::RapidFire )
		{
			input->viewAngles = s_state.bestPointVec;
			ClampQAngle( s_state.bestPoint );

			if ( historyEntry->has_view_angles() )
			{
				historyEntry->mutable_view_angles()->set_x( s_state.bestPoint.m_x );
				historyEntry->mutable_view_angles()->set_y( s_state.bestPoint.m_y );
				historyEntry->mutable_view_angles()->set_z( 0.f );
			}

			ApplyBacktrackInterp( historyEntry );

			if ( historyEntry->has_player_tick_count() )
				historyEntry->set_player_tick_count( 0 );

			input->playerTickCount = 0;
			GetCL_Bypass()->SetAttack( cmd , true );
		}
#else
		(void)cmd;
		(void)inputMessage;
		(void)historyEntry;
#endif
	}
}

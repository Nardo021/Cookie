#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"

#include <Client/Features/Misc/AntiAim.hpp>
#include <Client/Features/Movement/Bhop.hpp>
#include <Client/Features/Movement/Movement.hpp>
#include <Client/Game/Trace.hpp>

namespace SyntheticTabs
{
	auto RenderMovementTab() noexcept -> void
	{
		using namespace SyntheticUI;

		gui->begin_group();
		{
			gui->begin_child( "Anti-Aim" );
			{
				Checkbox( "Anti-Aim" , &AntiAim::config.enabled );
				if ( AntiAim::config.enabled )
				{
					static const auto aaModes = Strings( { "Static" , "Spin" } );
					Combo( "Mode" , &AntiAim::config.mode , aaModes );

					if ( AntiAim::config.mode == static_cast<int>( AntiAim::Mode::Static ) )
					{
						static const auto pitchModes = Strings( { "Custom" , "Down" , "Up" , "Zero" } );
						Combo( "Pitch Mode" , &AntiAim::config.pitchType , pitchModes );
						if ( AntiAim::config.pitchType == static_cast<int>( AntiAim::PitchType::None ) )
							SliderFloat( "Pitch" , &AntiAim::config.pitch , -89.f , 89.f , 1.f , "%.0f" );

						static const auto yawModes = Strings( { "Custom Offset" , "Backwards" , "Forwards" } );
						Combo( "Yaw Mode" , &AntiAim::config.yawType , yawModes );
						if ( AntiAim::config.yawType == static_cast<int>( AntiAim::YawType::None ) )
							SliderFloat( "Yaw" , &AntiAim::config.yaw , -180.f , 180.f , 1.f , "%.0f" );
					}
					else
					{
						SliderFloat( "Pitch" , &AntiAim::config.pitch , -89.f , 89.f , 1.f , "%.0f" );
						SliderFloat( "Spin Speed" , &AntiAim::config.spinSpeed , 1.f , 360.f , 1.f , "%.0f" );
					}
				}
			}
			gui->end_child();

			gui->begin_child( "Bhop" );
			{
				Checkbox( "Bunny Hop" , &Bhop::config.enabled );
				Checkbox( "Edge Jump" , &Bhop::config.edgeJump );
				Checkbox( "Jump Bug" , &Bhop::config.jumpBug );
				Checkbox( "Hold Space" , &Bhop::config.requireSpace );
				Checkbox( "Auto Forward" , &Bhop::config.autoForward );
				SliderInt( "Hop Chance" , &Bhop::config.hopChance , 0 , 100 , 1 , "%d%%" );
				if ( ( Bhop::NeedsTrace() || Movement::config.edgeBug ) && !Trace::ready )
				{
					draw->render_text(
						GetWindowDrawList() ,
						set->c_font.inter_medium[0] ,
						GetWindowPos() + SCALE( 0 , GetCursorPosY() ) ,
						GetWindowPos() + GetWindowSize() ,
						gui->get_clr( clr->c_other_clr.accent_clr ) ,
						"Trace unavailable for edge features." ,
						nullptr ,
						nullptr ,
						ImVec2( 0.f , 0.f ) );
				}
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Movement Advanced" );
			{
				Checkbox( "Movement Fix" , &Movement::config.movementFix );
				Checkbox( "Movement Correction" , &Movement::config.movementCorrection );
				Checkbox( "Validate Angles" , &Movement::config.validateAngles );
				Checkbox( "Edge Bug" , &Movement::config.edgeBug );
				if ( Movement::config.edgeBug )
				{
					Checkbox( "Edge Bug Use Key" , &Movement::config.edgeBugUseKey );
					if ( Movement::config.edgeBugUseKey )
						KeyBind( "Edge Bug Key" , &Movement::config.edgeBugKey );
				}

				static const auto strafeModes = Strings( { "Off" , "Legit" , "Rage" } );
				Combo( "Auto Strafe Mode" , reinterpret_cast<int*>( &Movement::config.strafeMode ) , strafeModes );
				if ( Movement::config.strafeMode != Movement::StrafeMode::Off )
				{
					SliderFloat( "Strafe Smoothing" , &Movement::config.strafeSmooth , 0.f , 100.f , 1.f , "%.0f" );
					Checkbox( "Strafe Assist (WASD)" , &Movement::config.strafeAssist );
				}
			}
			gui->end_child();
		}
		gui->end_group();
	}
}

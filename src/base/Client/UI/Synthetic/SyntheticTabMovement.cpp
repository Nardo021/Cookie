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
				UiCheckbox( "Anti-Aim" , &AntiAim::config.enabled );
				if ( AntiAim::config.enabled )
				{
					static const auto aaModes = ItemStrings( { "Static" , "Spin" } );
					UiCombo( "Mode" , &AntiAim::config.mode , aaModes );

					if ( AntiAim::config.mode == static_cast<int>( AntiAim::Mode::Static ) )
					{
						static const auto pitchModes = ItemStrings( { "Custom" , "Down" , "Up" , "Zero" } );
						UiCombo( "Pitch Mode" , &AntiAim::config.pitchType , pitchModes );
						if ( AntiAim::config.pitchType == static_cast<int>( AntiAim::PitchType::None ) )
							UiSliderFloat( "Pitch" , &AntiAim::config.pitch , -89.f , 89.f , 1.f , "%.0f" );

						static const auto yawModes = ItemStrings( { "Custom Offset" , "Backwards" , "Forwards" } );
						UiCombo( "Yaw Mode" , &AntiAim::config.yawType , yawModes );
						if ( AntiAim::config.yawType == static_cast<int>( AntiAim::YawType::None ) )
							UiSliderFloat( "Yaw" , &AntiAim::config.yaw , -180.f , 180.f , 1.f , "%.0f" );
					}
					else
					{
						UiSliderFloat( "Pitch" , &AntiAim::config.pitch , -89.f , 89.f , 1.f , "%.0f" );
						UiSliderFloat( "Spin Speed" , &AntiAim::config.spinSpeed , 1.f , 360.f , 1.f , "%.0f" );
					}
				}
			}
			gui->end_child();

			gui->begin_child( "Bhop" );
			{
				UiCheckbox( "Bunny Hop" , &Bhop::config.enabled );
				UiCheckbox( "Edge Jump" , &Bhop::config.edgeJump );
				UiCheckbox( "Jump Bug" , &Bhop::config.jumpBug );
				UiCheckbox( "Hold Space" , &Bhop::config.requireSpace );
				UiCheckbox( "Auto Forward" , &Bhop::config.autoForward );
				UiSliderInt( "Hop Chance" , &Bhop::config.hopChance , 0 , 100 , 1 , "%d%%" );
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
				UiCheckbox( "Movement Fix" , &Movement::config.movementFix );
				UiCheckbox( "Movement Correction" , &Movement::config.movementCorrection );
				UiCheckbox( "Validate Angles" , &Movement::config.validateAngles );
				UiCheckbox( "Edge Bug" , &Movement::config.edgeBug );
				if ( Movement::config.edgeBug )
				{
					UiCheckbox( "Edge Bug Use Key" , &Movement::config.edgeBugUseKey );
					if ( Movement::config.edgeBugUseKey )
						UiKeyBind( "Edge Bug Key" , &Movement::config.edgeBugKey );
				}

				static const auto strafeModes = ItemStrings( { "Off" , "Legit" , "Rage" } );
				UiCombo( "Auto Strafe Mode" , reinterpret_cast<int*>( &Movement::config.strafeMode ) , strafeModes );
				if ( Movement::config.strafeMode != Movement::StrafeMode::Off )
				{
					UiSliderFloat( "Strafe Smoothing" , &Movement::config.strafeSmooth , 0.f , 100.f , 1.f , "%.0f" );
					UiCheckbox( "Strafe Assist (WASD)" , &Movement::config.strafeAssist );
				}
			}
			gui->end_child();
		}
		gui->end_group();
	}
}


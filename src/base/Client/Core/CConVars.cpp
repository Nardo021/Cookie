#include "CConVars.hpp"

#include <CS2/SDK/SDK.hpp>

namespace CookieCore
{
	namespace detail
	{
		static auto FindConVar( IEngineCVar* cvar , const char* name ) -> CConVar*
		{
			if ( !cvar || !name )
				return nullptr;

			return cvar->Find( name );
		}

		static auto RequireConVar( bool& success , CConVar*& out , IEngineCVar* cvar , const char* name ) -> void
		{
			out = FindConVar( cvar , name );
			success &= out != nullptr;
		}
	}

	auto CConVars::Setup() -> bool
	{
		auto* cvar = SDK::Interfaces::EngineCvar();
		if ( !cvar )
		{
			DEV_LOG( XorStr( "[error] CConVars::Setup: EngineCvar unavailable\n" ) );
			return false;
		}

		bool success = true;

		detail::RequireConVar( success , mp_teammates_are_enemies , cvar , XorStr( "mp_teammates_are_enemies" ) );
		detail::RequireConVar( success , cl_interp , cvar , XorStr( "cl_interp" ) );
		detail::RequireConVar( success , cl_interp_ratio , cvar , XorStr( "cl_interp_ratio" ) );
		cl_updaterate = detail::FindConVar( cvar , XorStr( "cl_updaterate" ) );
		sv_maxunlag = detail::FindConVar( cvar , XorStr( "sv_maxunlag" ) );
		r_modelAmbientMin = detail::FindConVar( cvar , XorStr( "r_modelAmbientMin" ) );
		r_shadows = detail::FindConVar( cvar , XorStr( "r_shadows" ) );
		fog_override = detail::FindConVar( cvar , XorStr( "fog_override" ) );
		fog_enable = detail::FindConVar( cvar , XorStr( "fog_enable" ) );
		fog_maxdensity = detail::FindConVar( cvar , XorStr( "fog_maxdensity" ) );
		fog_start = detail::FindConVar( cvar , XorStr( "fog_start" ) );
		fog_end = detail::FindConVar( cvar , XorStr( "fog_end" ) );
		fog_color = detail::FindConVar( cvar , XorStr( "fog_color" ) );
		detail::RequireConVar( success , sv_autobunnyhopping , cvar , XorStr( "sv_autobunnyhopping" ) );
		detail::RequireConVar( success , cam_idealdist , cvar , XorStr( "cam_idealdist" ) );
		detail::RequireConVar( success , c_thirdpersonshoulder , cvar , XorStr( "c_thirdpersonshoulder" ) );
		detail::RequireConVar( success , c_thirdpersonshoulderaimdist , cvar , XorStr( "c_thirdpersonshoulderaimdist" ) );
		detail::RequireConVar( success , c_thirdpersonshoulderdist , cvar , XorStr( "c_thirdpersonshoulderdist" ) );
		detail::RequireConVar( success , c_thirdpersonshoulderheight , cvar , XorStr( "c_thirdpersonshoulderheight" ) );
		detail::RequireConVar( success , c_thirdpersonshoulderoffset , cvar , XorStr( "c_thirdpersonshoulderoffset" ) );
		detail::RequireConVar( success , m_pitch , cvar , XorStr( "m_pitch" ) );
		detail::RequireConVar( success , m_yaw , cvar , XorStr( "m_yaw" ) );
		detail::RequireConVar( success , sensitivity , cvar , XorStr( "sensitivity" ) );
		sv_min_jump_landing_sound = detail::FindConVar( cvar , XorStr( "sv_min_jump_landing_sound" ) );
		sv_standable_normal = detail::FindConVar( cvar , XorStr( "sv_standable_normal" ) );
		detail::RequireConVar( success , mp_damage_scale_ct_head , cvar , XorStr( "mp_damage_scale_ct_head" ) );
		detail::RequireConVar( success , mp_damage_scale_t_head , cvar , XorStr( "mp_damage_scale_t_head" ) );
		detail::RequireConVar( success , mp_damage_scale_ct_body , cvar , XorStr( "mp_damage_scale_ct_body" ) );
		detail::RequireConVar( success , mp_damage_scale_t_body , cvar , XorStr( "mp_damage_scale_t_body" ) );

		if ( !success )
			DEV_LOG( XorStr( "[warn] CConVars::Setup: one or more convars were not found\n" ) );

		return success;
	}

	auto CConVars::Dump() -> void
	{
		auto dumpOne = []( const char* name , CConVar* cv )
		{
			if ( !cv )
			{
				DEV_LOG( "[ConVar] %s = (missing)\n" , name );
				return;
			}

			DEV_LOG( "[ConVar] %s = fl:%.4f i:%d s:%s\n" , name , cv->value.fl , cv->value.i1 , cv->value.sz );
		};

		dumpOne( "mp_teammates_are_enemies" , mp_teammates_are_enemies );
		dumpOne( "cl_interp" , cl_interp );
		dumpOne( "cl_interp_ratio" , cl_interp_ratio );
		dumpOne( "cl_updaterate" , cl_updaterate );
		dumpOne( "sv_autobunnyhopping" , sv_autobunnyhopping );
		dumpOne( "m_pitch" , m_pitch );
		dumpOne( "m_yaw" , m_yaw );
		dumpOne( "sensitivity" , sensitivity );
		dumpOne( "sv_standable_normal" , sv_standable_normal );
		dumpOne( "sv_min_jump_landing_sound" , sv_min_jump_landing_sound );
		dumpOne( "mp_damage_scale_ct_head" , mp_damage_scale_ct_head );
		dumpOne( "mp_damage_scale_t_head" , mp_damage_scale_t_head );
		dumpOne( "mp_damage_scale_ct_body" , mp_damage_scale_ct_body );
		dumpOne( "mp_damage_scale_t_body" , mp_damage_scale_t_body );
	}
}

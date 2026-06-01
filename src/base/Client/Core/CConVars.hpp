#pragma once

#include <Common/Common.hpp>

#include <CS2/SDK/Interface/IEngineCvar.hpp>

namespace CookieCore
{
	class CConVars final
	{
	public:
		static auto Setup() -> bool;
		static auto Dump() -> void;

		static auto Get( CConVar* const var ) -> CConVar*
		{
			return var;
		}

	public:
		static inline CConVar* mp_teammates_are_enemies = nullptr;
		static inline CConVar* cl_interp = nullptr;
		static inline CConVar* cl_interp_ratio = nullptr;
		static inline CConVar* cl_updaterate = nullptr;
		static inline CConVar* sv_maxunlag = nullptr;
		static inline CConVar* r_modelAmbientMin = nullptr;
		static inline CConVar* r_shadows = nullptr;
		static inline CConVar* fog_override = nullptr;
		static inline CConVar* fog_enable = nullptr;
		static inline CConVar* fog_maxdensity = nullptr;
		static inline CConVar* fog_start = nullptr;
		static inline CConVar* fog_end = nullptr;
		static inline CConVar* fog_color = nullptr;
		static inline CConVar* sv_autobunnyhopping = nullptr;
		static inline CConVar* cam_idealdist = nullptr;
		static inline CConVar* c_thirdpersonshoulder = nullptr;
		static inline CConVar* c_thirdpersonshoulderaimdist = nullptr;
		static inline CConVar* c_thirdpersonshoulderdist = nullptr;
		static inline CConVar* c_thirdpersonshoulderheight = nullptr;
		static inline CConVar* c_thirdpersonshoulderoffset = nullptr;
		static inline CConVar* m_pitch = nullptr;
		static inline CConVar* m_yaw = nullptr;
		static inline CConVar* sensitivity = nullptr;
		static inline CConVar* sv_min_jump_landing_sound = nullptr;
		static inline CConVar* sv_standable_normal = nullptr;
		static inline CConVar* mp_damage_scale_ct_head = nullptr;
		static inline CConVar* mp_damage_scale_t_head = nullptr;
		static inline CConVar* mp_damage_scale_ct_body = nullptr;
		static inline CConVar* mp_damage_scale_t_body = nullptr;
	};
}

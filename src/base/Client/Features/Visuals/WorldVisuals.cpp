#include "WorldVisuals.hpp"

#include <CS2/SDK/Types/Color.hpp>

#include <Client/Core/CConVars.hpp>

namespace WorldVisuals
{
	namespace
	{
		struct SavedConVars
		{
			bool  captured = false;
			float ambientMin = 0.f;
			float shadows = 1.f;
			float fogOverride = 0.f;
			float fogEnable = 0.f;
			float fogMaxDensity = 0.f;
			float fogStart = 0.f;
			float fogEnd = 0.f;
			Color fogColor{};
		};

		SavedConVars s_saved{};

		auto CaptureDefaults() noexcept -> void
		{
			if ( s_saved.captured )
				return;

			if ( CookieCore::CConVars::r_modelAmbientMin )
				s_saved.ambientMin = CookieCore::CConVars::r_modelAmbientMin->value.fl;
			if ( CookieCore::CConVars::r_shadows )
				s_saved.shadows = CookieCore::CConVars::r_shadows->value.fl;
			if ( CookieCore::CConVars::fog_override )
				s_saved.fogOverride = CookieCore::CConVars::fog_override->value.fl;
			if ( CookieCore::CConVars::fog_enable )
				s_saved.fogEnable = CookieCore::CConVars::fog_enable->value.fl;
			if ( CookieCore::CConVars::fog_maxdensity )
				s_saved.fogMaxDensity = CookieCore::CConVars::fog_maxdensity->value.fl;
			if ( CookieCore::CConVars::fog_start )
				s_saved.fogStart = CookieCore::CConVars::fog_start->value.fl;
			if ( CookieCore::CConVars::fog_end )
				s_saved.fogEnd = CookieCore::CConVars::fog_end->value.fl;
			if ( CookieCore::CConVars::fog_color )
				s_saved.fogColor = CookieCore::CConVars::fog_color->value.clr;

			s_saved.captured = true;
		}

		auto RestoreDefaults() noexcept -> void
		{
			if ( !s_saved.captured )
				return;

			if ( CookieCore::CConVars::r_modelAmbientMin )
				CookieCore::CConVars::r_modelAmbientMin->value.fl = s_saved.ambientMin;
			if ( CookieCore::CConVars::r_shadows )
				CookieCore::CConVars::r_shadows->value.fl = s_saved.shadows;
			if ( CookieCore::CConVars::fog_override )
				CookieCore::CConVars::fog_override->value.fl = s_saved.fogOverride;
			if ( CookieCore::CConVars::fog_enable )
				CookieCore::CConVars::fog_enable->value.fl = s_saved.fogEnable;
			if ( CookieCore::CConVars::fog_maxdensity )
				CookieCore::CConVars::fog_maxdensity->value.fl = s_saved.fogMaxDensity;
			if ( CookieCore::CConVars::fog_start )
				CookieCore::CConVars::fog_start->value.fl = s_saved.fogStart;
			if ( CookieCore::CConVars::fog_end )
				CookieCore::CConVars::fog_end->value.fl = s_saved.fogEnd;
			if ( CookieCore::CConVars::fog_color )
				CookieCore::CConVars::fog_color->value.clr = s_saved.fogColor;
		}

		auto ApplyFogModulate() noexcept -> void
		{
			if ( !config.worldModulate )
				return;

			if ( CookieCore::CConVars::fog_override )
				CookieCore::CConVars::fog_override->value.fl = 1.f;
			if ( CookieCore::CConVars::fog_enable )
				CookieCore::CConVars::fog_enable->value.fl = 1.f;
			if ( CookieCore::CConVars::fog_maxdensity )
			{
				const float density = config.modulateColor[3] > 0.01f ? config.modulateColor[3] : 0.85f;
				CookieCore::CConVars::fog_maxdensity->value.fl = density;
			}
			if ( CookieCore::CConVars::fog_start )
				CookieCore::CConVars::fog_start->value.fl = -500.f;
			if ( CookieCore::CConVars::fog_end )
				CookieCore::CConVars::fog_end->value.fl = 5000.f;
			if ( CookieCore::CConVars::fog_color )
			{
				CookieCore::CConVars::fog_color->value.clr = Color(
					static_cast<int>( config.modulateColor[0] * 255.f ) ,
					static_cast<int>( config.modulateColor[1] * 255.f ) ,
					static_cast<int>( config.modulateColor[2] * 255.f ) ,
					255 );
			}
		}

		auto ApplyActive() noexcept -> void
		{
			CaptureDefaults();

			const bool active = config.nightMode || config.worldModulate || config.noShadow;
			if ( !active )
			{
				RestoreDefaults();
				return;
			}

			if ( CookieCore::CConVars::r_modelAmbientMin )
			{
				CookieCore::CConVars::r_modelAmbientMin->value.fl = config.nightMode
					? config.nightAmbient
					: s_saved.ambientMin;
			}

			if ( CookieCore::CConVars::r_shadows )
			{
				CookieCore::CConVars::r_shadows->value.fl = config.noShadow
					? 0.f
					: s_saved.shadows;
			}

			if ( config.worldModulate )
				ApplyFogModulate();
			else
			{
				if ( CookieCore::CConVars::fog_override )
					CookieCore::CConVars::fog_override->value.fl = s_saved.fogOverride;
				if ( CookieCore::CConVars::fog_enable )
					CookieCore::CConVars::fog_enable->value.fl = s_saved.fogEnable;
				if ( CookieCore::CConVars::fog_maxdensity )
					CookieCore::CConVars::fog_maxdensity->value.fl = s_saved.fogMaxDensity;
				if ( CookieCore::CConVars::fog_start )
					CookieCore::CConVars::fog_start->value.fl = s_saved.fogStart;
				if ( CookieCore::CConVars::fog_end )
					CookieCore::CConVars::fog_end->value.fl = s_saved.fogEnd;
				if ( CookieCore::CConVars::fog_color )
					CookieCore::CConVars::fog_color->value.clr = s_saved.fogColor;
			}
		}
	}

	auto OnFrameStageNotify( int frameStage ) noexcept -> void
	{
		if ( frameStage != 6 )
			return;

		ApplyActive();
	}

	auto Shutdown() noexcept -> void
	{
		RestoreDefaults();
		s_saved.captured = false;
	}

} // namespace WorldVisuals

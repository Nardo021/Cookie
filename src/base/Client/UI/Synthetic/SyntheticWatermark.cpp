#include "SyntheticWatermark.hpp"

#include <cstdio>

#include <Common/Include/Config.hpp>
#include <framework/settings/functions.h>

namespace SyntheticWatermark
{
	auto Update() noexcept -> void
	{
		const ImGuiIO& io = ImGui::GetIO();

		char fpsBuf[32];
		snprintf( fpsBuf , sizeof( fpsBuf ) , "%.0f FPS" , io.Framerate );

		var->c_watermark.watermark_content = {
			CHEAT_NAME ,
			fpsBuf ,
			gui->get_current_date() ,
			"" ,
		};
	}

	auto Render() noexcept -> void
	{
		Update();
		gui->water_mark(
			"watermark" ,
			var->c_watermark.watermark_content ,
			static_cast<watermark_position>( var->c_watermark.watermark_position ) ,
			&var->c_watermark.watermark );
	}
}

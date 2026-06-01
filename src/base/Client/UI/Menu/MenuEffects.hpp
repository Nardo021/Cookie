#pragma once

#include <cstddef>
#include <vector>

#include <ImGui/imgui.h>

namespace MenuEffects
{
	struct Config
	{
		bool  watermark = true;
		bool  particles = false;
		bool  shaderBlur = false;
		bool  blurPlaceholder = false;
		bool  menuBackgroundImage = false;
		float menuBackgroundAlpha = 0.35f;
		int   maxParticles = 80;
		float particleLinkDistance = 120.f;
	};

	inline Config config;

	struct Particle
	{
		ImVec2 position{};
		ImVec2 velocity{};
	};

	class ParticleBackground
	{
	public:
		void Ensure( int count , const ImVec2& screenSize );
		void Reset( int count , const ImVec2& screenSize );
		void Update( const ImVec2& screenSize , float dt );
		void Render( ImDrawList* draw , float alpha ) const;

	private:
		std::vector<Particle> particles_;
	};

	inline ImU32 PanelBlurTintColor() noexcept { return IM_COL32( 12 , 14 , 22 , 180 ); }

	inline auto DrawPanelBlurOverlay( ImDrawList* draw , ImVec2 min , ImVec2 max , float rounding ) -> void
	{
		if ( !draw )
			return;
		draw->AddRectFilled( min , max , PanelBlurTintColor() , rounding );
	}
	auto RenderWatermark() -> void;
	auto RenderMenuBackground( ImVec2 size , const ImVec2* originOverride = nullptr ) -> void;

} // namespace MenuEffects

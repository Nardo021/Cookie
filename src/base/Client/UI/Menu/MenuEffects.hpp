#pragma once

#include <cstddef>
#include <vector>

#include <ImGui/imgui.h>

namespace MenuEffects
{
	struct Config
	{
		bool  watermark = true;
		bool  particles = true;
		bool  shaderBlur = true;
		bool  blurPlaceholder = true;
		bool  menuBackgroundImage = true;
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

	auto RenderWatermark() -> void;
	auto RenderMenuBackground( ImVec2 size , const ImVec2* originOverride = nullptr ) -> void;

} // namespace MenuEffects

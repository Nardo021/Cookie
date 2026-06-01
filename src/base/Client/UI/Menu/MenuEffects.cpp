#include "MenuEffects.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#include <Common/Include/Config.hpp>

#include <Client/Fonts/EmbeddedFonts.hpp>
#include <Client/UI/Menu/MenuAssets.hpp>

namespace MenuEffects
{
	namespace
	{
		inline ParticleBackground s_particles;

		auto RandomRange( float minV , float maxV ) -> float
		{
			return minV + ( maxV - minV ) * ( static_cast<float>( rand() ) / static_cast<float>( RAND_MAX ) );
		}
	}

	void ParticleBackground::Ensure( int count , const ImVec2& screenSize )
	{
		if ( particles_.empty() )
			Reset( count , screenSize );
	}

	void ParticleBackground::Reset( int count , const ImVec2& screenSize )
	{
		particles_.clear();
		particles_.reserve( static_cast<std::size_t>( count ) );

		for ( int i = 0; i < count; ++i )
		{
			Particle p;
			p.position = ImVec2( RandomRange( 0.f , screenSize.x ) , RandomRange( 0.f , screenSize.y ) );
			p.velocity = ImVec2( RandomRange( -40.f , 40.f ) , RandomRange( -40.f , 40.f ) );
			particles_.push_back( p );
		}
	}

	void ParticleBackground::Update( const ImVec2& screenSize , float dt )
	{
		for ( auto& p : particles_ )
		{
			p.position.x += p.velocity.x * dt;
			p.position.y += p.velocity.y * dt;

			if ( p.position.x <= 0.f || p.position.x >= screenSize.x )
				p.velocity.x = -p.velocity.x;
			if ( p.position.y <= 0.f || p.position.y >= screenSize.y )
				p.velocity.y = -p.velocity.y;

			p.position.x = std::clamp( p.position.x , 0.f , screenSize.x );
			p.position.y = std::clamp( p.position.y , 0.f , screenSize.y );
		}
	}

	void ParticleBackground::Render( ImDrawList* draw , float alpha ) const
	{
		if ( !draw )
			return;

		const ImU32 dotColor = IM_COL32( 120 , 170 , 255 , static_cast<int>( 180.f * alpha ) );
		const ImU32 lineColorBase = IM_COL32( 80 , 120 , 200 , 255 );

		for ( const auto& p : particles_ )
			draw->AddCircleFilled( p.position , 2.f , dotColor , 8 );

		const float maxDist = config.particleLinkDistance;
		const float maxDistSq = maxDist * maxDist;

		for ( std::size_t i = 0; i < particles_.size(); ++i )
		{
			for ( std::size_t j = i + 1; j < particles_.size(); ++j )
			{
				const ImVec2 delta(
					particles_[i].position.x - particles_[j].position.x ,
					particles_[i].position.y - particles_[j].position.y );
				const float distSq = delta.x * delta.x + delta.y * delta.y;
				if ( distSq > maxDistSq )
					continue;

				const float dist = sqrtf( distSq );
				const float linkAlpha = ( 1.f - dist / maxDist ) * alpha;
				const ImU32 lineColor = ( lineColorBase & 0x00FFFFFFu )
					| ( static_cast<ImU32>( linkAlpha * 255.f ) << 24 );
				draw->AddLine( particles_[i].position , particles_[j].position , lineColor , 1.f );
			}
		}
	}

	auto RenderWatermark() -> void
	{
		if ( !config.watermark )
			return;

		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		if ( !draw )
			return;

		const ImGuiIO& io = ImGui::GetIO();
		char text[128];
		snprintf(
			text , sizeof( text ) , "%s | %.0f FPS" ,
			CHEAT_NAME ,
			io.Framerate );

		ImFont* font = ImGui::GetFont();
		if ( !EmbeddedFonts::FontReady( font ) )
			return;

		const ImVec2 textSize = ImGui::CalcTextSize( text );
		const ImVec2 pos( io.DisplaySize.x - textSize.x - 12.f , 8.f );

		draw->AddRectFilled(
			ImVec2( pos.x - 6.f , pos.y - 2.f ) ,
			ImVec2( pos.x + textSize.x + 6.f , pos.y + textSize.y + 2.f ) ,
			IM_COL32( 0 , 0 , 0 , 120 ) ,
			4.f );
		draw->AddText( pos , IM_COL32( 255 , 255 , 255 , 230 ) , text );
	}

	auto RenderMenuBackground( ImVec2 size , const ImVec2* originOverride ) -> void
	{
		ImDrawList* draw = ImGui::GetWindowDrawList();
		if ( !draw )
			return;

		const ImVec2 origin = originOverride ? *originOverride : ImGui::GetWindowPos();

		if ( config.menuBackgroundImage && MenuAssets::GetMenuBackgroundTexture() )
			MenuAssets::RenderMenuBackgroundImage( draw , origin , size , config.menuBackgroundAlpha );

		if ( config.blurPlaceholder && !config.shaderBlur )
			DrawPanelBlurOverlay( draw , origin , ImVec2( origin.x + size.x , origin.y + size.y ) , 8.f );

		if ( !config.particles )
			return;

		s_particles.Ensure( config.maxParticles , size );
		s_particles.Update( size , ImGui::GetIO().DeltaTime );
		s_particles.Render( draw , 0.85f );
	}
}

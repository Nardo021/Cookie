#include "SyntheticEspPreview.hpp"

#include <algorithm>

#include <Client/Features/Visuals/EspOverlay.hpp>
#include <framework/settings/functions.h>

namespace SyntheticEspPreview
{
	namespace
	{
		auto ColorFromConfig( const float color[4] ) noexcept -> ImVec4
		{
			return ImVec4( color[0] , color[1] , color[2] , color[3] );
		}

		auto AlignToPreviewPosition( int alignSide ) noexcept -> int
		{
			switch ( static_cast<EspOverlay::AlignSide>( std::clamp( alignSide , 0 , 3 ) ) )
			{
			case EspOverlay::AlignSide::Top: return position_top;
			case EspOverlay::AlignSide::Bottom: return position_bottom;
			case EspOverlay::AlignSide::Left: return position_left;
			case EspOverlay::AlignSide::Right: return position_right;
			default: return position_top;
			}
		}

		auto PreviewPositionToAlign( int previewPos ) noexcept -> int
		{
			switch ( previewPos )
			{
			case position_top: return static_cast<int>( EspOverlay::AlignSide::Top );
			case position_bottom: return static_cast<int>( EspOverlay::AlignSide::Bottom );
			case position_left: return static_cast<int>( EspOverlay::AlignSide::Left );
			case position_right: return static_cast<int>( EspOverlay::AlignSide::Right );
			default: return static_cast<int>( EspOverlay::AlignSide::Top );
			}
		}

		auto FindItem( std::vector<item_state>& items , const char* name ) noexcept -> item_state*
		{
			for ( auto& item : items )
			{
				if ( item.name == name )
					return &item;
			}
			return nullptr;
		}

		auto RebuildPreviewItems() noexcept -> void
		{
			esp->text.clear();
			esp->bar.clear();

			auto& cfg = EspOverlay::config;

			if ( cfg.showName )
			{
				esp->text.push_back( {
					"NAME" ,
					ColorFromConfig( cfg.textColor ) ,
					AlignToPreviewPosition( cfg.nameSide ) ,
					ImVec2() ,
					false ,
					false ,
					ImRect() } );
			}

			if ( cfg.showDistance )
			{
				esp->text.push_back( {
					"DISTANCE" ,
					ColorFromConfig( cfg.textColor ) ,
					AlignToPreviewPosition( cfg.distanceSide ) ,
					ImVec2() ,
					false ,
					false ,
					ImRect() } );
			}

			if ( cfg.showHelmetFlag || cfg.showKitFlag )
			{
				esp->text.push_back( {
					"FLAGS" ,
					ImVec4( 0.47f , 0.78f , 1.f , 1.f ) ,
					AlignToPreviewPosition( cfg.flagsSide ) ,
					ImVec2() ,
					false ,
					false ,
					ImRect() } );
			}

			if ( cfg.showHealthBar )
			{
				esp->bar.push_back( {
					"HEALTH" ,
					ColorFromConfig( cfg.healthBarColor ) ,
					AlignToPreviewPosition( cfg.healthBarSide ) ,
					ImVec2() ,
					false ,
					false ,
					ImRect() } );
			}

			if ( cfg.showAmmoBar )
			{
				esp->bar.push_back( {
					"AMMO" ,
					ColorFromConfig( cfg.ammoBarColor ) ,
					AlignToPreviewPosition( cfg.ammoBarSide ) ,
					ImVec2() ,
					false ,
					false ,
					ImRect() } );
			}
		}

		auto ApplyLayoutToConfig() noexcept -> void
		{
			auto& cfg = EspOverlay::config;

			if ( auto* name = FindItem( esp->text , "NAME" ) )
				cfg.nameSide = PreviewPositionToAlign( name->position );
			if ( auto* dist = FindItem( esp->text , "DISTANCE" ) )
				cfg.distanceSide = PreviewPositionToAlign( dist->position );
			if ( auto* flags = FindItem( esp->text , "FLAGS" ) )
				cfg.flagsSide = PreviewPositionToAlign( flags->position );
			if ( auto* health = FindItem( esp->bar , "HEALTH" ) )
				cfg.healthBarSide = PreviewPositionToAlign( health->position );
			if ( auto* ammo = FindItem( esp->bar , "AMMO" ) )
				cfg.ammoBarSide = PreviewPositionToAlign( ammo->position );
		}
	}

	auto RenderPanel( const ImVec2& origin , const ImVec2& size ) noexcept -> void
	{
		if ( size.x < 40.f || size.y < 40.f )
			return;

		RebuildPreviewItems();

		const ImVec2 boxSize( size.x * 0.35f , size.y * 0.55f );
		const ImVec2 boxPos(
			origin.x + ( size.x - boxSize.x ) * 0.5f ,
			origin.y + ( size.y - boxSize.y ) * 0.5f );

		esp->initialize_preview(
			boxPos ,
			boxSize ,
			var->c_preview.flags ,
			var->c_preview.bars );

		ApplyLayoutToConfig();
	}
}

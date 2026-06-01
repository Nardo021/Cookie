#include "SyntheticTabs.hpp"
#include "SyntheticTabCommon.hpp"
#include "SyntheticSkinWeapons.hpp"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <mutex>

#include <Client/CCookieGUI.hpp>
#include <Client/Features/Inventory/Gloves.hpp>
#include <Client/Features/Inventory/PaintKits.hpp>
#include <Client/Features/Inventory/SkinChanger.hpp>
#include <Client/Features/Inventory/TextureOverride.hpp>
#include <GameClient/CL_ItemDefinition.hpp>

namespace SyntheticTabs
{
	namespace
	{
		int s_selectedWeapon = 0;
		char s_skinSearch[64] = "";
		int s_rarityFilter = 0;
		int s_paintKitPick = 0;
		std::vector<std::string> s_paintKitLabels;
		std::vector<int> s_paintKitIds;
		char s_lastSearch[64] = "";
		int s_lastRarity = -1;

		auto RebuildPaintKitList( int currentPaintKit ) noexcept -> void
		{
			s_paintKitLabels.clear();
			s_paintKitIds.clear();

			std::string searchLower = s_skinSearch;
			for ( char& c : searchLower )
				c = static_cast<char>( std::tolower( static_cast<unsigned char>( c ) ) );

			for ( const auto& pk : g_PaintKits )
			{
				if ( s_rarityFilter > 0 && static_cast<int>( pk.rarity ) != s_rarityFilter - 1 )
					continue;

				if ( !searchLower.empty() )
				{
					std::string nameLower = pk.name;
					for ( char& c : nameLower )
						c = static_cast<char>( std::tolower( static_cast<unsigned char>( c ) ) );
					if ( nameLower.find( searchLower ) == std::string::npos )
						continue;
				}

				char label[160];
				snprintf( label , sizeof( label ) , "[%d] %s" , pk.id , pk.name );
				s_paintKitLabels.emplace_back( label );
				s_paintKitIds.push_back( pk.id );
			}

			s_paintKitPick = 0;
			for ( int i = 0; i < static_cast<int>( s_paintKitIds.size() ); ++i )
			{
				if ( s_paintKitIds[static_cast<size_t>( i )] == currentPaintKit )
				{
					s_paintKitPick = i;
					break;
				}
			}
		}

		auto PaintKitPicker( SkinChanger::SkinConfig& cfg ) noexcept -> void
		{
			using namespace SyntheticUI;

			if ( std::strcmp( s_lastSearch , s_skinSearch ) != 0 || s_lastRarity != s_rarityFilter )
			{
				std::strncpy( s_lastSearch , s_skinSearch , sizeof( s_lastSearch ) - 1 );
				s_lastRarity = s_rarityFilter;
				RebuildPaintKitList( cfg.paintKit );
			}

			widget->text_field( "Search skins" , "M" , s_skinSearch , sizeof( s_skinSearch ) , SCALE( ImVec2( GetContentRegionAvail().x , 35 ) ) );

			static const auto rarityNames = Strings( {
				"All" , "Consumer" , "Industrial" , "Mil-Spec" , "Restricted" , "Classified" , "Covert" , "Contraband" ,
			} );
			Combo( "Rarity" , &s_rarityFilter , rarityNames );

			if ( s_paintKitLabels.empty() )
			{
				draw->render_text(
					GetWindowDrawList() ,
					set->c_font.inter_medium[0] ,
					GetWindowPos() + SCALE( 0 , GetCursorPosY() ) ,
					GetWindowPos() + GetWindowSize() ,
					gui->get_clr( clr->c_text.text ) ,
					"No matching paint kits." ,
					nullptr ,
					nullptr ,
					ImVec2( 0.f , 0.f ) );
			}
			else
			{
				if ( Combo( "Paint Kit" , &s_paintKitPick , s_paintKitLabels ) )
					cfg.paintKit = s_paintKitIds[static_cast<size_t>( s_paintKitPick )];
			}

			SliderInt( "Paint Kit ID" , &cfg.paintKit , 0 , 120000 , 1 , "%d" );
		}

		auto DrawStatusLine( const char* text , bool ok ) noexcept -> void
		{
			using namespace SyntheticUI;

			draw->render_text(
				GetWindowDrawList() ,
				set->c_font.inter_medium[0] ,
				GetWindowPos() + ImVec2( 0.f , GetCursorPosY() ) ,
				GetWindowPos() + GetWindowSize() ,
				gui->get_clr( ok ? clr->c_other_clr.accent_clr : clr->c_text.text ) ,
				text ,
				nullptr ,
				nullptr ,
				ImVec2( 0.f , 0.f ) );
			gui->spacing();
		}

		auto RenderCustomTextureSection() noexcept -> void
		{
			using namespace SyntheticUI;

			Checkbox( "Enable Custom Texture" , &CustomTexture::enabled );
			widget->text_field( "Image Path" , "M" , CustomTexture::imagePath , sizeof( CustomTexture::imagePath ) , SCALE( ImVec2( GetContentRegionAvail().x , 35 ) ) );

			if ( widget->button( "Load Image" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
			{
				if ( auto* device = GetCookieGUI()->GetDevice() )
					CustomTexture::LoadCustomImage( device , CustomTexture::imagePath );
			}

			DrawStatusLine( CustomTexture::imageLoaded ? "Image loaded" : "No image loaded" , CustomTexture::imageLoaded );

			SliderInt( "Filter Width (0=any)" , &CustomTexture::filterWidth , 0 , 4096 , 256 , "%d" );
			SliderInt( "Filter Height (0=any)" , &CustomTexture::filterHeight , 0 , 4096 , 256 , "%d" );
			if ( CustomTexture::filterWidth < 0 )
				CustomTexture::filterWidth = 0;
			if ( CustomTexture::filterHeight < 0 )
				CustomTexture::filterHeight = 0;

			Separator();

			if ( !CustomTexture::browseMode )
			{
				if ( widget->button( "Start Browse" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
				{
					CustomTexture::ResetBrowse();
					CustomTexture::browseMode = true;
				}
			}
			else
			{
				char browseBuf[96];
				snprintf(
					browseBuf ,
					sizeof( browseBuf ) ,
					"Browse: %d / %d candidates" ,
					CustomTexture::browseIndex + 1 ,
					CustomTexture::candidateCount );
				DrawStatusLine( browseBuf , true );

				const float halfW = ( GetContentRegionAvail().x - SCALE( 4 ) ) * 0.5f;
				if ( widget->button( "Prev (F10)" , ImVec2( halfW , SCALE( 35 ) ) ) )
					CustomTexture::BrowsePrev();
				gui->sameline();
				if ( widget->button( "Next (F11)" , ImVec2( halfW , SCALE( 35 ) ) ) )
					CustomTexture::BrowseNext();

				if ( widget->button( "Lock (F12)" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
					CustomTexture::LockCurrent();

				if ( widget->button( "Stop Browse" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
					CustomTexture::browseMode = false;
			}

			if ( CustomTexture::lockedSRV )
			{
				DrawStatusLine( "Texture LOCKED" , true );
				if ( widget->button( "Unlock" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
					CustomTexture::Unlock();
			}

			if ( widget->button( "Reset Browse" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
				CustomTexture::ResetBrowse();

			Separator();
			Checkbox( "No Filter (all 2D)" , &CustomTexture::noFilter );
			SliderInt( "Min Texture Size" , &CustomTexture::minSize , 1 , 512 , 32 , "%d" );
			if ( CustomTexture::minSize < 1 )
				CustomTexture::minSize = 1;

			char debugBuf[128];
			snprintf(
				debugBuf ,
				sizeof( debugBuf ) ,
				"Hook: %s | calls %d | SRVs %d" ,
				CustomTexture::hooked ? "on" : "off" ,
				CustomTexture::hookCalls ,
				CustomTexture::totalUniqueSRVs );
			DrawStatusLine( debugBuf , CustomTexture::hooked );
		}

		auto RandomizeAllSkins() noexcept -> void
		{
			if ( g_PaintKits.empty() )
				return;

			std::lock_guard<std::mutex> lock( SkinChanger::configMutex );
			for ( int i = 0; i < SyntheticSkinWeapons::kCount; ++i )
			{
				const int defIdx = SyntheticSkinWeapons::kList[i].defIndex;
				SkinChanger::SkinConfig& cfg = SkinChanger::weaponSkins[defIdx];
				cfg.enabled = true;
				const int randomIdx = rand() % static_cast<int>( g_PaintKits.size() );
				cfg.paintKit = g_PaintKits[static_cast<size_t>( randomIdx )].id;
				cfg.wear = 0.001f;
				cfg.seed = 0;
				cfg.statTrak = -1;
			}
			SkinChanger::forceUpdate.store( true );
		}
	}

	auto RenderSkinsTab() noexcept -> void
	{
		using namespace SyntheticUI;

		gui->begin_group();
		{
			gui->begin_child( "Gloves & Knife" );
			{
				Checkbox( "Enable Gloves" , &Gloves::config.enabled );
				if ( Gloves::config.enabled )
				{
					static std::vector<std::string> gloveNames;
					if ( gloveNames.empty() )
					{
						for ( const auto& g : g_GlovesNames )
							gloveNames.emplace_back( g.m_pszName ? g.m_pszName : "?" );
					}
					if ( !gloveNames.empty() )
						Combo( "Glove Model" , &Gloves::config.modelIndex , gloveNames );

					SliderInt( "Glove PaintKit" , &Gloves::config.paintKit , 0 , 100000 , 1 , "%d" );
					SliderFloat( "Glove Wear" , &Gloves::config.wear , 0.f , 1.f , 0.001f , "%.4f" );
					SliderInt( "Glove Seed" , &Gloves::config.seed , 0 , 1000 , 1 , "%d" );
					static const auto teams = Strings( { "Terrorist (2)" , "CT (3)" } );
					int teamIdx = Gloves::config.team == 2 ? 0 : 1;
					if ( Combo( "Loadout Team" , &teamIdx , teams ) )
						Gloves::config.team = teamIdx == 0 ? 2 : 3;
					if ( widget->button( "Apply Gloves" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
						Gloves::ApplyFromConfig();
				}

				Separator();
				Checkbox( "Knife Changer" , &SkinChanger::knifeChangerEnabled );
				if ( SkinChanger::knifeChangerEnabled )
				{
					static std::vector<std::string> knifeNames;
					if ( knifeNames.empty() )
					{
						for ( int i = 0; i < SkinChanger::knifeModelCount; ++i )
							knifeNames.emplace_back( SkinChanger::knifeModels[i].name );
					}
					if ( Combo( "Knife Model" , &SkinChanger::selectedKnifeModel , knifeNames ) )
					{
						SkinChanger::lastAppliedWeapon = 0;
						SkinChanger::forceUpdate.store( true );
					}
				}

				Separator();
				if ( widget->button( "Force Update" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
					SkinChanger::forceUpdate.store( true );
				if ( widget->button( "Randomize All" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
					RandomizeAllSkins();
				if ( widget->button( "Clear All Skins" , ImVec2( GetContentRegionAvail().x , SCALE( 35 ) ) ) )
				{
					std::lock_guard<std::mutex> lock( SkinChanger::configMutex );
					SkinChanger::weaponSkins.clear();
					SkinChanger::forceUpdate.store( true );
				}
			}
			gui->end_child();
		}
		gui->end_group();

		gui->sameline();

		gui->begin_group();
		{
			gui->begin_child( "Weapon Skin" );
			{
				static std::vector<std::string> weaponNames;
				if ( weaponNames.empty() )
				{
					for ( int i = 0; i < SyntheticSkinWeapons::kCount; ++i )
						weaponNames.emplace_back( SyntheticSkinWeapons::kList[i].name );
				}
				Combo( "Weapon" , &s_selectedWeapon , weaponNames );

				if ( s_selectedWeapon >= 0 && s_selectedWeapon < SyntheticSkinWeapons::kCount )
				{
					const int defIdx = SyntheticSkinWeapons::kList[s_selectedWeapon].defIndex;
					SkinChanger::SkinConfig& cfg = SkinChanger::weaponSkins[defIdx];

					Checkbox( "Enabled" , &cfg.enabled );
					PaintKitPicker( cfg );
					SliderFloat( "Wear" , &cfg.wear , 0.f , 1.f , 0.001f , "%.4f" );
					SliderInt( "Seed" , &cfg.seed , 0 , 1000 , 1 , "%d" );

					bool useStatTrak = cfg.statTrak >= 0;
					if ( Checkbox( "StatTrak" , &useStatTrak ) )
						cfg.statTrak = useStatTrak ? 0 : -1;
					if ( useStatTrak )
						SliderInt( "StatTrak Value" , &cfg.statTrak , 0 , 999999 , 1 , "%d" );
				}
			}
			gui->end_child();

			gui->begin_child( "Custom Texture" );
			{
				RenderCustomTextureSection();
			}
			gui->end_child();
		}
		gui->end_group();
	}
}

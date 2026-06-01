#include "WeaponIcons.hpp"

#include "../Resources/embedded/game_icons.h"
#include "../Resources/embedded/iconscs2_embedded.hpp"

#include <DllLauncher.hpp>

#include <ImGui/imgui.h>

#include <filesystem>
#include <string>

#include <GameClient/CL_ItemDefinition.hpp>

namespace WeaponIcons
{
	namespace
	{
		ImFont* s_gunIconsFont = nullptr;
		ImFont* s_knifeIconsFont = nullptr;
		ImFont* s_obsIconsFont = nullptr;
		bool s_initAttempted = false;

		static const ImWchar kIconRanges[] = {
			0x0020 , 0x00FF ,
			0 ,
		};

		auto ResolveFontPath( const char* const* candidates , size_t count ) -> std::string
		{
			const std::string base = GetDllDir();
			for ( size_t i = 0; i < count; ++i )
			{
				const std::string path = base + candidates[i];
				if ( std::filesystem::exists( path ) )
					return path;
			}

			return {};
		}

		auto LoadFontFromFile( const std::string& path , float size ) -> ImFont*
		{
			if ( path.empty() )
				return nullptr;

			ImFontConfig config{};
			config.OversampleH = 2;
			config.OversampleV = 2;

			return ImGui::GetIO().Fonts->AddFontFromFileTTF( path.c_str() , size , &config , kIconRanges );
		}

		auto LoadFontFromMemory( const void* data , int dataSize , float size ) -> ImFont*
		{
			if ( data == nullptr || dataSize <= 0 )
				return nullptr;

			ImFontConfig config{};
			config.OversampleH = 2;
			config.OversampleV = 2;

			return ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
				const_cast<void*>( data ) ,
				dataSize ,
				size ,
				&config ,
				kIconRanges );
		}

		auto LoadGunIconsEmbedded( float size ) -> ImFont*
		{
			ImFontConfig config{};
			config.OversampleH = 2;
			config.OversampleV = 2;

			return ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
				game_icons_compressed_data ,
				static_cast<int>( game_icons_compressed_size ) ,
				size ,
				&config ,
				kIconRanges );
		}

		auto LoadKnifeIconsEmbedded( float size ) -> ImFont*
		{
			return LoadFontFromMemory( kIconScs2EmbeddedData , static_cast<int>( kIconScs2EmbeddedSize ) , size );
		}
	}

	auto Invalidate() noexcept -> void
	{
		s_gunIconsFont = nullptr;
		s_knifeIconsFont = nullptr;
		s_obsIconsFont = nullptr;
		s_initAttempted = false;
	}

	auto Init() noexcept -> bool
	{
		if ( FontReady( s_gunIconsFont ) || FontReady( s_knifeIconsFont ) || FontReady( s_obsIconsFont ) )
			return true;

		if ( s_initAttempted )
			return s_gunIconsFont != nullptr || s_knifeIconsFont != nullptr || s_obsIconsFont != nullptr;

		s_initAttempted = true;

		const char* gunCandidates[] = {
			"Resources/CS2GunIcons.ttf" ,
			"Client/Resources/CS2GunIcons.ttf" ,
			"../Resources/CS2GunIcons.ttf" ,
		};

		const char* knifeCandidates[] = {
			"Resources/iconscs2.ttf" ,
			"Client/Resources/iconscs2.ttf" ,
			"../Resources/iconscs2.ttf" ,
		};

		const char* obsCandidates[] = {
			"Resources/obs_icons.ttf" ,
			"Client/Resources/obs_icons.ttf" ,
			"../Resources/obs_icons.ttf" ,
		};

		constexpr float kFontSize = 16.f;

		s_gunIconsFont = LoadFontFromFile( ResolveFontPath( gunCandidates , 3 ) , kFontSize );
		if ( !s_gunIconsFont )
			s_gunIconsFont = LoadGunIconsEmbedded( kFontSize );

		s_knifeIconsFont = LoadFontFromFile( ResolveFontPath( knifeCandidates , 3 ) , kFontSize );
		if ( !s_knifeIconsFont )
			s_knifeIconsFont = LoadKnifeIconsEmbedded( kFontSize );

		s_obsIconsFont = LoadFontFromFile( ResolveFontPath( obsCandidates , 3 ) , kFontSize );

		return s_gunIconsFont != nullptr || s_knifeIconsFont != nullptr || s_obsIconsFont != nullptr;
	}

	auto GetFont() noexcept -> ImFont*
	{
		return s_gunIconsFont ? s_gunIconsFont : s_knifeIconsFont;
	}

	auto GetKnifeFont() noexcept -> ImFont*
	{
		return s_knifeIconsFont ? s_knifeIconsFont : s_obsIconsFont;
	}

	auto GetObsFont() noexcept -> ImFont*
	{
		return s_obsIconsFont;
	}

	auto GetWeaponIconChar( int defIndex ) -> wchar_t
	{
		switch ( static_cast<ItemDefinitionIndex>( defIndex ) )
		{
		case WEAPON_P90: return L'P';
		case WEAPON_MP9: return L'O';
		case WEAPON_MP5_SD: return L'O';
		case WEAPON_M4A1: return L'S';
		case WEAPON_M4A1_S: return L'T';
		case WEAPON_DESERT_EAGLE: return L'A';
		case WEAPON_DUAL_BERETTAS: return L'B';
		case WEAPON_FIVE_SEVEN: return L'C';
		case WEAPON_GLOCK_18: return L'D';
		case WEAPON_R8_REVOLVER: return L'J';
		case WEAPON_P2000: return L'E';
		case WEAPON_P250: return L'F';
		case WEAPON_USP_S: return L'G';
		case WEAPON_TEC_9: return L'H';
		case WEAPON_CZ75_AUTO: return L'I';
		case WEAPON_MAC_10: return L'K';
		case WEAPON_UMP_45: return L'L';
		case WEAPON_PP_BIZON: return L'M';
		case WEAPON_MP7: return L'N';
		case WEAPON_GALIL_AR: return L'Q';
		case WEAPON_FAMAS: return L'R';
		case WEAPON_AUG: return L'U';
		case WEAPON_SG_553: return L'V';
		case WEAPON_AK_47: return L'W';
		case WEAPON_G3SG1: return L'X';
		case WEAPON_SCAR_20: return L'Y';
		case WEAPON_AWP: return L'Z';
		case WEAPON_SSG_08: return L'a';
		case WEAPON_XM1014: return L'b';
		case WEAPON_SAWED_OFF: return L'c';
		case WEAPON_MAG_7: return L'd';
		case WEAPON_NOVA: return L'e';
		case WEAPON_NEGEV: return L'f';
		case WEAPON_M249: return L'g';
		case WEAPON_ZEUS_X27: return L'h';
		case WEAPON_FLASHBANG: return L'i';
		case WEAPON_HIGH_EXPLOSIVE_GRENADE: return L'j';
		case WEAPON_SMOKE_GRENADE: return L'k';
		case WEAPON_MOLOTOV: return L'l';
		case WEAPON_DECOY_GRENADE: return L'm';
		case WEAPON_INCENDIARY_GRENADE: return L'n';
		case WEAPON_C4_EXPLOSIVE: return L'o';
		case WEAPON_KNIFE_BAYONET: return L'1';
		case WEAPON_KNIFE_CSS: return L'2';
		case WEAPON_KNIFE_FLIP: return L'3';
		case WEAPON_KNIFE_GUT: return L'4';
		case WEAPON_KNIFE_KARAMBIT: return L'5';
		case WEAPON_KNIFE_M9_BAYONET: return L'6';
		case WEAPON_KNIFE_TACTICAL: return L'7';
		case WEAPON_KNIFE_FALCHION: return L'8';
		case WEAPON_KNIFE_SURVIVAL_BOWIE: return L'9';
		case WEAPON_KNIFE_BUTTERFLY: return L'0';
		case WEAPON_KNIFE_PUSH: return L'q';
		case WEAPON_KNIFE_CORD: return L'w';
		case WEAPON_KNIFE_CANIS: return L'e';
		case WEAPON_KNIFE_URSUS: return L'r';
		case WEAPON_KNIFE_GYPSY_JACKKNIFE: return L't';
		case WEAPON_KNIFE_OUTDOOR: return L'y';
		case WEAPON_KNIFE_STILETTO: return L'u';
		case WEAPON_KNIFE_WIDOWMAKER: return L'i';
		case WEAPON_KNIFE_SKELETON: return L'o';
		case WEAPON_KNIFE_KUKRI: return L'p';
		case WEAPON_KNIFE0:
		case WEAPON_KNIFE1:
		case WEAPON_KNIFE2:
		case WEAPON_KNIFE3:
			return L']';
		default:
			return L'?';
		}
	}
}

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <ImGui/imgui.h>

#include <Client/Features/Visuals/Esp.hpp>

class CCSPlayerController;
class C_CSPlayerPawn;

namespace EspOverlay
{
	enum class AlignSide : std::uint8_t
	{
		Left = 0 ,
		Top ,
		Right ,
		Bottom ,
		Max ,
	};

	enum class AlignDirection : std::uint8_t
	{
		Left = 0 ,
		Top ,
		Right ,
		Bottom ,
	};

	struct BoxComponent
	{
		ImU32 color = IM_COL32( 255 , 255 , 255 , 255 );
		ImU32 outlineColor = IM_COL32( 0 , 0 , 0 , 255 );
		float thickness = 1.f;
		bool  filled = false;
		ImU32 fillColor = IM_COL32( 0 , 0 , 0 , 40 );
	};

	struct Config
	{
		bool enabled = true;
		bool useComponents = true;
		bool showBox = true;
		bool showHealthBar = true;
		bool showName = true;
		bool showDistance = true;
		bool showWeaponIcon = true;
		bool showAmmoBar = false;
		bool showHelmetFlag = false;
		bool showKitFlag = false;
		float boxColor[4] = { 1.f , 1.f , 1.f , 1.f };
		float healthBarColor[4] = { 0.f , 1.f , 0.f , 1.f };
		float ammoBarColor[4] = { 0.31f , 0.63f , 1.f , 1.f };
		float textColor[4] = { 1.f , 1.f , 1.f , 1.f };
		float boxThickness = 1.f;
		float barThickness = 2.f;
	};

	inline Config config;

	struct BarComponent
	{
		AlignSide side = AlignSide::Left;
		float progress = 1.f;
		ImU32 color = IM_COL32( 0 , 255 , 0 , 255 );
		float thickness = 2.f;
		ImVec2 size{};
	};

	struct TextComponent
	{
		AlignSide      side = AlignSide::Top;
		AlignDirection direction = AlignDirection::Top;
		std::string    text;
		ImU32          color = IM_COL32( 255 , 255 , 255 , 255 );
		ImVec2         size{};
	};

	class OverlayContext
	{
	public:
		void AddBar( const BarComponent& bar );
		void AddText( const TextComponent& text );
		void AddBox( const BoxComponent& box );
		void Clear();
		void Render( ImDrawList* draw , const ImVec4& box ) const;

	private:
		std::vector<BarComponent>  bars_;
		std::vector<TextComponent> texts_;
		BoxComponent               box_{};
		bool                       hasBox_ = false;
		float sidePadding_[static_cast<std::size_t>( AlignSide::Max )]{};
	};

	auto RenderPlayer(
		uintptr_t controller ,
		uintptr_t pawn ,
		const ImVec4& box ,
		int health ,
		const char* name ,
		float distanceMeters ,
		int weaponDefIndex = 0 ) -> void;

	auto RenderAll() -> void;

} // namespace EspOverlay

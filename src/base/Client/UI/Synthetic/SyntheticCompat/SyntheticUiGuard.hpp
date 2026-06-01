#pragma once



#include <framework/settings/functions.h>

#include <ImGui/imgui_internal.h>



namespace SyntheticUi

{

	// Pointer equality only — safe when font may be a stale address after atlas->Clear().

	inline auto FontBelongsToAtlas( ImFont* font ) noexcept -> bool

	{

		if ( !font )

			return false;



		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;

		if ( !atlas )

			return false;



		for ( ImFont* const atlasFont : atlas->Fonts )

		{

			if ( atlasFont == font )

				return true;

		}



		return false;

	}



	inline auto ResolveFontOrFallback( ImFont* font ) noexcept -> ImFont*

	{

		if ( FontBelongsToAtlas( font ) )

			return font;



		if ( ImFont* defaultFont = ImGui::GetDefaultFont() )

		{

			if ( FontBelongsToAtlas( defaultFont ) )

				return defaultFont;

		}



		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;

		if ( atlas && atlas->Fonts.Size > 0 )

			return atlas->Fonts[0];



		return nullptr;

	}



	inline auto FontUsable( ImFont* font ) noexcept -> bool

	{

		if ( !FontBelongsToAtlas( font ) )

			return false;



		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;

		return atlas && atlas->IsBuilt() && atlas->TexID != ImTextureID{} && font->IsLoaded();

	}



	inline auto AtlasReady() noexcept -> bool

	{

		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;

		return atlas && atlas->IsBuilt() && atlas->TexID != ImTextureID{};

	}



	inline auto MenuFont( int index = 0 ) noexcept -> ImFont*

	{

		if ( index < 0 || index > 1 )

			return nullptr;



		if ( FontUsable( set->c_font.inter_medium[index] ) )

			return set->c_font.inter_medium[index];



		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;

		if ( atlas && atlas->Fonts.Size > 0 && FontUsable( atlas->Fonts[0] ) )

			return atlas->Fonts[0];



		return nullptr;

	}



	inline auto MenuFontsReady() noexcept -> bool

	{

		return MenuFont( 0 ) != nullptr && MenuFont( 1 ) != nullptr;

	}



	// After Fonts->Clear()/Build(), stale pointers can remain on FontStack and ctx->Font.

	inline auto ResetContextFonts() noexcept -> void

	{

		ImGuiContext* const ctx = ImGui::GetCurrentContext();

		if ( !ctx )

			return;



		ImGuiIO& io = ImGui::GetIO();

		ctx->FontStack.clear();



		ImFont* font = MenuFont( 0 );

		if ( !font && io.Fonts && io.Fonts->Fonts.Size > 0 )

		{

			for ( ImFont* const atlasFont : io.Fonts->Fonts )

			{

				if ( FontUsable( atlasFont ) )

				{

					font = atlasFont;

					break;

				}

			}

		}



		if ( font )

		{

			io.FontDefault = font;

			ImGui::SetCurrentFont( font );

		}

		else

		{

			io.FontDefault = nullptr;

			ctx->Font = nullptr;

			ctx->FontSize = 0.f;

			ctx->FontBaseSize = 0.f;

		}

	}



	inline auto PushFont( ImFont* font ) noexcept -> bool

	{

		font = ResolveFontOrFallback( font );

		if ( !FontUsable( font ) )

			return false;



		return gui->push_font( font );

	}



	inline auto PopFont() noexcept -> void

	{

		ImGuiContext* const ctx = ImGui::GetCurrentContext();

		if ( !ctx || ctx->FontStack.Size == 0 )

			return;



		gui->pop_font();

	}



	inline auto WindowDrawList() noexcept -> ImDrawList*

	{

		return ImGui::GetWindowDrawList();

	}



	inline auto WidgetDrawList( ImGuiWindow* window ) noexcept -> ImDrawList*

	{

		return window ? window->DrawList : nullptr;

	}



	inline auto FontAtlasTexture( ImFont* font ) noexcept -> ImTextureID

	{

		font = ResolveFontOrFallback( font );

		if ( FontUsable( font ) )

		{

			ImFontAtlas* const owner = ImGui::GetIO().Fonts;

			if ( owner && owner->TexID != ImTextureID{} )

				return owner->TexID;

		}



		ImFontAtlas* const atlas = ImGui::GetIO().Fonts;

		if ( atlas && atlas->IsBuilt() )

			return atlas->TexID;



		return ImTextureID{};

	}

}


#include "MenuAssets.hpp"

#include <Client/Features/Visuals/Esp.hpp>
#include <Client/Features/Visuals/EspOverlay.hpp>
#include <Client/Resources/embedded/Bgs.h>
#include <Client/Resources/embedded/esp_preview.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Common/Include/stb/stb_image.h>

namespace MenuAssets
{
	namespace
	{
		ID3D11ShaderResourceView* s_menuBackgroundSrv = nullptr;
		ID3D11Texture2D* s_menuBackgroundTex = nullptr;
		ID3D11ShaderResourceView* s_espPreviewSrv = nullptr;
		ID3D11Texture2D* s_espPreviewTex = nullptr;

		auto CreateSrvFromMemory( ID3D11Device* device , const unsigned char* data , int dataSize , ID3D11Texture2D** outTex , ID3D11ShaderResourceView** outSrv ) noexcept -> bool
		{
			if ( !device || !data || dataSize <= 0 || !outTex || !outSrv )
				return false;

			int width = 0;
			int height = 0;
			int channels = 0;
			unsigned char* pixels = stbi_load_from_memory( data , dataSize , &width , &height , &channels , 4 );
			if ( !pixels || width <= 0 || height <= 0 )
				return false;

			D3D11_TEXTURE2D_DESC desc{};
			desc.Width = static_cast<UINT>( width );
			desc.Height = static_cast<UINT>( height );
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			D3D11_SUBRESOURCE_DATA subresource{};
			subresource.pSysMem = pixels;
			subresource.SysMemPitch = static_cast<UINT>( width * 4 );

			ID3D11Texture2D* texture = nullptr;
			const HRESULT hrCreate = device->CreateTexture2D( &desc , &subresource , &texture );
			stbi_image_free( pixels );
			if ( FAILED( hrCreate ) || !texture )
				return false;

			ID3D11ShaderResourceView* srv = nullptr;
			const HRESULT hrSrv = device->CreateShaderResourceView( texture , nullptr , &srv );
			if ( FAILED( hrSrv ) || !srv )
			{
				texture->Release();
				return false;
			}

			*outTex = texture;
			*outSrv = srv;
			return true;
		}

		auto ReleaseTexture( ID3D11Texture2D*& tex , ID3D11ShaderResourceView*& srv ) noexcept -> void
		{
			if ( srv )
			{
				srv->Release();
				srv = nullptr;
			}
			if ( tex )
			{
				tex->Release();
				tex = nullptr;
			}
		}
	}

	auto Init( ID3D11Device* device ) noexcept -> bool
	{
		if ( !device )
			return false;

		Shutdown();

		const bool bgOk = CreateSrvFromMemory(
			device ,
			main_texture ,
			static_cast<int>( sizeof( main_texture ) ) ,
			&s_menuBackgroundTex ,
			&s_menuBackgroundSrv );

		const bool previewOk = CreateSrvFromMemory(
			device ,
			esp_preview1 ,
			static_cast<int>( sizeof( esp_preview1 ) ) ,
			&s_espPreviewTex ,
			&s_espPreviewSrv );

		return bgOk && previewOk;
	}

	auto Shutdown() noexcept -> void
	{
		ReleaseTexture( s_menuBackgroundTex , s_menuBackgroundSrv );
		ReleaseTexture( s_espPreviewTex , s_espPreviewSrv );
	}

	auto GetMenuBackgroundTexture() noexcept -> ImTextureID
	{
		return reinterpret_cast<ImTextureID>( s_menuBackgroundSrv );
	}

	auto GetEspPreviewTexture() noexcept -> ImTextureID
	{
		return reinterpret_cast<ImTextureID>( s_espPreviewSrv );
	}

	auto RenderMenuBackgroundImage( ImDrawList* draw , ImVec2 origin , ImVec2 size , float alpha ) noexcept -> void
	{
		if ( !draw || !s_menuBackgroundSrv )
			return;

		const ImU32 tint = IM_COL32( 255 , 255 , 255 , static_cast<int>( 255.f * alpha ) );
		draw->AddImage(
			GetMenuBackgroundTexture() ,
			origin ,
			ImVec2( origin.x + size.x , origin.y + size.y ) ,
			ImVec2( 0.f , 0.f ) ,
			ImVec2( 1.f , 1.f ) ,
			tint );
	}

	auto RenderEspPreviewPanel( ImDrawList* draw , ImVec2 origin , ImVec2 size ) noexcept -> void
	{
		if ( !draw || !s_espPreviewSrv )
			return;

		const ImVec2 imageMin( origin.x + 8.f , origin.y + 8.f );
		const ImVec2 imageMax( origin.x + size.x - 8.f , origin.y + size.y - 8.f );
		draw->AddImage(
			GetEspPreviewTexture() ,
			imageMin ,
			imageMax ,
			ImVec2( 0.f , 0.f ) ,
			ImVec2( 1.f , 1.f ) ,
			IM_COL32( 255 , 255 , 255 , 255 ) );

		const ImVec4 previewBox(
			imageMin.x + size.x * 0.22f ,
			imageMin.y + size.y * 0.08f ,
			imageMax.x - size.x * 0.22f ,
			imageMax.y - size.y * 0.12f );

		if ( EspOverlay::config.showBox || ESP::config.bBox )
		{
			const ImU32 boxColor = IM_COL32(
				static_cast<int>( ESP::config.boxColor[0] * 255.f ) ,
				static_cast<int>( ESP::config.boxColor[1] * 255.f ) ,
				static_cast<int>( ESP::config.boxColor[2] * 255.f ) ,
				static_cast<int>( ESP::config.boxColor[3] * 255.f ) );
			draw->AddRect(
				ImVec2( previewBox.x , previewBox.y ) ,
				ImVec2( previewBox.z , previewBox.w ) ,
				boxColor ,
				0.f ,
				0 ,
				2.f );
		}

		if ( EspOverlay::config.showHealthBar || ESP::config.bHealthBar )
		{
			const ImVec2 barMin( previewBox.x - 6.f , previewBox.y );
			const ImVec2 barMax( previewBox.x - 2.f , previewBox.w );
			draw->AddRectFilled( barMin , barMax , IM_COL32( 20 , 20 , 20 , 200 ) );
			const float fill = 0.72f;
			const float barHeight = ( barMax.y - barMin.y ) * fill;
			draw->AddRectFilled(
				ImVec2( barMin.x , barMax.y - barHeight ) ,
				barMax ,
				IM_COL32( 80 , 220 , 80 , 255 ) );
		}

		if ( EspOverlay::config.showName || ESP::config.bName )
		{
			const char* previewName = "Player";
			const ImVec2 textSize = ImGui::CalcTextSize( previewName );
			draw->AddText(
				ImVec2( ( previewBox.x + previewBox.z ) * 0.5f - textSize.x * 0.5f , previewBox.y - textSize.y - 4.f ) ,
				IM_COL32( 255 , 255 , 255 , 255 ) ,
				previewName );
		}

		if ( EspOverlay::config.showDistance || ESP::config.bDistance )
		{
			const char* distText = "[ 24m ]";
			const ImVec2 textSize = ImGui::CalcTextSize( distText );
			draw->AddText(
				ImVec2( ( previewBox.x + previewBox.z ) * 0.5f - textSize.x * 0.5f , previewBox.w + 4.f ) ,
				IM_COL32( 200 , 200 , 200 , 255 ) ,
				distText );
		}

		if ( EspOverlay::config.showHelmetFlag )
		{
			draw->AddText(
				ImVec2( previewBox.z + 4.f , previewBox.y ) ,
				IM_COL32( 120 , 200 , 255 , 255 ) ,
				"HK" );
		}

		if ( EspOverlay::config.showKitFlag )
		{
			draw->AddText(
				ImVec2( previewBox.z + 4.f , previewBox.y + 14.f ) ,
				IM_COL32( 100 , 255 , 140 , 255 ) ,
				"KIT" );
		}
	}
}

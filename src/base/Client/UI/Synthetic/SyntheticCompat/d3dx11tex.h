#pragma once

#include <vector>

#include <d3d11.h>
#include <wincodec.h>
#include <wrl/client.h>

#pragma comment( lib, "windowscodecs.lib" )

struct D3DX11_IMAGE_LOAD_INFO
{
	UINT Width = 0;
	UINT Height = 0;
	UINT Depth = 0;
	UINT FirstMipLevel = 0;
	UINT MipLevels = 1;
	UINT Usage = D3D11_USAGE_DEFAULT;
	UINT BindFlags = D3D11_BIND_SHADER_RESOURCE;
	UINT CpuAccessFlags = 0;
	UINT MiscFlags = 0;
	DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
	UINT Filter = 0;
	UINT MipFilter = 0;
};

struct ID3DX11ThreadPump {};

inline HRESULT D3DX11CreateShaderResourceViewFromMemory(
	ID3D11Device* device ,
	const void* data ,
	size_t dataSize ,
	D3DX11_IMAGE_LOAD_INFO* loadInfo ,
	ID3DX11ThreadPump* ,
	ID3D11ShaderResourceView** outSrv ,
	HRESULT* )
{
	if ( !device || !data || !dataSize || !outSrv )
		return E_INVALIDARG;

	*outSrv = nullptr;

	static bool comInit = false;
	if ( !comInit )
	{
		CoInitializeEx( nullptr , COINIT_MULTITHREADED );
		comInit = true;
	}

	Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
	if ( FAILED( CoCreateInstance( CLSID_WICImagingFactory , nullptr , CLSCTX_INPROC_SERVER , IID_PPV_ARGS( &factory ) ) ) )
		return E_FAIL;

	Microsoft::WRL::ComPtr<IWICStream> stream;
	if ( FAILED( factory->CreateStream( &stream ) ) )
		return E_FAIL;

	if ( FAILED( stream->InitializeFromMemory( reinterpret_cast<BYTE*>( const_cast<void*>( data ) ) , static_cast<DWORD>( dataSize ) ) ) )
		return E_FAIL;

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	if ( FAILED( factory->CreateDecoderFromStream( stream.Get() , nullptr , WICDecodeMetadataCacheOnDemand , &decoder ) ) )
		return E_FAIL;

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	if ( FAILED( decoder->GetFrame( 0 , &frame ) ) )
		return E_FAIL;

	UINT width = 0;
	UINT height = 0;
	frame->GetSize( &width , &height );
	if ( width == 0 || height == 0 )
		return E_FAIL;

	Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
	if ( FAILED( factory->CreateFormatConverter( &converter ) ) )
		return E_FAIL;

	if ( FAILED( converter->Initialize( frame.Get() , GUID_WICPixelFormat32bppRGBA , WICBitmapDitherTypeNone , nullptr , 0.f , WICBitmapPaletteTypeCustom ) ) )
		return E_FAIL;

	const UINT stride = width * 4;
	const UINT imageSize = stride * height;
	std::vector<BYTE> pixels( imageSize );
	if ( FAILED( converter->CopyPixels( nullptr , stride , imageSize , pixels.data() ) ) )
		return E_FAIL;

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA sub{};
	sub.pSysMem = pixels.data();
	sub.SysMemPitch = stride;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	if ( FAILED( device->CreateTexture2D( &desc , &sub , &texture ) ) )
		return E_FAIL;

	return device->CreateShaderResourceView( texture.Get() , nullptr , outSrv );
}

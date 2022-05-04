#include "Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include "D3D11Internal.hpp"


//-----------------------------------------------------------------------------------------------
Texture::Texture()
{

}


//-----------------------------------------------------------------------------------------------
Texture::~Texture()
{
	ReleaseResources();
}


//-----------------------------------------------------------------------------------------------
bool Texture::LoadFromFile( Renderer* source, char const* imageFilePath )
{
	Image newImage = Image( imageFilePath );
	bool  result   = CreateFromImage( source, imageFilePath, newImage );

	return result;
}


//-----------------------------------------------------------------------------------------------
bool Texture::CreateFromImage( Renderer* source, char const* imageFilePath, Image const& image )
{
	ID3D11Texture2D* textureHandle = nullptr;
	
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width              = image.GetWidth();
	textureDesc.Height             = image.GetHeight();
	textureDesc.MipLevels          = 1;
	textureDesc.ArraySize          = 1;
	textureDesc.Usage              = D3D11_USAGE_IMMUTABLE;
	textureDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags     = 0;
	textureDesc.MiscFlags          = 0;
	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem          = image.GetRawData();
	initialData.SysMemPitch      = image.GetBytePerPixel() * image.GetPitch();
	initialData.SysMemSlicePitch = 0;

	HRESULT hResult = source->GetDevice()->CreateTexture2D( &textureDesc, &initialData, &textureHandle );

	if (!SUCCEEDED( hResult ))
		return false;


#if defined(ENGINE_DEBUG_RENDERER)

	source->SetResourceDebugName( textureHandle, imageFilePath );

#endif

	m_handle = textureHandle;

	textureHandle = nullptr;

	m_imageFilePath = imageFilePath;
	m_dimensions = IntVec2( image.GetWidth(), image.GetHeight() );

	return true;
}


//-----------------------------------------------------------------------------------------------
ID3D11RenderTargetView* Texture::GetOrCreateRenderTargetView( Renderer* renderer )
{
	if (m_renderTargetView != nullptr)
	{
		return m_renderTargetView;
	}

	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT result = renderer->GetSwapChain()->GetBuffer(
		                                       0,
		                                       __uuidof(ID3D11Texture2D),
		                                       reinterpret_cast<void**>(&backBuffer)
	                                     );

	ASSERT_OR_DIE( SUCCEEDED( result ), "Failed to create the backbuffer." );

	WatchInternal( backBuffer );

	HRESULT hResult = renderer->GetDevice()->CreateRenderTargetView(
		m_handle,
		nullptr,
		&m_renderTargetView
	);

	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed to create the Render Target View." );

	DX_SAFE_RELEASE( backBuffer );

	return m_renderTargetView;
}


//------------------------------------------------------------------------------------------------
ID3D11RenderTargetView* Texture::GetOrCreateRenderTargetViewArray( Renderer* renderer, uint arrayNum /*= 1*/ )
{
	if ( m_renderTargetView != nullptr )
	{
		return m_renderTargetView;
	}

	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT result = renderer->GetSwapChain()->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		reinterpret_cast< void** >( &backBuffer )
	);

	ASSERT_OR_DIE( SUCCEEDED( result ), "Failed to create the backbuffer." );

	WatchInternal( backBuffer );

	D3D11_TEX2D_ARRAY_RTV tex2dArrayRTV = {};
	tex2dArrayRTV.ArraySize = arrayNum;

	D3D11_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray = tex2dArrayRTV;

	HRESULT hResult = renderer->GetDevice()->CreateRenderTargetView(
		m_handle,
		&desc,
		&m_renderTargetView
	);

	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed to create the Render Target View." );

	DX_SAFE_RELEASE( backBuffer );

	return m_renderTargetView;
}

//-----------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* Texture::GetOrCreateShaderResourceView( Renderer* renderer )
{
	if (m_shaderResourceView != nullptr)
	{
		return m_shaderResourceView;
	}

	HRESULT hResult = renderer->GetDevice()->CreateShaderResourceView( m_handle, nullptr, &m_shaderResourceView );
	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed To Create Shader Resource View" );


#if defined(ENGINE_DEBUG_RENDERER)

	renderer->SetResourceDebugName( m_shaderResourceView, std::string(m_imageFilePath.c_str()).append(".srv").c_str() );

#endif

	return m_shaderResourceView;

}


//-----------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* Texture::GetOrCreateShaderResourceViewDepthBuffer( Renderer* renderer )
{
	if ( m_shaderResourceView != nullptr )
	{
		return m_shaderResourceView;
	}
	
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2DArray.ArraySize = m_texArraySize;
	shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
	shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
	
	HRESULT hResult = renderer->GetDevice()->CreateShaderResourceView( m_handle, &shaderResourceViewDesc, &m_shaderResourceView );
	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed To Create Shader Resource View" );


#if defined(ENGINE_DEBUG_RENDERER)

	renderer->SetResourceDebugName( m_shaderResourceView, "Depth Buffer View" );

#endif

	return m_shaderResourceView;
}


//------------------------------------------------------------------------------------------------
bool Texture::CreateCubemapfromFile( Renderer* source, std::vector<std::string>& imagePaths )
{
	ID3D11Texture2D* textureHandle = nullptr;

	m_texArraySize = 6;

	D3D11_SUBRESOURCE_DATA initialData[6] = {};
	
	Image newImage[6];
	for ( int imageNum = 0; imageNum < 6; imageNum++ )
	{
		newImage[imageNum] = Image( imagePaths[imageNum].c_str() );
		initialData[imageNum].pSysMem          = newImage[imageNum].GetRawData();
		initialData[imageNum].SysMemPitch      = newImage[imageNum].GetBytePerPixel() * newImage[imageNum].GetPitch();
		initialData[imageNum].SysMemSlicePitch = 0;
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	
	textureDesc.Width              = newImage[0].GetWidth();
	textureDesc.Height             = newImage[0].GetHeight();
	textureDesc.MipLevels          = 1;
	textureDesc.ArraySize          = m_texArraySize;
	textureDesc.Usage              = D3D11_USAGE_IMMUTABLE;
	textureDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags     = 0;
	textureDesc.MiscFlags          = D3D11_RESOURCE_MISC_TEXTURECUBE;
	textureDesc.SampleDesc.Count   = 1;
	textureDesc.SampleDesc.Quality = 0;


	HRESULT hResult = source->GetDevice()->CreateTexture2D( &textureDesc, initialData, &textureHandle );

	if ( !SUCCEEDED( hResult ) )
		return false;


#if defined(ENGINE_DEBUG_RENDERER)

	source->SetResourceDebugName( textureHandle, imagePaths[0].c_str() );

#endif

	m_handle = textureHandle;

	textureHandle = nullptr;

	m_imageFilePath = newImage[0].GetImageFilePath();
	m_dimensions = IntVec2( newImage[0].GetWidth(), newImage[0].GetHeight() );

	return true;
}


//------------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* Texture::GetOrCreateShaderResourceViewTextureCube( Renderer* renderer )
{
	if ( m_shaderResourceView != nullptr )
	{
		return m_shaderResourceView;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	HRESULT hResult = renderer->GetDevice()->CreateShaderResourceView( m_handle, &shaderResourceViewDesc, &m_shaderResourceView );
	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed To Create Shader Resource View" );


#if defined(ENGINE_DEBUG_RENDERER)

	renderer->SetResourceDebugName( m_shaderResourceView, "Depth Buffer View" );

#endif

	return m_shaderResourceView;
}


//-----------------------------------------------------------------------------------------------
bool Texture::CreateDepthStencilTarget( Renderer* source, Image const& image )
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width              = image.GetWidth();
	desc.Height             = image.GetHeight();
	desc.MipLevels          = 1;
	desc.ArraySize          = 1;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;

	HRESULT hResult = source->GetDevice()->CreateTexture2D( &desc, nullptr, &m_handle );

	if (!SUCCEEDED( hResult ))
	{
		ERROR_AND_DIE( "Failed to Create Depth Texture" );
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool Texture::CreateDepthBufferTarget( Renderer* source, IntVec2 const& dimensions, uint arrayDimensions /*= 1*/ )
{
	m_texArraySize = arrayDimensions;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width              = static_cast< UINT >( dimensions.x );
	desc.Height             = static_cast< UINT >( dimensions.y );
	desc.MipLevels          = 1;
	desc.ArraySize          = arrayDimensions;
	desc.Usage              = D3D11_USAGE_DEFAULT;
	desc.Format             = DXGI_FORMAT_R32_TYPELESS;
	desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;

	HRESULT hResult = source->GetDevice()->CreateTexture2D( &desc, nullptr, &m_handle );

	if ( !SUCCEEDED( hResult ) )
	{
		ERROR_AND_DIE( "Failed to Create Depth Texture" );
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
ID3D11DepthStencilView* Texture::GetOrCreateDepthStencilBufferView( Renderer* renderer, int sliceNum /*= 0*/ )
{
	if ( m_depthBufferViews[ sliceNum ] != nullptr )
	{
		return m_depthBufferViews[ sliceNum ];
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc  = {};
	depthStencilViewDesc.Format                         = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension                  = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDesc.Texture2DArray.ArraySize       = 1;
	depthStencilViewDesc.Texture2DArray.FirstArraySlice = sliceNum;
	depthStencilViewDesc.Texture2DArray.MipSlice        = 0;
	
	HRESULT hResult = renderer->GetDevice()->CreateDepthStencilView( m_handle, &depthStencilViewDesc, &m_depthBufferViews[ sliceNum ] );
	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed To Create Depth Stencil View for Depth Buffer" );
	
	return m_depthBufferViews[ sliceNum ];
}


//-----------------------------------------------------------------------------------------------
ID3D11DepthStencilView* Texture::GetOrCreateDepthStencilView( Renderer* renderer )
{
	if (m_depthStencilView != nullptr)
	{
		return m_depthStencilView;
	}

	HRESULT hResult = renderer->GetDevice()->CreateDepthStencilView( m_handle, nullptr, &m_depthStencilView );
	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed To Create Depth Stencil View" );

	return m_depthStencilView;
}


//-----------------------------------------------------------------------------------------------
void Texture::ReleaseResources()
{
	for ( ID3D11DepthStencilView*& view : m_depthBufferViews )
	{
		DX_SAFE_RELEASE( view );
	}

	DX_SAFE_RELEASE( m_depthStencilView );
	DX_SAFE_RELEASE( m_shaderResourceView );
	DX_SAFE_RELEASE( m_renderTargetView );
	DX_SAFE_RELEASE( m_handle );
}


//-----------------------------------------------------------------------------------------------
void Texture::WatchInternal( ID3D11Texture2D* handle )
{
	ReleaseResources();
	m_handle = handle;

	if (handle != nullptr)
	{
		m_handle->AddRef();
	}
}


//-----------------------------------------------------------------------------------------------
bool Texture::IsValid() const
{
	if (m_handle == nullptr)
	{
		return false;
	}

	return true;
}


#include "Renderer.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/D3D11Internal.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/DefaultShaderSource.hpp"
#include "Engine/Renderer/ErrorShaderSource.hpp"
#include "Engine/Renderer/Lighting/LightCamera.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Window/Window.hpp"

#include "ThirdParty/stb/stb_image.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <shobjidl.h>
#include <shobjidl_core.h>

#include <d3d11sdklayers.h>

#if defined(ENGINE_DEBUG_RENDERER)

#include <dxgidebug.h>

#endif

#pragma comment(lib, "dxguid.lib")


//-----------------------------------------------------------------------------------------------
Renderer* g_theRenderer = nullptr;


//-----------------------------------------------------------------------------------------------
Renderer::Renderer( RenderConfig const& config )
	: m_config( config )
{

}


//-----------------------------------------------------------------------------------------------
void Renderer::Startup()
{
	CreateRC();

#if defined(ENGINE_DEBUG_RENDERER)

	CreateDebugLayer();

#endif

	AcquireBackBufferRenderTargetView();
	m_cameraCBO = CreateConstantBuffer( sizeof( ShaderTransformationData ) );
	m_modelCBO = CreateConstantBuffer( sizeof( ModelTransformationData ) );
	m_lightCBO = CreateConstantBuffer( sizeof( ShaderLightData ) );
	m_immediateVBO = CreateDynamicVertexBuffer( sizeof( Vertex_PCUTBN ) );
	CreateDefaultAndErrorShader();
	CreateBlendStates();
	CreateSamplerStates();
	CreateDefaultTexture();
	m_defaultDepthStencil = CreateDepthStencilTexture( IntVec2( m_config.m_window->GetClientWidth(), m_config.m_window->GetClientHeight() ) );

#if defined(ENGINE_DEBUG_RENDERING)

	DebugRenderConfig config;
	config.m_renderer = this;
	config.m_startHidden = true;
	DebugRenderSystemStartup( config );

#endif
}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{

#if defined(ENGINE_DEBUG_RENDERING)

	DebugRenderBeginFrame();

#endif
}


//-----------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	::Sleep( 0 );

#if defined(ENGINE_DEBUG_RENDERING)

	DebugRenderEndFrame();

#endif

	m_swapChain->Present( 0, 0 );

}


//-----------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	DestroyRC();

#if defined (ENGINE_DEBUG_RENDERING)

	DebugRenderSystemShutdown();

#endif

	for ( int blendStateNum = 0; blendStateNum < static_cast< int >( BlendMode::NUMSTATES ); blendStateNum++ )
	{
		DX_SAFE_RELEASE( m_blendStates[ blendStateNum ] );
	}

	DestroySamplerStates();

	std::map<std::string, Texture*>::iterator it;

	for ( it = m_loadedTextures.begin(); it != m_loadedTextures.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	m_loadedTextures.clear();

	DestroyVertexBuffer( m_immediateVBO );
	m_immediateVBO = nullptr;

	DestroyConstantBuffer( m_cameraCBO );
	m_cameraCBO = nullptr;

	DestroyConstantBuffer( m_modelCBO );
	m_modelCBO = nullptr;

	DestroyConstantBuffer( m_lightCBO );
	m_lightCBO = nullptr;

#if defined(ENGINE_DEBUG_RENDERER)

	ReportLiveObjects();
	DestroyDebugLayer();

#endif

}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen( const Rgba8& clearColor )
{
	float clearColorAsFloats[ 4 ];
	clearColor.GetAsFloats( clearColorAsFloats );
	m_context->ClearRenderTargetView( m_backBuffer->m_renderTargetView, clearColorAsFloats );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( const Camera& camera )
{
	Vec3 bottomLeft = camera.GetOrthoBottomLeft();
	Vec3 topRight = camera.GetOrthoTopRight();

	m_currentCamera = &camera;

	ClearDepth();
	SetDepthOptions( DepthTest::ALWAYS, false );

	ID3D11RenderTargetView* renderTargetView = m_backBuffer->GetOrCreateRenderTargetView( this );
	ID3D11DepthStencilView* depthStencilView = m_defaultDepthStencil->GetOrCreateDepthStencilView( this );

	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_context->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	Vec2 screenSize = Vec2( static_cast< float >( m_config.m_window->GetClientWidth() ), static_cast< float >( m_config.m_window->GetClientHeight() ) );

	AABB2 viewPort = m_currentCamera->GetViewport( screenSize );
	float width = viewPort.GetDimensions().x;
	float height = fabsf( viewPort.GetDimensions().y );

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = viewPort.m_mins.x;
	viewport.TopLeftY = viewPort.m_mins.y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	m_context->RSSetViewports( 1, &viewport );

	ShaderTransformationData data;
	Mat44 renderMatrix = camera.GetRenderMatrix();
	renderMatrix.Append( camera.GetViewMatrix() );
	data.m_viewMatrix = renderMatrix;
	data.m_projectionMatrix = camera.GetProjectionMatrix();
	data.m_cameraPosition = camera.GetPosition();

	ModelTransformationData defaultData;
	Rgba8::WHITE.GetAsFloats( defaultData.tint );

	m_cameraCBO->SetData( &data, sizeof( ShaderTransformationData ) );
	m_modelCBO->SetData( defaultData );

	BindConstantBuffer( 2, m_cameraCBO );
	BindConstantBuffer( 3, m_modelCBO );

	BindShader( nullptr );
	BindTexture( nullptr );

	renderTargetView = nullptr;
	depthStencilView = nullptr;

	SetSamplerMode( SamplerMode::POINT_WRAP );
	SetBlendMode( BlendMode::ALPHA );

}


//-----------------------------------------------------------------------------------------------
void Renderer::BeginCamera( const LightCamera& camera, int cascadeNum /*= 0*/ )
{
	ClearDepthBuffer( camera.GetDepthTexture(), 1.0f, cascadeNum );
	ClearScreen( Rgba8::WHITE );
	SetDepthOptions( DepthTest::LESS_EQUAL, true );

	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11DepthStencilView* depthStencilView = camera.GetDepthTexture()->GetOrCreateDepthStencilBufferView( this, cascadeNum );

	m_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	m_context->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	Vec2 screenSize = Vec2( static_cast< float >( m_config.m_window->GetClientWidth() ), static_cast< float >( m_config.m_window->GetClientHeight() ) );

	AABB2 viewPort = camera.GetViewport( screenSize );

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX       = 0;
	viewport.TopLeftY       = 0;
	viewport.Width          = DEPTH_TEXTURE_SIZE;
	viewport.Height         = DEPTH_TEXTURE_SIZE;
	viewport.MinDepth       = 0;
	viewport.MaxDepth       = 1;

	m_context->RSSetViewports( 1, &viewport );

	ShaderTransformationData data;
	//Mat44 renderMatrix = camera.GetRenderMatrix().GetOrthonormalInverse();
	Mat44 renderViewMatrix = camera.GetRenderMatrix();
	renderViewMatrix.Append( camera.GetViewMatrix() );
	data.m_viewMatrix = renderViewMatrix;
	data.m_projectionMatrix = camera.GetProjectionMatrix( cascadeNum );
	data.m_cameraPosition   = camera.GetPosition();

	ModelTransformationData defaultData;
	Rgba8::WHITE.GetAsFloats( defaultData.tint );

	m_cameraCBO->SetData( &data, sizeof( ShaderTransformationData ) );
	m_modelCBO->SetData( defaultData );

	BindConstantBuffer( 2, m_cameraCBO );
	BindConstantBuffer( 3, m_modelCBO );

	BindShader( nullptr );
	BindTexture( nullptr );

	depthStencilView = nullptr;

	SetSamplerMode( SamplerMode::POINT_WRAP );
	SetDepthSamplerMode( 1, SamplerMode::BILINEAR_CLAMP );
	SetBlendMode( BlendMode::ALPHA );
}


//-----------------------------------------------------------------------------------------------
Camera const* Renderer::GetActiveCamera() const
{
	return m_currentCamera;
}


//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera( const Camera& camera )
{
	if ( m_currentCamera == &camera )
	{
		m_currentCamera = nullptr;
	}


}


//-----------------------------------------------------------------------------------------------
void Renderer::EndCamera( const LightCamera& camera )
{
	ID3D11RenderTargetView* rtv[ 1 ];
	rtv[ 0 ] = nullptr;

	m_context->OMSetRenderTargets( 1, rtv, NULL );

	UNUSED( camera );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes )
{
	m_immediateVBO->CopyVertexData( vertexes, sizeof( Vertex_PCU ) * numVertexes );
	DrawVertexBuffer( m_immediateVBO, numVertexes );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray( int numVertexes, const Vertex_PCUTBN* vertexes )
{
	m_immediateVBO->CopyVertexData( vertexes, sizeof( Vertex_PCUTBN ) * numVertexes, sizeof( Vertex_PCUTBN ) );
	DrawVertexBuffer( m_immediateVBO, numVertexes );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetBlendMode( BlendMode blendMode )
{
	if ( m_currentBlendMode == blendMode )
	{
		return;
	}

	float const blendConstants[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* state = m_blendStates[ static_cast< int >( blendMode ) ];

	m_context->OMSetBlendState( state, blendConstants, 0xffffffff );

	m_currentBlendMode = blendMode;

	state = nullptr;
}


//------------------------------------------------------------------------------------------------
static ID3D11BlendState* CreateBlendState( ID3D11Device* d3dDevice, D3D11_BLEND srcFactor, D3D11_BLEND destFactor, D3D11_BLEND_OP op )
{
	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[ 0 ].BlendEnable = true;
	desc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.RenderTarget[ 0 ].SrcBlend = srcFactor;
	desc.RenderTarget[ 0 ].DestBlend = destFactor;
	desc.RenderTarget[ 0 ].BlendOp = op;

	desc.RenderTarget[ 0 ].SrcBlendAlpha = srcFactor;
	desc.RenderTarget[ 0 ].DestBlendAlpha = destFactor;
	desc.RenderTarget[ 0 ].BlendOpAlpha = op;

	ID3D11BlendState* blendState = nullptr;
	d3dDevice->CreateBlendState( &desc, &blendState );

	return blendState;
}


//------------------------------------------------------------------------------------------------
static ID3D11RasterizerState* CreateRasterizerState( ID3D11Device* d3dDevice, RasterState state )
{

	D3D11_CULL_MODE cullMode;

	switch ( state.m_cullmode )
	{
		case CullMode::NONE:
			cullMode = D3D11_CULL_NONE;
			break;
		case CullMode::BACK:
			cullMode = D3D11_CULL_BACK;
			break;
		case CullMode::FRONT:
			cullMode = D3D11_CULL_FRONT;
			break;
		default:
			cullMode = D3D11_CULL_NONE;
	}

	D3D11_FILL_MODE fillMode;

	switch ( state.m_fillMode )
	{
		case FillMode::SOLID:
			fillMode = D3D11_FILL_SOLID;
			break;

		case FillMode::WIREFRAME:
			fillMode = D3D11_FILL_WIREFRAME;
			break;

		default:
			fillMode = D3D11_FILL_SOLID;
			break;
	}

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = fillMode;
	rasterDesc.CullMode = cullMode;
	rasterDesc.FrontCounterClockwise = state.m_windingOrder == WindingOrder::COUNTER_CLOCKWISE ? true : false;

	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;

	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = true;

	ID3D11RasterizerState* rasterState = nullptr;
	d3dDevice->CreateRasterizerState( &rasterDesc, &rasterState );

	return rasterState;
}


//------------------------------------------------------------------------------------------------
static ID3D11SamplerState* CreateSamplerState( ID3D11Device* d3dDevice, SamplerMode mode )
{
	D3D11_FILTER filter;
	D3D11_TEXTURE_ADDRESS_MODE addressMode;

	switch ( mode )
	{
		case SamplerMode::POINT_CLAMP:
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::POINT_WRAP:
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		case SamplerMode::BILINEAR_CLAMP:
		{

			filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::BILINEAR_WRAP:
		{
			filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		case SamplerMode::TRILINEAR_CLAMP:
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::TRILINEAR_WRAP:
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		case SamplerMode::ANISOTROPIC_CLAMP:
		{
			filter = D3D11_FILTER_ANISOTROPIC;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::ANISOTROPIC_WRAP:
		{
			filter = D3D11_FILTER_ANISOTROPIC;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		default:
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

	}

	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.Filter = filter;
	sampleDesc.AddressU = addressMode;
	sampleDesc.AddressV = addressMode;
	sampleDesc.AddressW = addressMode;

	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MinLOD = 0.0f;
	sampleDesc.MaxLOD = 0.0f;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_EQUAL;

	sampleDesc.MaxAnisotropy = 0;

	ID3D11SamplerState* state = nullptr;

	d3dDevice->CreateSamplerState( &sampleDesc, &state );

	ASSERT_OR_DIE( state != nullptr, "Failed to create Sampler State" );

	return state;
}


//------------------------------------------------------------------------------------------------
static ID3D11SamplerState* CreateDepthSamplerState( ID3D11Device* d3dDevice, SamplerMode mode )
{
	D3D11_FILTER filter;
	D3D11_TEXTURE_ADDRESS_MODE addressMode;

	switch ( mode )
	{
		case SamplerMode::POINT_CLAMP:
		{
			filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::POINT_WRAP:
		{
			filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		case SamplerMode::BILINEAR_CLAMP:
		{

			filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::BILINEAR_WRAP:
		{
			filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		case SamplerMode::TRILINEAR_CLAMP:
		{
			filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::TRILINEAR_WRAP:
		{
			filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		case SamplerMode::ANISOTROPIC_CLAMP:
		{
			filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
			addressMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		}
		break;

		case SamplerMode::ANISOTROPIC_WRAP:
		{
			filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

		default:
		{
			filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			addressMode = D3D11_TEXTURE_ADDRESS_WRAP;
		}
		break;

	}

	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.Filter = filter;
	sampleDesc.AddressU = addressMode;
	sampleDesc.AddressV = addressMode;
	sampleDesc.AddressW = addressMode;

	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MinLOD = 0.0f;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_LESS;

	sampleDesc.MaxAnisotropy  = 1u;
	sampleDesc.BorderColor[0] = 0.0f;
	sampleDesc.BorderColor[1] = 0.0f;
	sampleDesc.BorderColor[2] = 0.0f;
	sampleDesc.BorderColor[3] = 0.0f;

	ID3D11SamplerState* state = nullptr;

	d3dDevice->CreateSamplerState( &sampleDesc, &state );

	ASSERT_OR_DIE( state != nullptr, "Failed to create Sampler State" );

	return state;

	//D3D11_SAMPLER_DESC sampleDesc = {};
	//sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	//sampleDesc.BorderColor[ 0 ] = 1.0f;
	//sampleDesc.BorderColor[ 1 ] = 1.0f;
	//sampleDesc.BorderColor[ 2 ] = 1.0f;
	//sampleDesc.BorderColor[ 3 ] = 1.0f;
	//sampleDesc.MinLOD = 0.f;
	//sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//sampleDesc.MipLODBias = 0.f;
	//sampleDesc.MaxAnisotropy = 0;
	//sampleDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	//sampleDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	//ID3D11SamplerState* state = nullptr;

	//d3dDevice->CreateSamplerState( &sampleDesc, &state );

	//ASSERT_OR_DIE( state != nullptr, "Failed to create Sampler State" );

	//return state;
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindShader( Shader* shader )
{
	if ( shader == nullptr )
	{
		shader = m_defaultShader;
		m_currentShader = m_defaultShader;
	}

	m_context->VSSetShader( shader->m_vertexShader, nullptr, 0 );
	m_context->PSSetShader( shader->m_pixelShader, nullptr, 0 );
	m_context->GSSetShader( shader->m_geometryShader, nullptr, 0 );
	m_currentShader = shader;
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindShaderByName( char const* shaderName )
{
	Shader* shader = CreateOrGetShaderFromFile( shaderName );
	BindShader( shader );
	m_currentShader = shader;
}


//-----------------------------------------------------------------------------------------------
Shader* Renderer::CreateOrGetShaderFromFile( char const* fileNameWithoutExtension )
{
	for ( int shaderNum = 0; shaderNum < m_loadedShaders.size(); shaderNum++ )
	{
		std::string shaderPath = m_loadedShaders[ shaderNum ]->m_config.m_name;

		if ( _strcmpi( fileNameWithoutExtension, shaderPath.c_str() ) == 0 )
		{
			return m_loadedShaders[ shaderNum ];
		}
	}

	Shader* newShader = CreateShaderFromFile( fileNameWithoutExtension );

	return newShader;
}


//-----------------------------------------------------------------------------------------------
Shader* Renderer::CreateOrGetShaderFromSource( char const* shaderName, std::string source )
{
	for ( int shaderNum = 0; shaderNum < m_loadedShaders.size(); shaderNum++ )
	{
		std::string shaderPath = m_loadedShaders[ shaderNum ]->m_config.m_name;

		if ( _strcmpi( shaderName, shaderPath.c_str() ) == 0 )
		{
			return m_loadedShaders[ shaderNum ];
		}
	}

	Shader* newShader = CreateShaderFromSource( shaderName, source );
	m_loadedShaders.push_back( newShader );
	return newShader;
}


//-----------------------------------------------------------------------------------------------
ConstantBuffer* Renderer::CreateConstantBuffer( size_t const size )
{
	ConstantBuffer* newConstantBuffer = new ConstantBuffer( this, size );
	return newConstantBuffer;
}


//-----------------------------------------------------------------------------------------------
void Renderer::DestroyConstantBuffer( ConstantBuffer* cbo )
{
	DX_SAFE_RELEASE( cbo->m_gpuBuffer );
	delete cbo;
	cbo = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetModelBuffer( ModelTransformationData data )
{
	if ( !m_modelCBO->SetData( data ) )
	{
		ERROR_RECOVERABLE( "Error in setting the model matrix" );
	}

	BindConstantBuffer( 3, m_modelCBO );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetLightBuffer( ShaderLightData data )
{
	if ( !m_lightCBO->SetData( data ) )
	{
		ERROR_RECOVERABLE( "Error in setting the light data" );
	}

	BindConstantBuffer( 4, m_lightCBO );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindConstantBuffer( int slot, ConstantBuffer* constantBuffer )
{
	ID3D11Buffer* cboHandle = constantBuffer->GetHandle();

	m_context->VSSetConstantBuffers( slot, 1, &cboHandle );
	m_context->PSSetConstantBuffers( slot, 1, &cboHandle );
	m_context->GSSetConstantBuffers( slot, 1, &cboHandle );
}


//-----------------------------------------------------------------------------------------------
VertexBuffer* Renderer::CreateDynamicVertexBuffer( size_t const initialByteSize /*= 0 */ )
{
	VertexBuffer* newVertexBuffer = new VertexBuffer( this );

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = static_cast< UINT >( initialByteSize );
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer( &bufferDesc, nullptr, &newVertexBuffer->m_gpuBuffer );
	ASSERT_OR_DIE( newVertexBuffer->m_gpuBuffer != nullptr, "Failed to create Vertex Buffer." );

	return newVertexBuffer;
}


//-----------------------------------------------------------------------------------------------
void Renderer::CreateNewVertexBuffer( VertexBuffer* vertexBuffer, size_t byteSize )
{
	if ( vertexBuffer != nullptr )
	{
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = static_cast< UINT >( byteSize );
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		m_device->CreateBuffer( &bufferDesc, nullptr, &vertexBuffer->m_gpuBuffer );
		ASSERT_OR_DIE( vertexBuffer->m_gpuBuffer != nullptr, "Failed to create Vertex Buffer." );
	}

	else
	{
		ERROR_AND_DIE( "Trying to create a GPU Buffer when Vertex buffer has not been created" );
	}
}

//-----------------------------------------------------------------------------------------------
void Renderer::DestroyVertexBuffer( VertexBuffer const* vbo )
{
	delete vbo;
	vbo = nullptr;
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateDepthStencilTexture( IntVec2 size )
{
	Image image = Image();
	image.SetupAsSolidColor( size.x, size.y, Rgba8::WHITE );

	Texture* depthStencilTexture = new Texture();
	depthStencilTexture->CreateDepthStencilTarget( this, image );

	return depthStencilTexture;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetDepthOptions( DepthTest test, bool writeDepth )
{
	m_desiredDepthTest = test;
	m_writeDepth = writeDepth;
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearDepth( float depthValue /*= 1.0f */ )
{
	ID3D11DepthStencilView* view = m_defaultDepthStencil->GetOrCreateDepthStencilView( this );

	if ( view == nullptr )
	{
		ERROR_AND_DIE( "Depth Stencil View is Nullptr" );
	}

	m_context->ClearDepthStencilView( view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthValue, '\0' );

	view = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearDepth( Texture* depthTexture, float depthValue /*= 1.0f */ )
{
	if ( depthTexture == nullptr )
	{
		ERROR_AND_DIE( "Depth Texture is Nullptr" );
	}

	ID3D11DepthStencilView* view = depthTexture->GetOrCreateDepthStencilView( this );

	if ( view == nullptr )
	{
		ERROR_AND_DIE( "Depth Stencil View is Nullptr" );
	}

	m_context->ClearDepthStencilView( view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthValue, '\0' );

	view = nullptr;
}


//------------------------------------------------------------------------------------------------
void Renderer::ClearDepthBuffer( Texture* depthTexture, float depthValue /*= 1.0f*/, uint bufferIndex /*= 0 */)
{
	if ( depthTexture == nullptr )
	{
		ERROR_AND_DIE( "Depth Texture is Nullptr" );
	}

	ID3D11DepthStencilView* view = depthTexture->GetOrCreateDepthStencilBufferView( this, bufferIndex );

	if ( view == nullptr )
	{
		ERROR_AND_DIE( "Depth Stencil View is Nullptr" );
	}

	m_context->ClearDepthStencilView( view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthValue, '\0' );

	view = nullptr;
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateDepthBufferTexture( IntVec2 size, uint numCascades /*= 1*/ )
{
	Texture* depthStencilTexture = new Texture();
	depthStencilTexture->CreateDepthBufferTarget( this, size, numCascades );

	return depthStencilTexture;
}


//-----------------------------------------------------------------------------------------------
void Renderer::Draw( int vertexCount, int vertexOffset /*= 0*/ )
{
	if ( IsRasterStateDirty() )
	{
		DX_SAFE_RELEASE( m_rasterState );
		m_rasterState = CreateRasterizerState( m_device, m_desiredState );
		m_context->RSSetState( m_rasterState );

		m_currentState = m_desiredState;
	}

	UpdateDepthStencilState();

	m_context->Draw( vertexCount, vertexOffset );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer( VertexBuffer const* vbo, int vertexCount )
{
	ID3D11Buffer* vboHandle = vbo->GetHandle();
	UINT stride = static_cast< UINT >( vbo->GetStride() );
	UINT offset = 0;

	m_context->IASetVertexBuffers(
		0,
		1,
		&vboHandle,
		&stride,
		&offset
	);

	ID3D11InputLayout* layout = nullptr;

	if ( stride == static_cast< UINT >( sizeof( Vertex_PCUTBN ) ) )
	{
		layout = m_currentShader->CreateOrGetInputLayoutFor_Vertex_PCUTBN();
	}
	else
	{
		layout = m_currentShader->CreateOrGetInputLayoutFor_Vertex_PCU();
	}
	m_context->IASetInputLayout( layout );

	Draw( vertexCount );
}


//-----------------------------------------------------------------------------------------------
bool Renderer::IsRasterStateDirty()
{
	if ( m_rasterState == nullptr ||
		 ( m_currentState.m_cullmode != m_desiredState.m_cullmode ||
		 m_currentState.m_windingOrder != m_desiredState.m_windingOrder ||
		 m_currentState.m_fillMode != m_desiredState.m_fillMode ) )
		return true;

	return false;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetRasterState( RasterState state )
{
	m_desiredState = state;
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetSamplerMode( SamplerMode mode, int slot /*= 0 */ )
{
	ID3D11SamplerState* state = m_samplerStates[ static_cast< int >( mode ) ];
	m_context->PSSetSamplers( slot, 1, &state );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetDepthSamplerMode( int slot /*= 1 */, SamplerMode mode /*= SamplerMode::POINT_WRAP*/  )
{
	ID3D11SamplerState* state = m_depthBufferSamplerState[static_cast< int >( mode )];
	m_context->PSSetSamplers( slot, 1, &state );
}


//-----------------------------------------------------------------------------------------------
ID3D11Device* Renderer::GetDevice() const
{
	return m_device;
}


//-----------------------------------------------------------------------------------------------
ID3D11DeviceContext* Renderer::GetDeviceContext() const
{
	return m_context;
}


//-----------------------------------------------------------------------------------------------
IDXGISwapChain* Renderer::GetSwapChain() const
{
	return m_swapChain;
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFile( char const* imageFilePath )
{
	std::map<std::string, Texture* >::iterator it = m_loadedTextures.find( imageFilePath );

	if ( it != m_loadedTextures.end() )
	{
		return it->second;
	}

	Texture* newTexture = CreateTextureFromFile( imageFilePath );

	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetCubeTextureFromFiles( std::vector<std::string>& imagePaths )
{
	std::map<std::string, Texture* >::iterator it = m_loadedTextures.find( imagePaths[0].c_str() );

	if ( it != m_loadedTextures.end() )
	{
		return it->second;
	}

	Texture* newTexture = CreateTextureCubeFromFile( imagePaths );

	return newTexture;
}


//-----------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateOrGetBitmapFontFromFile( char const* filePathWithoutExtension )
{
	int numFonts = static_cast< int >( m_loadedFonts.size() );

	for ( int fontNum = 0; fontNum < numFonts; fontNum++ )
	{
		std::string fontImgPath = m_loadedFonts[ fontNum ]->GetFontPathWithoutExtension();

		if ( _strcmpi( filePathWithoutExtension, fontImgPath.c_str() ) == 0 )
		{
			return m_loadedFonts[ fontNum ];
		}
	}

	BitmapFont* newFont = CreateBitmapFontFromFile( filePathWithoutExtension );

	return newFont;
}


//------------------------------------------------------------------------------------------------
void Renderer::DeleteTexture( Texture*& texture )
{
	if ( texture == nullptr )
		return;

	delete texture;
	texture = nullptr;
}


//------------------------------------------------------------------------------------------------
void Renderer::CreateRC()
{
	DWORD deviceFlags = 0;
	deviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

#if defined(ENGINE_DEBUG_RENDERER)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	Window* window = m_config.m_window;
	HWND hwnd = ( HWND ) ( window->GetHWND() );

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { };
	swapChainDesc.BufferDesc.Width = m_config.m_window->GetClientWidth();
	swapChainDesc.BufferDesc.Height = m_config.m_window->GetClientHeight();
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.SampleDesc.Count = 1;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_context
	);

	ASSERT_OR_DIE( SUCCEEDED( result ), "Failed to create D3D11 Device!" );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DestroyRC()
{
	if ( m_currentShader != nullptr )
	{
		m_currentShader = nullptr;
	}

	for ( int shaderNum = 0; shaderNum < m_loadedShaders.size(); shaderNum++ )
	{
		m_loadedShaders[ shaderNum ]->Destroy();
		delete m_loadedShaders[ shaderNum ];
		m_loadedShaders[ shaderNum ] = nullptr;
	}

	if ( m_defaultShader != nullptr )
	{
		m_defaultShader->Destroy();
		delete m_defaultShader;
		m_defaultShader = nullptr;
	}

	if ( m_errorShader != nullptr )
	{
		m_errorShader->Destroy();
		delete m_errorShader;
		m_errorShader = nullptr;
	}

	DX_SAFE_RELEASE( m_rasterState );

	m_backBuffer->ReleaseResources();
	delete m_backBuffer;
	m_backBuffer = nullptr;

	DX_SAFE_RELEASE( m_depthStencilState );

	m_defaultDepthStencil->ReleaseResources();
	delete m_defaultDepthStencil;
	m_defaultDepthStencil = nullptr;

	DX_SAFE_RELEASE( m_device );
	DX_SAFE_RELEASE( m_context );
	DX_SAFE_RELEASE( m_swapChain );
}


//-----------------------------------------------------------------------------------------------
void Renderer::CreateDefaultAndErrorShader()
{
	std::string shaderNameDefault = "Default";
	std::string shaderSourceDefault = g_defaultShaderSource;
	m_defaultShader = CreateShaderFromSource( shaderNameDefault.c_str(), shaderSourceDefault );

	std::string shaderNameError = "Error";
	std::string shaderSourceError = g_errorShaderSource;
	m_errorShader = CreateShaderFromSource( shaderNameError.c_str(), shaderSourceError );
}


//-----------------------------------------------------------------------------------------------
void Renderer::CreateBlendStates()
{
	m_blendStates[ static_cast< int >( BlendMode::OPAQUE ) ] = CreateBlendState( m_device, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD );
	m_blendStates[ static_cast< int >( BlendMode::ALPHA ) ] = CreateBlendState( m_device, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD );
	m_blendStates[ static_cast< int >( BlendMode::ADDITIVE ) ] = CreateBlendState( m_device, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD );
}


#if defined(ENGINE_DEBUG_RENDERER)

//-----------------------------------------------------------------------------------------------
void Renderer::CreateDebugLayer()
{
	HMODULE modHandle = ::LoadLibraryA( "Dxgidebug.dll" );

	if ( modHandle == nullptr )
	{
		ERROR_RECOVERABLE( "Failed to Load Dxgidebug.dll" );
		return;
	}

	typedef HRESULT( WINAPI* GetDebugModuleCB )( REFIID, void** );

	GetDebugModuleCB getModuleFunc = ( GetDebugModuleCB )::GetProcAddress( modHandle, "DXGIGetDebugInterface" );

	ASSERT_OR_DIE( getModuleFunc != nullptr, "Failed to load Get Debug Interface." );

	HRESULT result = getModuleFunc( __uuidof( IDXGIDebug ), reinterpret_cast< void** > ( &m_debug ) );
	ASSERT_OR_DIE( SUCCEEDED( result ), "get Debug Module Function Failed" );

	m_debugModule = modHandle;

}


//-----------------------------------------------------------------------------------------------
void Renderer::SetResourceDebugName( ID3D11DeviceChild* obj, char const* name )
{
	if ( obj != nullptr )
	{
		obj->SetPrivateData(
			WKPDID_D3DDebugObjectName,
			static_cast< UINT >( strlen( name ) + 1 ),
			name
		);
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::ReportLiveObjects()
{
	if ( m_debug != nullptr )
	{
		DXGI_DEBUG_RLO_FLAGS const flags = ( DXGI_DEBUG_RLO_FLAGS ) ( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL );

		m_debug->ReportLiveObjects( DXGI_DEBUG_ALL, flags );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::DestroyDebugLayer()
{
	if ( m_debugModule )
	{
		DX_SAFE_RELEASE( m_debug );
		::FreeLibrary( ( HMODULE ) m_debugModule );
		m_debugModule = nullptr;
	}
}

#endif


//-----------------------------------------------------------------------------------------------
void Renderer::AcquireBackBufferRenderTargetView()
{
	m_backBuffer = new Texture();
	m_backBuffer->GetOrCreateRenderTargetView( this );
}


//-----------------------------------------------------------------------------------------------
Shader* Renderer::CreateShaderFromFile( char const* shaderPathWithoutExtension )
{
	Shader* newShader = new Shader();

	ShaderConfig shaderConfig;
	shaderConfig.m_name = shaderPathWithoutExtension;

	newShader->Create( this, shaderConfig );

	m_loadedShaders.push_back( newShader );

	return newShader;
}


//-----------------------------------------------------------------------------------------------
Shader* Renderer::CreateShaderFromSource( char const* shaderName, std::string source )
{
	Shader* newShader = new Shader();

	ShaderConfig shaderConfig;
	shaderConfig.m_name = shaderName;

	newShader->Create( this, shaderConfig, source );

	return newShader;
}


//-----------------------------------------------------------------------------------------------
void Renderer::CreateSamplerStates()
{
	for ( int stateNum = 0; stateNum < static_cast< int >( SamplerMode::COUNT ); stateNum++ )
	{
		m_samplerStates[ stateNum ] = CreateSamplerState( m_device, static_cast< SamplerMode >( stateNum ) );
		m_depthBufferSamplerState[stateNum] = CreateDepthSamplerState( m_device, static_cast< SamplerMode >( stateNum ) );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::DestroySamplerStates()
{
	for ( int stateNum = 0; stateNum < static_cast< int >( SamplerMode::COUNT ); stateNum++ )
	{
		DX_SAFE_RELEASE( m_samplerStates[ stateNum ] );
		DX_SAFE_RELEASE( m_depthBufferSamplerState[stateNum] );
	}

}


//-----------------------------------------------------------------------------------------------
void Renderer::UpdateDepthStencilState()
{
	if ( m_depthStencilState != nullptr && m_desiredDepthTest == m_currentDepthTest )
		return;

	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = true;
	desc.DepthWriteMask = m_writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

	switch ( m_desiredDepthTest )
	{
		case DepthTest::LESS_EQUAL:
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			break;

		case DepthTest::ALWAYS:
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			break;

		case DepthTest::GREATER_EQUAL:
			desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			break;

		case DepthTest::GREATER:
			desc.DepthFunc = D3D11_COMPARISON_GREATER;
			break;

		default:
			ERROR_AND_DIE( "Error in depth test State" );
			break;
	}

	desc.StencilEnable = false;

	DX_SAFE_RELEASE( m_depthStencilState );

	m_device->CreateDepthStencilState( &desc, &m_depthStencilState );
	m_context->OMSetDepthStencilState( m_depthStencilState, 0 );

	m_currentDepthTest = m_desiredDepthTest;
}


//-----------------------------------------------------------------------------------------------
void Renderer::CreateDefaultTexture()
{
	m_defaultDiffuse = RegisterColorTexture( "WHITE", Rgba8::WHITE );
	m_defaultNormal = RegisterColorTexture( "NORMAL", Rgba8( 127, 127, 255 ) );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture( Texture const* texture, int slot /*= 0*/ )
{
	if ( texture == nullptr )
	{
		switch ( slot )
		{
			case 0:
				texture = m_defaultDiffuse;
				ASSERT_OR_DIE( m_defaultDiffuse != nullptr, "Default Diffuse Map is nullptr" );
				break;

			case 1:
				texture = m_defaultNormal;
				ASSERT_OR_DIE( m_defaultNormal != nullptr, "Default Normal Map is nullptr" );
				break;
		}

	}

	ID3D11ShaderResourceView* srv = const_cast< Texture* >( texture )->GetOrCreateShaderResourceView( this );
	m_context->PSSetShaderResources( slot, 1, &srv );
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearTextureAtSlot( int slot /*= 8 */ )
{
	ID3D11ShaderResourceView* srv = nullptr;
	m_context->PSSetShaderResources( slot, 1, &srv );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindDepthTexture( const Texture* texture, int slot /*= 8 */ )
{
	if ( texture == nullptr )
	{
		ERROR_AND_DIE( "BINDING NULLPTR" );
	}

	ID3D11ShaderResourceView* srv = const_cast< Texture* >( texture )->GetOrCreateShaderResourceViewDepthBuffer( this );
	m_context->PSSetShaderResources( slot, 1, &srv );
}


//------------------------------------------------------------------------------------------------
void Renderer::BindCubeTexture( const Texture* texture, int slot /*= 0 */ )
{
	if ( texture == nullptr )
	{
		ERROR_AND_DIE( "BINDING NULLPTR" );
	}

	ID3D11ShaderResourceView* srv = const_cast< Texture* >( texture )->GetOrCreateShaderResourceViewTextureCube( this );
	m_context->PSSetShaderResources( slot, 1, &srv );
}


//-----------------------------------------------------------------------------------------------
void Renderer::RecompileAllShaders()
{
	for ( auto shader : m_loadedShaders )
	{
		shader->RecompileShader();
	}
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::RegisterColorTexture( char const* textureName, Rgba8 const& color )
{
	Image image;
	image.SetupAsSolidColor( 1, 1, color );

	Texture* texture = new Texture();
	texture->CreateFromImage( this, textureName, image );

	RegisterTexture( textureName, texture );

	return texture;
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::RegisterTexture( char const* textureName, Texture* texture )
{
	m_loadedTextures[ std::string( textureName ) ] = texture;
	return texture;
}


//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile( const char* imageFilePath )
{
	Texture* newTexture = new Texture();
	newTexture->LoadFromFile( this, imageFilePath );

	if ( newTexture->IsValid() )
	{
		RegisterTexture( imageFilePath, newTexture );
	}
	else
	{
		delete newTexture;
		newTexture = nullptr;
	}

	return newTexture;
}


//------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureCubeFromFile( std::vector<std::string>& imageFilePaths )
{
	Texture* newTexture = new Texture();
	
	newTexture->CreateCubemapfromFile( this, imageFilePaths );

	if ( newTexture->IsValid() )
	{
		RegisterTexture( imageFilePaths[0].c_str(), newTexture );
	}
	else
	{
		delete newTexture;
		newTexture = nullptr;
	}

	return newTexture;
}


//-----------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateBitmapFontFromFile( char const* filePathWithoutExtension )
{
	std::string fontFile = std::string( std::string( filePathWithoutExtension ).append( ".png" ).c_str() );

	Texture* texture = CreateOrGetTextureFromFile( fontFile.c_str() );
	BitmapFont* font = new BitmapFont( filePathWithoutExtension, *texture );

	m_loadedFonts.push_back( font );

	return font;
}


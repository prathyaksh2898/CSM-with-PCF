#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/LightStructure.hpp"

#include <vector>


//-----------------------------------------------------------------------------------------------
class Window;
class Texture;
class BitmapFont;
class Shader;
class ConstantBuffer;
class VertexBuffer;

struct LightCamera;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct IDXGISwapChain;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11Texture2D;
struct ID3D11DepthStencilState;

#if defined(ENGINE_DEBUG_RENDERER)

struct IDXGIDebug;
struct ID3D11DeviceChild;

#endif


//------------------------------------------------------------------------------------------------
struct ShaderTransformationData
{
	Mat44 m_viewMatrix;
	Mat44 m_projectionMatrix;
	Vec4  m_cameraPosition;
};


//------------------------------------------------------------------------------------------------
struct ModelTransformationData
{
	Mat44 modelMatrix;
	float tint[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
};


//------------------------------------------------------------------------------------------------
enum class DepthTest : unsigned char
{
	ALWAYS,
	NEVER,
	EQUAL,
	NOT_EQUAL,
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
};


//------------------------------------------------------------------------------------------------
enum class SamplerMode
{
	POINT_CLAMP = 0,
	POINT_WRAP,
	BILINEAR_CLAMP,
	BILINEAR_WRAP,
	TRILINEAR_CLAMP,
	TRILINEAR_WRAP,
	ANISOTROPIC_CLAMP,
	ANISOTROPIC_WRAP,
			  
	COUNT,
};


//-----------------------------------------------------------------------------------------------
enum class BlendMode
{
	OPAQUE,
	ALPHA,
	ADDITIVE,
	
	NUMSTATES
};


//-----------------------------------------------------------------------------------------------
enum class CullMode
{
	NONE = 1,
	FRONT,
	BACK
};
   

//-----------------------------------------------------------------------------------------------
enum class WindingOrder
{
	CLOCKWISE,
	COUNTER_CLOCKWISE
};


//-----------------------------------------------------------------------------------------------
enum class FillMode
{
	SOLID,
	WIREFRAME
};


//-----------------------------------------------------------------------------------------------
struct RasterState
{
	CullMode      m_cullmode       = CullMode::NONE;
	WindingOrder  m_windingOrder   = WindingOrder::COUNTER_CLOCKWISE;
	FillMode      m_fillMode       = FillMode::SOLID;
};


//-----------------------------------------------------------------------------------------------
struct RenderConfig
{
	Window* m_window = nullptr;
};


//-----------------------------------------------------------------------------------------------
class Renderer 
{
public:
	Renderer( RenderConfig const& config );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			BASIC ENGINE SYSTEM CALLS
//--------------------------------------------------------------------------------------------------------------------------------------------

	void                 Startup();
	void                 BeginFrame();
	void                 EndFrame();
	void                 Shutdown();


//--------------------------------------------------------------------------------------------------------------------------------------------
//			PRE CAMERA WORK AND POST CAMERA WORK
//--------------------------------------------------------------------------------------------------------------------------------------------


	void                 ClearScreen( const Rgba8& clearColor );
	Camera const*        GetActiveCamera() const;
	void                 BeginCamera( const Camera& camera );
	void                 EndCamera( const Camera& camera );


//--------------------------------------------------------------------------------------------------------------------------------------------
//			PRE LIGHTING CAMERA AND POST LIGHTING CAMERA CALLS
//--------------------------------------------------------------------------------------------------------------------------------------------

	
	void                 BeginCamera( const LightCamera& camera, int cascadeNum /*= 0*/ );
	void                 EndCamera( const LightCamera& camera );


//--------------------------------------------------------------------------------------------------------------------------------------------
//			DRAW CALLS FOR VERTEX TYPES
//--------------------------------------------------------------------------------------------------------------------------------------------


	void                 DrawVertexArray( int numVertexes, const Vertex_PCU* vertexes );
	void                 DrawVertexArray( int numVertexes, const Vertex_PCUTBN* vertexes );


//--------------------------------------------------------------------------------------------------------------------------------------------
//			CREATING TEXTURES AND FONTS - PUBLIC INTERFACE
//--------------------------------------------------------------------------------------------------------------------------------------------
	

	Texture*             CreateOrGetTextureFromFile( const char* imageFilePath );
	Texture*             CreateOrGetCubeTextureFromFiles( std::vector<std::string>& imagePaths );
	BitmapFont*          CreateOrGetBitmapFontFromFile(const char* filePathWithoutExtension);
	
	void                 DeleteTexture( Texture*& texture );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			CREATING TEXTURES AND FONTS - PUBLIC INTERFACE
//--------------------------------------------------------------------------------------------------------------------------------------------

	void                 SetBlendMode( BlendMode blendMode );

	void                 BindShader( Shader* shader );
	void                 BindShaderByName( char const* shaderName );
	void                 BindTexture( const Texture* texture, int slot = 0 );
	void                 ClearTextureAtSlot( int slot = 8 );
	void                 BindDepthTexture( const Texture* texture, int slot = 8 );
	void                 BindCubeTexture( const Texture* texture, int slot = 0 );
	void                 RecompileAllShaders();
	
	ID3D11Device*        GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	IDXGISwapChain*      GetSwapChain() const;
	Shader*              CreateOrGetShaderFromFile( char const* fileNameWithoutExtension );
	Shader*              CreateOrGetShaderFromSource( char const* shaderName, std::string source );


//--------------------------------------------------------------------------------------------------------------------------------------------
//			CONSTANT BUFFER MANAGEMENT
//--------------------------------------------------------------------------------------------------------------------------------------------				     
	
	
	ConstantBuffer*      CreateConstantBuffer(size_t const size);
	void                 DestroyConstantBuffer(ConstantBuffer* cbo);
	void                 BindConstantBuffer( int slot, ConstantBuffer* constantBuffer );
	void				 SetModelBuffer( ModelTransformationData data );
	void				 SetLightBuffer( ShaderLightData data );
					     

//--------------------------------------------------------------------------------------------------------------------------------------------
//			VERTEX BUFFER MANAGEMENT
//--------------------------------------------------------------------------------------------------------------------------------------------


	VertexBuffer*        CreateDynamicVertexBuffer( size_t const initialByteSize = 0 );
	void                 CreateNewVertexBuffer( VertexBuffer* vertexBuffer, size_t byteSize );
	void                 DestroyVertexBuffer( VertexBuffer const* vbo );

	Texture*             CreateDepthStencilTexture( IntVec2 size );
	void                 SetDepthOptions( DepthTest test, bool writeDepth ); 
	void                 ClearDepth( float depthValue = 1.0f );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			DEPTH BUFFER MANAGEMENT
//--------------------------------------------------------------------------------------------------------------------------------------------


	Texture*             CreateDepthBufferTexture( IntVec2 size, uint numCascades = 1 );
	void                 ClearDepth( Texture* depthTexture, float depthValue = 1.0f );
	void                 ClearDepthBuffer( Texture* depthTexture, float depthValue = 1.0f, uint bufferIndex = 0 );


//--------------------------------------------------------------------------------------------------------------------------------------------
//			DIRECTX 11 DRAWs
//--------------------------------------------------------------------------------------------------------------------------------------------
		                 
	void                 Draw( int vertexCount, int vertexOffset = 0 );
	void                 DrawVertexBuffer( VertexBuffer const* vbo, int vertexCount );
		                 
	bool                 IsRasterStateDirty();
	void                 SetRasterState( RasterState state );
		                 
	void                 SetSamplerMode( SamplerMode mode, int slot = 0 );
	void                 SetDepthSamplerMode( int slot = 1, SamplerMode samplemode = SamplerMode::POINT_WRAP );

#if defined(ENGINE_DEBUG_RENDERER)

	void                 SetResourceDebugName( ID3D11DeviceChild* obj, char const* name );

#endif
	
protected:
	void                 CreateRC();
	void                 DestroyRC();

	void                 CreateDefaultAndErrorShader();
	void                 CreateBlendStates();

#if defined(ENGINE_DEBUG_RENDERER)

	void                 CreateDebugLayer();
	void                 ReportLiveObjects();
	void                 DestroyDebugLayer();

#endif

	void                 AcquireBackBufferRenderTargetView();

//--------------------------------------------------------------------------------------------------------------------------------------------
//			SHADER CREATION
//--------------------------------------------------------------------------------------------------------------------------------------------

	Shader*              CreateShaderFromFile(char const* shaderPathWithoutExtension);
	Shader*              CreateShaderFromSource( char const* shaderName, std::string source );


//--------------------------------------------------------------------------------------------------------------------------------------------
//			PIPELINE STATES
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	void		         CreateSamplerStates();
	void		         DestroySamplerStates();
	void                 UpdateDepthStencilState();


//--------------------------------------------------------------------------------------------------------------------------------------------
//			CREATING TEXTURES AND FONTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	void                 CreateDefaultTexture();
	Texture*             RegisterColorTexture( char const* textureName, Rgba8 const& color );
	Texture*             RegisterTexture( char const* textureName, Texture* texture );
	Texture*             CreateTextureFromFile(char const* imageFilePath);
	Texture*             CreateTextureCubeFromFile(std::vector<std::string>& imageFilePaths);
	BitmapFont*          CreateBitmapFontFromFile(char const* filePathWithoutExtension);


protected:

//--------------------------------------------------------------------------------------------------------------------------------------------
//			DATA
//--------------------------------------------------------------------------------------------------------------------------------------------


	RenderConfig                    m_config;
	std::map<std::string, Texture*> m_loadedTextures;
	std::vector<BitmapFont*>        m_loadedFonts;

	Camera const*                   m_currentCamera       = nullptr;
								    

#if defined(ENGINE_DEBUG_RENDERER)

	void*                           m_debugModule         = nullptr;
	IDXGIDebug*                     m_debug               = nullptr;
								    				      
#endif							    				      
									    				      
	std::vector<Shader*>            m_loadedShaders;      
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			CONSTANT BUFFERS
//--------------------------------------------------------------------------------------------------------------------------------------------
	

	ConstantBuffer*                 m_cameraCBO           = nullptr;
	ConstantBuffer*                 m_modelCBO            = nullptr;
	ConstantBuffer*                 m_lightCBO            = nullptr;
	

//--------------------------------------------------------------------------------------------------------------------------------------------
//			VERTEX BUFFERS
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	VertexBuffer*                   m_immediateVBO        = nullptr;
	
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			SHADERS AND TEXTURES
//--------------------------------------------------------------------------------------------------------------------------------------------

	Shader*                         m_currentShader       = nullptr;
	Shader*                         m_defaultShader       = nullptr;
	Shader*                         m_errorShader         = nullptr;
	Texture*                        m_backBuffer          = nullptr;
	Texture*                        m_defaultDiffuse      = nullptr;
	Texture*                        m_defaultNormal       = nullptr;
	Texture*                        m_defaultDepthStencil = nullptr;


//--------------------------------------------------------------------------------------------------------------------------------------------
//			PIPELINE STATES
//--------------------------------------------------------------------------------------------------------------------------------------------
								    				   
	RasterState                     m_desiredState;	   
	RasterState                     m_currentState;	   
								    				   
	BlendMode				        m_currentBlendMode    = BlendMode::OPAQUE;
								    
	DepthTest                       m_desiredDepthTest    = DepthTest::ALWAYS;
	DepthTest                       m_currentDepthTest    = DepthTest::ALWAYS;
	bool                            m_writeDepth          = false;


//--------------------------------------------------------------------------------------------------------------------------------------------
//			DIRECTX 11 OBJECTS
//--------------------------------------------------------------------------------------------------------------------------------------------

	ID3D11Device*                   m_device              = nullptr;
	ID3D11DeviceContext*            m_context             = nullptr;
	IDXGISwapChain*                 m_swapChain           = nullptr;
	ID3D11RasterizerState*          m_rasterState         = nullptr;
	ID3D11DepthStencilState*        m_depthStencilState   = nullptr;
								    
	ID3D11BlendState*               m_blendStates   [static_cast<int>(BlendMode::NUMSTATES)] = {};
	ID3D11SamplerState*             m_samplerStates [static_cast<int>(SamplerMode::COUNT)]   = {};
	ID3D11SamplerState*             m_depthBufferSamplerState [static_cast< int >( SamplerMode::COUNT )] = {};
};
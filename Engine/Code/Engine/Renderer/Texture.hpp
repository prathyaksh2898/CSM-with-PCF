#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/LightStructure.hpp"
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
class Renderer;
class Image;

#if defined(_DIRECTX11)

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

#endif


//--------------------------------------------------------------------------------------------------------------------------------------------
class Texture{

	friend class Renderer;

private:
	                          Texture(); 
	                          Texture( Texture const& copy ) = delete; 
	                          ~Texture();

//--------------------------------------------------------------------------------------------------------------------------------------------
//			Base Texture Functions
//--------------------------------------------------------------------------------------------------------------------------------------------

	bool                      LoadFromFile( Renderer* source, char const* imageFilePath );						 
	bool                      CreateFromImage( Renderer* source, char const* imageFilePath, Image const& image ); 

//--------------------------------------------------------------------------------------------------------------------------------------------
//			Render Target Functions
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	ID3D11RenderTargetView*   GetOrCreateRenderTargetView( Renderer* renderer );		
	ID3D11RenderTargetView*   GetOrCreateRenderTargetViewArray( Renderer* renderer, uint arrayNum = 1 );		

//--------------------------------------------------------------------------------------------------------------------------------------------
//			Depth Stencil and Depth Buffer Functions
//--------------------------------------------------------------------------------------------------------------------------------------------

	bool                      CreateDepthStencilTarget( Renderer* source, Image const& image );
	ID3D11DepthStencilView*   GetOrCreateDepthStencilView( Renderer* renderer );
	bool                      CreateDepthBufferTarget( Renderer* source, IntVec2 const& dimensions, uint arrayDimensions = 1 );
	ID3D11DepthStencilView*   GetOrCreateDepthStencilBufferView( Renderer* renderer, int sliceNum /*= 0*/ );
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			Shader Resource View
//--------------------------------------------------------------------------------------------------------------------------------------------

	ID3D11ShaderResourceView* GetOrCreateShaderResourceView( Renderer* renderer );	
	ID3D11ShaderResourceView* GetOrCreateShaderResourceViewDepthBuffer( Renderer* renderer );	
		
//--------------------------------------------------------------------------------------------------------------------------------------------
//			Cube Map
//--------------------------------------------------------------------------------------------------------------------------------------------

	bool                      CreateCubemapfromFile( Renderer* source, std::vector<std::string>& imagePaths );
	ID3D11ShaderResourceView* GetOrCreateShaderResourceViewTextureCube( Renderer* renderer );

public:

//--------------------------------------------------------------------------------------------------------------------------------------------
//			DX11 Functions
//--------------------------------------------------------------------------------------------------------------------------------------------

	void				      ReleaseResources(); 
	void				      WatchInternal( ID3D11Texture2D* handle ); 

//--------------------------------------------------------------------------------------------------------------------------------------------
//			Texture Info Getters
//--------------------------------------------------------------------------------------------------------------------------------------------
						      
	IntVec2				      GetDimensions() const { return m_dimensions; }
	std::string const&        GetImageFilePath() const { return m_imageFilePath; }
						      
	float				      GetAspect() const { return (float)m_dimensions.x / (float)m_dimensions.y; }
	bool                      IsValid() const;

public:
	std::string                 m_imageFilePath;
	IntVec2                     m_dimensions          = IntVec2(0,0);

	uint                        m_texArraySize        = 0;
	
	ID3D11Texture2D*            m_handle              = nullptr;
							    				      
	ID3D11RenderTargetView*     m_renderTargetView    = nullptr;
	ID3D11ShaderResourceView*   m_shaderResourceView  = nullptr;
	ID3D11ShaderResourceView*   m_shaderCubeView      = nullptr;
	ID3D11DepthStencilView*     m_depthStencilView    = nullptr;

	ID3D11DepthStencilView*     m_depthBufferViews[ NUM_CASCADES ] = {};
};
#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/LightStructure.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;


//-----------------------------------------------------------------------------------------------
struct LightCameraData
{
	Mat44 m_viewMatrix;
	Mat44 m_projectionMatrix[ NUM_CASCADES ];
	Vec4  m_cameraPosition;
};


//-----------------------------------------------------------------------------------------------
struct LightCamera : public Camera
{
private:
	LightDataC m_lightValues;

	Texture*   m_texture  = nullptr; 
	bool       m_shadowCasting = false;
	Mat44      m_projectionMatrix[ NUM_CASCADES ];

	uint       m_numCascades = 0;

public:

	LightCamera();
	~LightCamera();
	LightCamera( LightCamera const& copy ) = delete;

	void            SetLightValues( LightDataC data );
			        
	Mat44           GetViewMatrix() const override;
			        
	void            SetProjectionMatrix( Mat44 proj, int num = 0 );
			        
	Mat44           GetProjectionMatrix( int num = 0 ) const;
	Texture*        GetDepthTexture() const;
	
	LightCameraData GetLightCameraData() const;
	
	void            ToggleShadowCasting();
	bool            IsShadowCasting() const;
			        
	void            GetLightCameraAxes( Vec3& iBasis, Vec3& jBasis, Vec3& kBasis ) const;

	void            CreateDepthTextures( uint numCascades = 1 );
};


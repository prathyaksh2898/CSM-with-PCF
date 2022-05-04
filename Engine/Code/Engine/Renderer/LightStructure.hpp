#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"


//------------------------------------------------------------------------------------------------
typedef unsigned int uint;


//------------------------------------------------------------------------------------------------
constexpr uint NUM_CASCADES       = 8;
constexpr uint MAXLIGHTS          = 8;
constexpr uint INVALID_LIGHT      = 0;
constexpr uint DIRECTIONAL_LIGHT  = 1;
constexpr uint SPOT_LIGHT         = 2;
constexpr uint POINT_LIGHT        = 3;
constexpr uint DEPTH_TEXTURE_SIZE = 4096;


//------------------------------------------------------------------------------------------------
struct LightDataC
{
	Vec3         m_worldPosition               = Vec3::ZERO;
	uint         m_lightType                   = INVALID_LIGHT;
				 					           
	Vec3         m_color                       = Vec3::ONE;
	float        m_intensity                   = 0.0f;                               
				 					           
	Vec3         m_direction                   = Vec3( 0.0f, 0.0f, 0.0f );           
	float        m_directionFactor;                                  
				 					           
	Vec3         m_attenuation                 = Vec3( 0.0f, 1.0f, 0.0f );         
	float        m_dotInnerAngle               = -1.0f;                          
				 					           
	Vec3         m_specularAttenuation         = Vec3( 0.0f, 0.0f, 1.0f ); 
	float        m_dotOuterAngle               = -1.0f; 
									           
	Mat44        m_viewMat                     = Mat44();
	Mat44        m_projectionMat[NUM_CASCADES] = {};

	uint         m_isShadowCasting             = 0;
	float        m_ShadowBias                  = 0.002f;
									           
	float        padding0                      = 777.0f;
	float        padding1                      = 777.0f;
};


//------------------------------------------------------------------------------------------------
struct ShaderLightData
{
	Vec4		m_ambientLight;								
	LightDataC	m_lights[ MAXLIGHTS ];
	
	float		m_diffuseFactor  = 1;							
	float		m_specularFactor = 1;							
	float		m_specularPower  = 32;						
	float		padding00 = 0.f;
};


//------------------------------------------------------------------------------------------------
struct DepthForCascade
{
	float depthVal;
	float padding[3];
};


//------------------------------------------------------------------------------------------------
struct CascadeConstantsData
{
	DepthForCascade cascadedDepthValues[ NUM_CASCADES ];
	float           dimensionOfDepthTexture;
	uint            numSamples = 4;
	uint            numCascades = 4;
	uint            enablePCF = 1;
};

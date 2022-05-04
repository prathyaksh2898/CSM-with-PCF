#include "Game/GameCommon.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Lighting/LightCamera.hpp"
#include "Engine/Renderer/LightStructure.hpp"
#include "Engine/Window/Window.hpp"


//-----------------------------------------------------------------------------------------------
LightCamera::LightCamera()
{
	CreateDepthTextures( 1 );
}


//-----------------------------------------------------------------------------------------------
LightCamera::~LightCamera()
{
	g_theRenderer->DeleteTexture( m_texture );
	m_texture = nullptr;
}


//-----------------------------------------------------------------------------------------------
void LightCamera::SetLightValues( LightDataC data )
{
	m_lightValues = data;
}


//-----------------------------------------------------------------------------------------------
Mat44 LightCamera::GetViewMatrix() const
{
	Mat44 viewMatrix;
	if ( m_lightValues.m_lightType != DIRECTIONAL_LIGHT )
	{
		viewMatrix.AppendTranslation3D( m_lightValues.m_worldPosition );
	}

	Vec3 iBasis;
	Vec3 jBasis;
	Vec3 kBasis;
	GetLightCameraAxes( iBasis, jBasis, kBasis );

	Mat44 direcMat( iBasis, jBasis, kBasis, Vec3::ZERO );
	viewMatrix.Append( direcMat );

	viewMatrix = viewMatrix.GetOrthonormalInverse();

	return viewMatrix;
}


//------------------------------------------------------------------------------------------------
void LightCamera::SetProjectionMatrix( Mat44 proj, int projNum /*= 0*/ ) 
{
	m_projectionMatrix[ projNum ] = proj;
}


//------------------------------------------------------------------------------------------------
Mat44 LightCamera::GetProjectionMatrix( int projnum /*= 0 */ ) const
{
	return m_projectionMatrix[ projnum ];
}


//-----------------------------------------------------------------------------------------------
Texture* LightCamera::GetDepthTexture() const
{
	return m_texture;
}


//------------------------------------------------------------------------------------------------
LightCameraData LightCamera::GetLightCameraData() const
{
	LightCameraData data;
	//Mat44 renderMatrix = camera.GetRenderMatrix().GetOrthonormalInverse();
	Mat44 renderMatrix = GetRenderMatrix();
	renderMatrix.Append( GetViewMatrix() );
	data.m_viewMatrix = renderMatrix;
	
	for ( int cascadeNum = 0; cascadeNum < NUM_CASCADES; cascadeNum++ )
	{
		data.m_projectionMatrix[ cascadeNum ] = m_projectionMatrix[ cascadeNum ];
	}
	
	data.m_cameraPosition = GetPosition();

	return data;
}


//-----------------------------------------------------------------------------------------------
void LightCamera::ToggleShadowCasting()
{
	m_shadowCasting = !m_shadowCasting;
}


//-----------------------------------------------------------------------------------------------
bool LightCamera::IsShadowCasting() const
{
	return m_shadowCasting;
}


//-----------------------------------------------------------------------------------------------
void LightCamera::GetLightCameraAxes( Vec3& out_iBasis, Vec3& out_jBasis, Vec3& out_kBasis ) const
{
	out_iBasis = ( m_lightValues.m_direction );
	out_iBasis = out_iBasis.GetNormalized();

	out_jBasis = CrossProduct3D( Vec3( 0.0f, 0.0f, 1.0f ), out_iBasis );
	
	if ( out_jBasis.GetLength() == 0.0f )
	{
		out_kBasis = CrossProduct3D( out_iBasis, Vec3( 0.0f, 1.0f, 0.0f ) );
		out_jBasis = CrossProduct3D( out_kBasis, out_iBasis );
	}
	else
	{
		out_jBasis.Normalize();
		out_kBasis = CrossProduct3D( out_iBasis, out_jBasis );
	}
}


//------------------------------------------------------------------------------------------------
void LightCamera::CreateDepthTextures( uint numCascades /*= 1 */ )
{
	if ( m_texture != nullptr && m_numCascades == numCascades )
		return;

	if ( m_texture != nullptr )
	{ 
		g_theRenderer->DeleteTexture( m_texture );
		m_texture = nullptr;
	}

	m_numCascades = numCascades;

	IntVec2 dimensions;
	dimensions.x = DEPTH_TEXTURE_SIZE;
	dimensions.y = DEPTH_TEXTURE_SIZE;

	m_texture = g_theRenderer->CreateDepthBufferTexture( dimensions, numCascades );
}

#include "Game.hpp"

#include "Game/App.hpp"
#include "Game/Definitions/LightConfigurations.hpp"
#include "Game/Definitions/SceneSetting.hpp"
#include "Game/FBXSceneObject.hpp"
#include "Game/Player.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/3D/FBXLoader.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Lighting/LightCamera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/VertexData/VertexUtils.hpp"
#include "Engine/Telemetry/D3D11PerformanceMarker.hpp"
#include "Engine/Window/Window.hpp"

#include "ThirdParty/imgui/imgui.h"


//----------------------------------------------------------------------------------------------------
Game* g_theGame = nullptr;


//----------------------------------------------------------------------------------------------------
Game::Game( App* owner ) :m_App( owner )
{
}



//----------------------------------------------------------------------------------------------------
Game::~Game()
{
	
}


//----------------------------------------------------------------------------------------------------
void Game::Update()
{
	float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetFrameDeltaSeconds() );
	m_vertsRendered = 0;

	UpdateDebug();
	UpdateShaderLightDataUsingUI();
	UpdateLightRotation( deltaSeconds );
	UpdateEntities( deltaSeconds );
	UpdateCamera( deltaSeconds );

#if defined(ENGINE_DEBUG_RENDERING)

	//DebugAddWorldBasis( Mat44(), Rgba8::WHITE, Rgba8::WHITE, 0.f, DebugRenderMode::USE_DEPTH );
	DebugAddScreenText( Stringf( "Player Position: (%.2f, %.2f, %.2f)", m_player->m_position.x, m_player->m_position.y, m_player->m_position.z ), Vec2( 400.0f, 168.0f ), 0.0f, Vec2( 1.0f, 1.0f ), 8.0f );
	DebugAddScreenText( Stringf( "Active Scene name: %s", m_sceneSetting->m_name.c_str() ), Vec2( 400.0f, 200.0f ), 0.0f, Vec2( 1.0f, 1.0f ), 8.0f );
	DebugAddScreenText( Stringf( "Active Camera: %s", m_useCamera1 ? "Main camera" : "Debug Camera" ), Vec2( 400.0f, 192.0f ), 0.0f, Vec2( 1.0f, 1.0f ), 8.0f );
	DebugAddScreenText( Stringf( "FPS: %.3f", 1.0f / deltaSeconds ), Vec2( 400.0f, 184.0f ), 0.0f, Vec2( 1.0f, 1.0f ), 8.0f );

#endif

}


//----------------------------------------------------------------------------------------------------
void Game::UpdateDebug()
{
	if ( g_theInput->WasKeyJustPressed( KEYCODE_ESCAPE ) )
	{
		g_theApp->HandleQuitRequested();
	}

	if ( g_theInput->WasKeyJustPressed( KEYCODE_F3 ) )
	{
		m_ImGui = !m_ImGui;

		if ( m_ImGui )
		{
			g_theInput->PushMouseConfig( &m_ImGuiConfig );
		}
		else
		{
			g_theInput->PopMouseConfig( &m_ImGuiConfig );
		}
	}

	if ( m_ImGui )
	{
		g_theInput->ClearState();
	}

	if ( g_theInput->WasKeyJustPressed( 'P' ) )
	{
		m_useCamera1 = !m_useCamera1;
		if ( m_useCamera1 )
		{
			m_player->m_position    = m_worldCamera.GetPosition();
			m_player->m_orientation = m_worldCamera.GetOrientation();
		}
		else
		{
			m_player->m_position    = m_worldCamera2.GetPosition();
			m_player->m_orientation = m_worldCamera2.GetOrientation();
		}
	}
}


//------------------------------------------------------------------------------------------------
void Game::UpdateLightRotation( float deltaSeconds )
{
	float multiplier = 1.0f;
	if ( g_theInput->IsKeyDown( KEYCODE_PERIOD ) )
	{
		multiplier *= 10.0f;
	}

	if ( g_theInput->IsKeyDown( KEYCODE_COMMA ) )
	{
		multiplier *= 0.1f;
	}

	float rotationPerSec = 5.0f;
	float zLimit = -0.05f;

	Mat44 rotationMatrixPlusX  = Mat44::CreateXRotationDegrees( deltaSeconds * multiplier * rotationPerSec );
	Mat44 rotationMatrixPlusY  = Mat44::CreateYRotationDegrees( deltaSeconds * multiplier * rotationPerSec );	
	Mat44 rotationMatrixPlusZ  = Mat44::CreateZRotationDegrees( deltaSeconds * multiplier * rotationPerSec );
	Mat44 rotationMatrixMinusX = Mat44::CreateXRotationDegrees( deltaSeconds * multiplier * -rotationPerSec );
	Mat44 rotationMatrixMinusY = Mat44::CreateYRotationDegrees( deltaSeconds * multiplier * -rotationPerSec );
	Mat44 rotationMatrixMinusZ = Mat44::CreateZRotationDegrees( deltaSeconds * multiplier * -rotationPerSec );

	for ( uint8_t lightNum = 0; lightNum < MAXLIGHTS; lightNum++ )
	{
		if ( m_shaderLightData.m_lights[lightNum].m_lightType != DIRECTIONAL_LIGHT )
		{
			if ( m_lightMoving[lightNum] && m_lightMoveSpeed[lightNum] != 0.0f )
			{
				if ( m_lightStartPosition[lightNum] != m_lightEndPosition[lightNum] &&
					 m_lightStartPosition[lightNum] != Vec3( 777.0f, 777.0f, 777.0f ) &&
					 m_lightEndPosition[lightNum] != Vec3( 777.0f, 777.0f, 777.0f ) && 
					 m_lightMoveSpeed[lightNum] > 0.0f )
				{
					Vec3 direction = m_lightEndPosition[lightNum] - m_shaderLightData.m_lights[lightNum].m_worldPosition;
					Vec3 startToEnd = m_lightEndPosition[lightNum] - m_lightStartPosition[lightNum];

					if ( DotProduct3D( direction, startToEnd ) < 0.0f )
					{
						Vec3 temp = m_lightStartPosition[lightNum];
						m_lightStartPosition[lightNum] = m_lightEndPosition[lightNum];
						m_lightEndPosition[lightNum]   = temp;

						m_shaderLightData.m_lights[lightNum].m_worldPosition = m_lightStartPosition[lightNum];
					}

					direction = (m_lightEndPosition[lightNum] - m_lightStartPosition[lightNum]).GetNormalized();

					m_shaderLightData.m_lights[lightNum].m_worldPosition += direction * deltaSeconds * m_lightMoveSpeed[lightNum];
				}
			}
		}


		if ( m_shaderLightData.m_lights[lightNum].m_lightType == DIRECTIONAL_LIGHT &&
			 m_lightRotating[lightNum] )
		{
			switch ( m_lightRotationAxes[lightNum] )
			{
				case 0:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixPlusX.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );					
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					if ( m_shaderLightData.m_lights[lightNum].m_direction.z > zLimit )
					{
						m_shaderLightData.m_lights[lightNum].m_direction.z = zLimit;

						bool sign = ( m_shaderLightData.m_lights[lightNum].m_direction.y > 0 );
						float x = m_shaderLightData.m_lights[lightNum].m_direction.x;
						m_shaderLightData.m_lights[lightNum].m_direction.y = sqrtf( 1.0f - ( x * x ) );
						if ( sign )
						{
							m_shaderLightData.m_lights[lightNum].m_direction.y = -m_shaderLightData.m_lights[lightNum].m_direction.y;
						}
					}
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 1:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixMinusX.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					if ( m_shaderLightData.m_lights[lightNum].m_direction.z > zLimit )
					{
						m_shaderLightData.m_lights[lightNum].m_direction.z = zLimit;

						bool sign = ( m_shaderLightData.m_lights[lightNum].m_direction.y > 0 );
						float x = m_shaderLightData.m_lights[lightNum].m_direction.x;
						m_shaderLightData.m_lights[lightNum].m_direction.y = sqrtf( 1.0f - ( x * x ) );
						if ( sign )
						{
							m_shaderLightData.m_lights[lightNum].m_direction.y = -m_shaderLightData.m_lights[lightNum].m_direction.y;
						}
					}
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 2:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixPlusY.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					if ( m_shaderLightData.m_lights[lightNum].m_direction.z > zLimit )
					{
						m_shaderLightData.m_lights[lightNum].m_direction.z = zLimit;

						bool sign = ( m_shaderLightData.m_lights[lightNum].m_direction.x > 0 );
						float y = m_shaderLightData.m_lights[lightNum].m_direction.y;
						m_shaderLightData.m_lights[lightNum].m_direction.x = sqrtf( 1.0f - ( y * y ) );
						if ( sign )
						{
							m_shaderLightData.m_lights[lightNum].m_direction.x = -m_shaderLightData.m_lights[lightNum].m_direction.x;
						}
					}
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 3:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixMinusY.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					if ( m_shaderLightData.m_lights[lightNum].m_direction.z > zLimit )
					{
						m_shaderLightData.m_lights[lightNum].m_direction.z = zLimit;

						bool sign = ( m_shaderLightData.m_lights[lightNum].m_direction.x > 0 );
						float y = m_shaderLightData.m_lights[lightNum].m_direction.y;
						m_shaderLightData.m_lights[lightNum].m_direction.x = sqrtf( 1.0f - ( y * y ) );
						if ( sign )
						{
							m_shaderLightData.m_lights[lightNum].m_direction.x = -m_shaderLightData.m_lights[lightNum].m_direction.x;
						}
					}
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					break;
				}
				case 4:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixPlusZ.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					if ( m_shaderLightData.m_lights[lightNum].m_direction.z > zLimit )
					{
						m_shaderLightData.m_lights[lightNum].m_direction.z = zLimit;
					}
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					break;
				}
				case 5:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixMinusZ.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					if ( m_shaderLightData.m_lights[lightNum].m_direction.z > zLimit )
					{
						m_shaderLightData.m_lights[lightNum].m_direction.z = zLimit;
					}
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
					break;
				}
				default:
					break;
			}
		}
		else if ( m_shaderLightData.m_lights[lightNum].m_lightType == SPOT_LIGHT &&
			    m_lightRotating[lightNum] )
		{
			switch ( m_lightRotationAxes[lightNum] )
			{
				case 0:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixPlusX.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 1:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixMinusX.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 2:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixPlusY.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 3:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixMinusY.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 4:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixPlusZ.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
				case 5:
				{
					m_shaderLightData.m_lights[lightNum].m_direction = rotationMatrixMinusZ.TransformVectorQuantity3D( m_shaderLightData.m_lights[lightNum].m_direction );
					m_shaderLightData.m_lights[lightNum].m_direction.Normalize();
				}
				break;
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------
void Game::UpdateEntities( float deltaSeconds )
{
	m_player->Update( deltaSeconds );

	for ( FBXSceneObject* obj : m_sceneSetting->m_sceneObjects )
	{
		if ( obj != nullptr )
		{
			obj->Update(deltaSeconds);
		}
	}

	if ( !m_hideDefaultGeometry )
	{
		m_vertsRendered += static_cast< uint32_t >( m_floor.size() );
		m_vertsRendered += static_cast< uint32_t >( m_wall.size() );
		m_vertsRendered += static_cast< uint32_t >( m_cubeVerts1.size() ) * 2;
	}
}


//----------------------------------------------------------------------------------------------------
void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	for ( int cascadeNum = 0; cascadeNum < NUM_CASCADES; cascadeNum++ )
	{
		m_cascadeData.cascadedDepthValues[cascadeNum].depthVal = ( static_cast< float >( m_cascadeDepthPercent[cascadeNum] ) * 0.01f * ( m_farPlane - m_nearPlane ) ) + m_nearPlane;
	}
	m_cascadeData.numCascades = m_numCascades;
	m_cascadeData.enablePCF = m_enablePCF ? 1 : 0;
	m_cascadeDepthConstantBuffer->SetData( m_cascadeData );

	if ( m_useCamera1 )
	{
		m_worldCamera.SetCameraPositionAndOrientation( m_player->m_position, m_player->m_orientation );
	}
	else
	{
		m_worldCamera2.SetCameraPositionAndOrientation( m_player->m_position, m_player->m_orientation );
	}

	for ( int lightNum = 0; lightNum < m_numLights; lightNum++ )
	{
		if ( m_lightCamFollow[ lightNum ] )
		{
			m_shaderLightData.m_lights[ lightNum ].m_worldPosition = m_worldCamera.GetPosition();
		}
		if ( m_lightCamDir[ lightNum ] )
		{
			m_shaderLightData.m_lights[ lightNum ].m_direction = m_worldCamera.GetOrientation().GetVectorXFwd();
		}
	}

	for ( int lightNum = 0; lightNum < MAXLIGHTS; lightNum++ )
	{
		m_lightCameraArray[lightNum]->SetLightValues( m_shaderLightData.m_lights[lightNum] );

		if ( m_shaderLightData.m_lights[lightNum].m_lightType == INVALID_LIGHT )
		{
			m_lightCameraArray[lightNum]->CreateDepthTextures( 1 );
			continue;
	    }
		else if ( m_shaderLightData.m_lights[lightNum].m_lightType == DIRECTIONAL_LIGHT )
		{
			m_lightCameraArray[lightNum]->CreateDepthTextures( m_numCascades );
			m_lightCameraArray[lightNum]->SetCameraType( CameraType::ORTHOGRAPHIC );
			m_lightCameraArray[lightNum]->SetGameSpace( Vec3( 0.0f, 1.0f, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ), Vec3( 1.0f, 0.0f, 0.0f ) );
			UpdateLightCameraProjection( lightNum );
		}
		else if ( m_shaderLightData.m_lights[lightNum].m_lightType == SPOT_LIGHT )
		{
			m_lightCameraArray[lightNum]->CreateDepthTextures( 1 );
			m_lightCameraArray[lightNum]->SetCameraType( CameraType::PERSPECTIVE );
			m_lightCameraArray[lightNum]->SetGameSpace( Vec3( 0.0f, -1.0f, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ), Vec3( 1.0f, 0.0f, 0.0f ) );
			m_lightCameraArray[lightNum]->SetFieldOfView( 2.0f * ConvertRadiansToDegrees( acosf( m_shaderLightData.m_lights[lightNum].m_dotOuterAngle ) ) );
			m_lightCameraArray[lightNum]->SetZNearZFar( 0.1f, 20.0f );
			m_lightCameraArray[lightNum]->SetAspect( 2.0f );

			Mat44 projMatrix = Mat44::CreatePerspectiveProjection( m_lightCameraArray[lightNum]->GetFOV(), m_lightCameraArray[lightNum]->GetAspect(), m_lightCameraArray[lightNum]->GetZNear(), m_lightCameraArray[lightNum]->GetZFar() );
			m_lightCameraArray[lightNum]->SetProjectionMatrix( projMatrix, 0 );
			
			Vec3 lightStart = m_shaderLightData.m_lights[lightNum].m_worldPosition;
			Vec3 lightEndArrow = m_shaderLightData.m_lights[lightNum].m_worldPosition + m_shaderLightData.m_lights[lightNum].m_direction * 0.25f;
			DebugAddWorldArrow( lightStart, lightEndArrow );
		}
		else if ( m_shaderLightData.m_lights[lightNum].m_lightType == POINT_LIGHT )
		{
			m_lightCameraArray[lightNum]->CreateDepthTextures( 1 );
			DebugAddWorldSphere( m_shaderLightData.m_lights[lightNum].m_worldPosition, 0.1f, 0.0f, Rgba8::WHITE );
		}
		
		for ( int projNum = 0; projNum < NUM_CASCADES; projNum++ )
		{
			m_shaderLightData.m_lights[ lightNum ].m_projectionMat[projNum] = m_lightCameraArray[ lightNum ]->GetProjectionMatrix( projNum );
		}
		Mat44 lightSpaceViewMatrix = m_lightCameraArray[ lightNum ]->GetRenderMatrix();
		lightSpaceViewMatrix.Append( m_lightCameraArray[ lightNum ]->GetViewMatrix() );

		m_shaderLightData.m_lights[ lightNum ].m_viewMat = lightSpaceViewMatrix;
		m_lightCameraArray[ lightNum ]->SetLightValues( m_shaderLightData.m_lights[ lightNum ] );
	}

	CameraConstantsForCamera1 data;
	Mat44 renderMat = m_worldCamera.GetRenderMatrix();
	renderMat.Append( m_worldCamera.GetViewMatrix() );
	data.m_projectionMatrix = m_worldCamera.GetProjectionMatrix();
	data.m_viewMatrix = renderMat;
	m_cam1ConstantBuffer->SetData( data );
}


//------------------------------------------------------------------------------------------------
void Game::UpdateLightCameraProjection( int lightNum )
{
	DebugAddWorldArrow( Vec3( 0.0f, 0.0f, 2.0f ), Vec3( 0.0f, 0.0f, 2.0f ) + m_shaderLightData.m_lights[ lightNum ].m_direction );;

	Mat44 worldToLightViewMat = m_lightCameraArray[ lightNum ]->GetViewMatrix();

	Vec3 lightDirection = m_shaderLightData.m_lights[lightNum].m_direction;
	Vec3 camPos = m_worldCamera.GetPosition();
	Vec3 camForward;
	Vec3 camLeft;
	Vec3 camUp;
	m_worldCamera.GetOrientation().GetAsVectors_XFwd_YLeft_ZUp( camForward, camLeft, camUp );

	float camNear = m_worldCamera.GetZNear();
	float camFar = m_worldCamera.GetZFar();
	float camAspect = m_worldCamera.GetAspect();
	float yfov = m_worldCamera.GetFOV();
	float xfov = yfov * camAspect;

	int numCascades = m_numCascades;
	for ( int projNum = 0; projNum < numCascades; projNum++ )
	{
		//Custom cascade depth values
		float zNear = camNear;
		float zFar  = camFar;

		if ( projNum != 0 )
		{
			zNear = m_cascadeData.cascadedDepthValues[projNum - 1].depthVal;
		}
		zFar = m_cascadeData.cascadedDepthValues[projNum].depthVal;

		float zCenter = ( zFar - zNear )  * 0.5f + zNear;

		float xNear = zNear * SinDegrees( xfov * 0.5f ) / CosDegrees( xfov * 0.5f );
		float xFar  = zFar  * SinDegrees( xfov * 0.5f ) / CosDegrees( xfov * 0.5f );

		float yNear = zNear * SinDegrees( yfov * 0.5f ) / CosDegrees( yfov * 0.5f );
		float yFar  = zFar  * SinDegrees( yfov * 0.5f ) / CosDegrees( yfov * 0.5f );
		
		//Frustum Bounds in GameSpace
		Vec3 world_bottomLeftNear  = camPos + (zNear * camForward) + (xNear * camLeft) - (yNear * camUp);
		Vec3 world_bottomRightNear = camPos + (zNear * camForward) - (xNear * camLeft) - (yNear * camUp);
		Vec3 world_topLeftNear     = camPos + (zNear * camForward) + (xNear * camLeft) + (yNear * camUp);
		Vec3 world_topRightNear    = camPos + (zNear * camForward) - (xNear * camLeft) + (yNear * camUp);

		Vec3 world_bottomLeftFar   = camPos + (zFar * camForward) + (xFar * camLeft) - (yFar * camUp);
		Vec3 world_bottomRightFar  = camPos + (zFar * camForward) - (xFar * camLeft) - (yFar * camUp);
		Vec3 world_topLeftFar      = camPos + (zFar * camForward) + (xFar * camLeft) + (yFar * camUp);
		Vec3 world_topRightFar     = camPos + (zFar * camForward) - (xFar * camLeft) + (yFar * camUp);
		
		//Save off untransformed camera frustum points to debug render later
		Vec3 world_frustumPoints[8];
		world_frustumPoints[ 0 ] = world_bottomLeftNear;
		world_frustumPoints[ 1 ] = world_bottomRightNear;
		world_frustumPoints[ 2 ] = world_topLeftNear;
		world_frustumPoints[ 3 ] = world_topRightNear;

		world_frustumPoints[ 4 ] = world_bottomLeftFar; 
		world_frustumPoints[ 5 ] = world_bottomRightFar;
		world_frustumPoints[ 6 ] = world_topLeftFar;
		world_frustumPoints[ 7 ] = world_topRightFar;   

		//z-biased center
		Vec3 center = camPos + (zCenter * camForward) ;

		AABB3 world_aabb( center, center );
		world_aabb.StretchToIncludePoint( world_frustumPoints[0] );
		world_aabb.StretchToIncludePoint( world_frustumPoints[1] );
		world_aabb.StretchToIncludePoint( world_frustumPoints[2] );
		world_aabb.StretchToIncludePoint( world_frustumPoints[3] );
		
		world_aabb.StretchToIncludePoint( world_frustumPoints[4] );
		world_aabb.StretchToIncludePoint( world_frustumPoints[5] );
		world_aabb.StretchToIncludePoint( world_frustumPoints[6] );
		world_aabb.StretchToIncludePoint( world_frustumPoints[7] );
		
		world_aabb.SetDimensions( world_aabb.GetDimensions() + Vec3( 18.0f, 18.0f, 18.0f ) );
		
		Vec3 worldDimensions = world_aabb.GetDimensions();
		
		//Transforming the entire frustum into light space
		Vec3 light_center = worldToLightViewMat.TransformPosition3D( center );
			 
	/*	Vec3 light_bottomLeftNear  = worldToLightViewMat.TransformPosition3D( world_bottomLeftNear );
		Vec3 light_bottomRightNear = worldToLightViewMat.TransformPosition3D( world_bottomRightNear );
		Vec3 light_topLeftNear     = worldToLightViewMat.TransformPosition3D( world_topLeftNear );
		Vec3 light_topRightNear    = worldToLightViewMat.TransformPosition3D( world_topRightNear );
			 
		Vec3 light_bottomLeftFar  = worldToLightViewMat.TransformPosition3D( world_bottomLeftFar );
		Vec3 light_bottomRightFar = worldToLightViewMat.TransformPosition3D( world_bottomRightFar );
		Vec3 light_topLeftFar     = worldToLightViewMat.TransformPosition3D( world_topLeftFar );
		Vec3 light_topRightFar    = worldToLightViewMat.TransformPosition3D( world_topRightFar ); */

		//Vec3 light_worldAABBmins = worldToLightViewMat.TransformPosition3D( world_aabb.m_mins );
		//Vec3 light_worldAABBmaxs = worldToLightViewMat.TransformPosition3D( world_aabb.m_maxs );

		Vec3 light_worldAABBVertex1 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_maxs.x, world_aabb.m_mins.y, world_aabb.m_mins.z ) );
		Vec3 light_worldAABBVertex2 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_maxs.x, world_aabb.m_maxs.y, world_aabb.m_mins.z ) );
		Vec3 light_worldAABBVertex3 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_maxs.x, world_aabb.m_mins.y, world_aabb.m_maxs.z ) );
		Vec3 light_worldAABBVertex4 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_maxs.x, world_aabb.m_maxs.y, world_aabb.m_maxs.z ) );

		Vec3 light_worldAABBVertex5 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_mins.x, world_aabb.m_mins.y, world_aabb.m_mins.z ) );
		Vec3 light_worldAABBVertex6 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_mins.x, world_aabb.m_maxs.y, world_aabb.m_mins.z ) );
		Vec3 light_worldAABBVertex7 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_mins.x, world_aabb.m_mins.y, world_aabb.m_maxs.z ) );
		Vec3 light_worldAABBVertex8 = worldToLightViewMat.TransformPosition3D( Vec3( world_aabb.m_mins.x, world_aabb.m_maxs.y, world_aabb.m_maxs.z ) );

		AABB3 light_aabb( light_center, light_center );
		light_aabb.StretchToIncludePoint( light_worldAABBVertex1 );
		light_aabb.StretchToIncludePoint( light_worldAABBVertex2 );
		light_aabb.StretchToIncludePoint( light_worldAABBVertex3 );
		light_aabb.StretchToIncludePoint( light_worldAABBVertex4 );
		
		light_aabb.StretchToIncludePoint( light_worldAABBVertex5 );
		light_aabb.StretchToIncludePoint( light_worldAABBVertex6 );
		light_aabb.StretchToIncludePoint( light_worldAABBVertex7 );
		light_aabb.StretchToIncludePoint( light_worldAABBVertex8 );
		
		//light_aabb.StretchToIncludePoint( light_bottomLeftNear );
		//light_aabb.StretchToIncludePoint( light_bottomRightNear );
		//light_aabb.StretchToIncludePoint( light_topLeftNear );
		//light_aabb.StretchToIncludePoint( light_topRightNear );
		//
		//light_aabb.StretchToIncludePoint( light_bottomLeftFar );
		//light_aabb.StretchToIncludePoint( light_bottomRightFar );
		//light_aabb.StretchToIncludePoint( light_topLeftFar );
		//light_aabb.StretchToIncludePoint( light_topRightFar );
		
		//light_aabb.SetDimensions( light_aabb.GetDimensions() + Vec3( 12.0f, 12.0f, 12.0f ) );

		light_aabb.MakeAABBCube();
		Vec3 dimensions = light_aabb.GetDimensions();
		float worldUnitsPerTexel = dimensions.x / ( DEPTH_TEXTURE_SIZE );
		
		light_aabb.m_mins  /= worldUnitsPerTexel;
		light_aabb.m_mins.x = floorf( light_aabb.m_mins.x );
		light_aabb.m_mins.y = floorf( light_aabb.m_mins.y );
		light_aabb.m_mins.z = floorf( light_aabb.m_mins.z );
		light_aabb.m_mins  *= worldUnitsPerTexel;
		
		light_aabb.m_maxs /= worldUnitsPerTexel;
		light_aabb.m_maxs.x = floorf( light_aabb.m_maxs.x );
		light_aabb.m_maxs.y = floorf( light_aabb.m_maxs.y );
		light_aabb.m_maxs.z = floorf( light_aabb.m_maxs.z );
		light_aabb.m_maxs *= worldUnitsPerTexel;

		Vec3 dims = light_aabb.GetDimensions();
		if ( !m_debugSpecCascades || (m_specCascadeNum == projNum) )
		{ 
			if ( m_debugSpecificLight && ( m_debugLightNumber == lightNum ) )
			{
				if ( m_debugFrustum )
				{
					DebugAddFrustum( world_frustumPoints );
				}
				if ( m_debugCascades )
				{
					Mat44 lightToWorldMatrix = worldToLightViewMat.GetOrthonormalInverse();
					DebugAddWorldWireAABB3( light_aabb, lightToWorldMatrix, 0.0f, Rgba8::RED );
					DebugAddWorldWireAABB3( world_aabb, Mat44(), 0.0f, Rgba8::GREEN );
				}
			}
		}
	    Mat44 projMatrix = Mat44::CreateOrthoProjection(light_aabb.m_maxs.y, light_aabb.m_mins.y, light_aabb.m_maxs.z, light_aabb.m_mins.z, light_aabb.m_mins.x, light_aabb.m_maxs.x );
		
		m_lightCameraArray[ lightNum ]->SetProjectionMatrix( projMatrix, projNum );
		g_theRenderer->ClearDepthBuffer( m_lightCameraArray[ lightNum ]->GetDepthTexture(), 1.0f, projNum );
	}
}


//------------------------------------------------------------------------------------------------
void Game::AddVertsRendered( uint32_t vertsAdded )
{
	m_vertsRendered += vertsAdded;
}


//----------------------------------------------------------------------------------------------------
void Game::Render() const
{
	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 255 ) );

	RenderForDepthBuffers();

	g_theRenderer->ClearScreen( Rgba8( 0, 0, 0, 255 ) );

	if ( m_useCamera1 )
	{
		g_theRenderer->BeginCamera( m_worldCamera );
		{
			RenderEntities();
			DebugRenderWorldToCamera( m_worldCamera );
		}
		g_theRenderer->EndCamera( m_worldCamera );
	}
	else
	{
		g_theRenderer->BeginCamera( m_worldCamera2 );
		{
			RenderEntities();
			DebugRenderWorldToCamera( m_worldCamera2 );
		}
		g_theRenderer->EndCamera( m_worldCamera2 );
	}

	DebugAddScreenText( Stringf( "Vertices Rendered: %d", m_vertsRendered ), Vec2( 400.0f, 176.0f ), 0.0f, Vec2( 1.0f, 1.0f ), 8.0f );

	g_theRenderer->BeginCamera( m_screenCamera );
	{
		RenderUI();

#if defined(ENGINE_DEBUG_RENDERING)

		DebugRenderScreenToCamera( m_screenCamera );

#endif
	}
	g_theRenderer->EndCamera( m_screenCamera );
}


//----------------------------------------------------------------------------------------------------
void Game::RenderForDepthBuffers() const
{
	RasterState state;
	state.m_cullmode = CullMode::NONE;
	state.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;
	g_theRenderer->SetRasterState( state );

	for ( int lightCamNum = 0; lightCamNum < MAXLIGHTS; lightCamNum++ )
	{
		ZoneScopedD3D11Marker d3dzone( Stringf( "Light Pass - %d", lightCamNum ).c_str() );

		int numCascades = m_numCascades;

		if ( m_shaderLightData.m_lights[lightCamNum].m_lightType == INVALID_LIGHT )
			continue;

		if ( m_shaderLightData.m_lights[lightCamNum].m_isShadowCasting == 0 )
			continue;

		if ( m_shaderLightData.m_lights[lightCamNum].m_lightType != DIRECTIONAL_LIGHT )
			numCascades = 1;

		for ( int cascadeNum = 0; cascadeNum < numCascades; cascadeNum++ )
		{ 
			ZoneScopedD3D11Marker* d3dCascadeZone = nullptr;
			if ( m_shaderLightData.m_lights[lightCamNum].m_lightType == DIRECTIONAL_LIGHT )
				d3dCascadeZone = new ZoneScopedD3D11Marker( Stringf( "Cascade - %d", cascadeNum ).c_str() );

			g_theRenderer->BeginCamera( *m_lightCameraArray[ lightCamNum ], cascadeNum );
			{
				g_theRenderer->SetDepthOptions( DepthTest::LESS_EQUAL, true );

				Shader* shader = g_theRenderer->CreateOrGetShaderFromFile( "Data/Shaders/LightDepthBuffer" );
				g_theRenderer->BindShader( shader );

				if ( !m_hideDefaultGeometry )
				{
					ModelTransformationData data;
					data.modelMatrix = m_cube1transform;
					Rgba8::WHITE.GetAsFloats( data.tint );
					g_theRenderer->SetModelBuffer( data );
					g_theRenderer->DrawVertexArray( static_cast< int >( m_cubeVerts1.size() ), m_cubeVerts1.data() );

					for ( int cubeNum = 0; cubeNum < 1; cubeNum++ )
					{
						ModelTransformationData cubeTransformData;
						cubeTransformData.modelMatrix = m_cubeTransforms[cubeNum];
						Rgba8::WHITE.GetAsFloats( cubeTransformData.tint );
						g_theRenderer->SetModelBuffer( cubeTransformData );
						g_theRenderer->DrawVertexArray( static_cast< int >( m_cubeVerts1.size() ), m_cubeVerts1.data() );
					}
				}

				for ( FBXSceneObject const* obj : m_sceneSetting->m_sceneObjects )
				{
					if ( obj != nullptr )
					{
						obj->Render();
					}
				}

				ModelTransformationData data1;
				Rgba8::WHITE.GetAsFloats( data1.tint );
				g_theRenderer->SetModelBuffer( data1 );
				if ( !m_hideDefaultGeometry )
				{
					g_theRenderer->DrawVertexArray( static_cast< int >( m_floor.size() ), m_floor.data() );
					g_theRenderer->DrawVertexArray( static_cast< int >( m_wall.size() ), m_wall.data() );
				}
				if ( d3dCascadeZone != nullptr )
				{
					delete d3dCascadeZone;
					d3dCascadeZone = nullptr;
				}
			}
			g_theRenderer->EndCamera( *m_lightCameraArray[ lightCamNum ] );
		}
	}
}


//----------------------------------------------------------------------------------------------------
void Game::RenderEntities() const
{
	ZoneScopedD3D11Marker mark( __FUNCTION__ );
	{
		ZoneScopedD3D11Marker skyBoxMark( "Skybox render pass");

		RasterState skyBoxState;
		skyBoxState.m_cullmode = CullMode::NONE;
		skyBoxState.m_fillMode = FillMode::SOLID;
		skyBoxState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;
		g_theRenderer->SetRasterState( skyBoxState );
		g_theRenderer->SetDepthOptions( DepthTest::ALWAYS, false );

		Shader* skyBoxShader = g_theRenderer->CreateOrGetShaderFromFile( "Data/Shaders/Skybox" );
		g_theRenderer->BindShader( skyBoxShader );
		g_theRenderer->BindCubeTexture( m_skybox );
		g_theRenderer->DrawVertexArray( static_cast< int >( m_skyBoxVerts.size() ), m_skyBoxVerts.data() );
	}

	RasterState state;
	state.m_cullmode = CullMode::BACK;
	state.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;
	g_theRenderer->SetRasterState( state );
	g_theRenderer->SetDepthOptions( DepthTest::LESS_EQUAL, true );

	Shader* shader = g_theRenderer->CreateOrGetShaderFromFile( "Data/Shaders/BlinnPhong" );
	Texture* boxTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Textures/tile_diffuse.png" );
	Texture* boxNormalTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Textures/tile_normal.png" );
	g_theRenderer->BindShader( shader );
	g_theRenderer->BindTexture( boxTexture );
	g_theRenderer->BindTexture( boxNormalTexture, 1 );

	for ( int lightCamNum = 0; lightCamNum < MAXLIGHTS; lightCamNum++ )
	{
		g_theRenderer->BindDepthTexture( m_lightCameraArray[ lightCamNum ]->GetDepthTexture(), 8 + lightCamNum );
	}

	g_theRenderer->SetLightBuffer( m_shaderLightData );
	g_theRenderer->BindConstantBuffer( 5, m_cascadeDepthConstantBuffer );
	g_theRenderer->BindConstantBuffer( 6, m_cam1ConstantBuffer );

	if ( !m_hideDefaultGeometry )
	{
		ModelTransformationData data;
		data.modelMatrix = m_cube1transform;
		Rgba8::WHITE.GetAsFloats( data.tint );
		g_theRenderer->SetModelBuffer( data );
		g_theRenderer->DrawVertexBuffer( m_cubeBuffer, static_cast< int >( m_cubeVerts1.size() ) );

		for ( int cubeNum = 0; cubeNum < 1; cubeNum++ )
		{
			ModelTransformationData cubeTransformData;
			cubeTransformData.modelMatrix = m_cubeTransforms[cubeNum];
			Rgba8::WHITE.GetAsFloats( cubeTransformData.tint );
			g_theRenderer->SetModelBuffer( cubeTransformData );
			g_theRenderer->DrawVertexBuffer( m_cubeBuffer, static_cast< int >( m_cubeVerts1.size() ) );
		}
	}
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->BindTexture( nullptr, 1 );

	ModelTransformationData data1;
	Rgba8::WHITE.GetAsFloats( data1.tint );
	g_theRenderer->SetModelBuffer( data1 );

	if ( !m_hideDefaultGeometry )
	{
		g_theRenderer->DrawVertexArray( static_cast< int >( m_floor.size() ), m_floor.data() );
		g_theRenderer->DrawVertexArray( static_cast< int >( m_wall.size() ), m_wall.data() );
	}

	state.m_cullmode = CullMode::NONE;
	state.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;
	g_theRenderer->SetRasterState( state );

	Shader* modelShader = g_theRenderer->CreateOrGetShaderFromFile( "Data/Shaders/BlinnPhongModels" );
	g_theRenderer->BindShader( modelShader );
	
	{
		ZoneScopedD3D11Marker fbxObjectMark( "FBX render pass" );
		for ( FBXSceneObject const* obj : m_sceneSetting->m_sceneObjects )
		{
			if ( obj != nullptr )
			{
				obj->Render();
			}
		}
	}

	for ( int lightCamNum = 0; lightCamNum < MAXLIGHTS; lightCamNum++ )
	{
		g_theRenderer->ClearTextureAtSlot( 8 + lightCamNum );
	}
}


//----------------------------------------------------------------------------------------------------
void Game::RenderUI() const
{
	if ( m_debugDepthBuffer )
	{
		RasterState state;
		state.m_cullmode = CullMode::BACK;
		state.m_fillMode = FillMode::SOLID;
		state.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;
		g_theRenderer->SetRasterState( state );

		Shader* shader = g_theRenderer->CreateOrGetShaderFromFile( "Data/Shaders/DepthBufferRender" );
		g_theRenderer->BindShader( shader );

		if ( m_debugAllDepthBuffers && m_shaderLightData.m_lights[m_debugLightNumber].m_lightType == DIRECTIONAL_LIGHT )
		{
			for ( int cascadeNum = 0; cascadeNum < m_numCascades; cascadeNum++ )
			{
				DebugCascadePrint data;
				data.debugCascadeLayer = static_cast< float >( cascadeNum );

				m_debugPrintConstantBuffer->SetData( data );
				g_theRenderer->BindConstantBuffer( 4, m_debugPrintConstantBuffer );

				std::vector<Vertex_PCU> verts;
				AddVertsForAABB2D( verts, AABB2( -400.0f + ( cascadeNum * 100.0f ), -200.0f, -300.0f + ( ( cascadeNum ) * 100.0f ), -100.0f), Rgba8::WHITE );
				g_theRenderer->BindDepthTexture( m_lightCameraArray[m_debugLightNumber]->GetDepthTexture(), 0 );
				g_theRenderer->DrawVertexArray( static_cast< int >( verts.size() ), verts.data() );
			}
		}
		else
		{
			DebugCascadePrint data;
			data.debugCascadeLayer = static_cast< float >( m_debugCascadeNum );

			m_debugPrintConstantBuffer->SetData( data );
			g_theRenderer->BindConstantBuffer( 4, m_debugPrintConstantBuffer );

			std::vector<Vertex_PCU> verts;
			AddVertsForAABB2D( verts, AABB2( -400.0f, -200.0, -300.0f, -100.0f ), Rgba8::WHITE );
			g_theRenderer->BindDepthTexture( m_lightCameraArray[ m_debugLightNumber ]->GetDepthTexture(), 0 );
			g_theRenderer->DrawVertexArray( static_cast< int >(verts.size()), verts.data() );
		}
	}

	AABB2 renderConsoleBox = AABB2( m_screenCamera.GetOrthoBottomLeft().x, m_screenCamera.GetOrthoBottomLeft().y, m_screenCamera.GetOrthoTopRight().x, m_screenCamera.GetOrthoTopRight().y );
	g_theConsole->Render( renderConsoleBox );
}


//----------------------------------------------------------------------------------------------------
void Game::ResetEntities()
{
}


//------------------------------------------------------------------------------------------------
void Game::ReloadXMLData()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile( "Data/XML/LightConfigurations.xml" );
	GUARANTEE_OR_DIE( doc.ErrorID() == tinyxml2::XML_SUCCESS, "Error Opening the Light Config XML Document" );

	tinyxml2::XMLElement* root = doc.RootElement();
	GUARANTEE_OR_DIE( _strcmpi( root->Name(), "LightConfigurations" ) == 0, "Root name different from LightConfigurations" );

	tinyxml2::XMLElement* childOfRoot = root->FirstChildElement();

	LightConfiguration::s_lightDefs.clear();
	while ( childOfRoot )
	{
		LightConfiguration* config = new LightConfiguration();
		config->LoadFromXmlElement( *childOfRoot );

		childOfRoot = childOfRoot->NextSiblingElement();
	}

	doc.LoadFile( "Data/XML/SceneSEtting.xml" );
	GUARANTEE_OR_DIE( doc.ErrorID() == tinyxml2::XML_SUCCESS, "Error Opening the Scene Setting XML Document" );

	root = doc.RootElement();
	GUARANTEE_OR_DIE( _strcmpi( root->Name(), "SceneSettings" ) == 0, "Root name different from SceneSettings" );

	childOfRoot = root->FirstChildElement();

	SceneSetting::s_sceneDefs.clear();
	while ( childOfRoot )
	{
		SceneSetting* config = new SceneSetting();
		config->LoadFromXmlElement( *childOfRoot );

		childOfRoot = childOfRoot->NextSiblingElement();
	}

	ASSERT_OR_DIE( SceneSetting::s_sceneDefs.size() > 0, "NO SCENE CONFIGS FOUND!!!" );

	LoadScene( m_activeGameScene );
}

//------------------------------------------------------------------------------------------------
void Game::LoadNextScene()
{
	m_activeGameScene++;
	if ( m_activeGameScene >= static_cast< int >( SceneSetting::s_sceneDefs.size() ) )
	{
		m_activeGameScene = 0;
	}

	LoadScene( m_activeGameScene );
}


//------------------------------------------------------------------------------------------------
void Game::LoadPreviousScene()
{
	m_activeGameScene--;
	if ( m_activeGameScene < 0 )
	{
		m_activeGameScene = static_cast< uint >( SceneSetting::s_sceneDefs.size() - 1 );
	}

	LoadScene( m_activeGameScene );
}


//------------------------------------------------------------------------------------------------
void Game::LoadScene( uint sceneNum )
{
	if ( sceneNum >= static_cast< uint >( SceneSetting::s_sceneDefs.size() ) ||
		 sceneNum < 0 )
	{
		sceneNum = 0;
	}

	for ( int lightNum = 0; lightNum < MAXLIGHTS; lightNum++ )
	{
		m_lightCamFollow[ lightNum ] = false;
		m_lightCamDir[ lightNum ] = false;
	}

	SceneSetting* setting = SceneSetting::s_sceneDefs[ sceneNum ];
	m_shaderLightData = setting->m_lightConfig->m_shaderData;
	m_numLights = setting->m_lightConfig->m_numLights;
	m_sceneSetting = setting;

	m_numCascades = setting->m_lightConfig->m_numCascades;
	
	for ( int cascadeNum = 0; cascadeNum < NUM_CASCADES; cascadeNum++ )
	{
		m_cascadeDepthPercent[cascadeNum] = setting->m_lightConfig->m_cascadePercentages[cascadeNum];
	}

	for ( int lightNum = 0; lightNum < MAXLIGHTS; lightNum++ )
	{
		m_lightRotating[lightNum] = ( setting->m_lightConfig->m_lightRotating[lightNum] == 1 );
		m_lightRotationAxes[lightNum] = ( setting->m_lightConfig->m_lightRotatingAxis[lightNum] );

		m_lightMoving[lightNum] = setting->m_lightConfig->m_lightMoving[lightNum];
		m_lightMoveSpeed[lightNum] = setting->m_lightConfig->m_lightMoveSpeed[lightNum];
		m_lightStartPosition[lightNum] = setting->m_lightConfig->m_lightStartPosition[lightNum];
		m_lightEndPosition[lightNum] = setting->m_lightConfig->m_lightEndPosition[lightNum];
	}

	m_debugSpecificLight   = setting->m_debugSpecificLight;  
	m_debugFrustum         = setting->m_debugFrustum;        
	m_debugCascades        = setting->m_debugCascades;       
	m_debugSpecCascades    = setting->m_debugSpecCascades;   
	m_debugDepthBuffer     = setting->m_debugDepthBuffer;    
	m_debugAllDepthBuffers = setting->m_debugAllDepthBuffers;
	m_debugCascadeNum      = setting->m_debugCascadeNum;     
	m_specCascadeNum       = setting->m_specCascadeNum;      

	m_useCamera1 = setting->m_useCamera1;
	m_enablePCF = setting->m_enablePCF;
	m_hideDefaultGeometry = setting->m_hideDefaultGeometry;

	if ( m_useCamera1 )
	{
		m_player->m_orientation = setting->m_cam1Orientation;
		m_player->m_position = setting->m_cam1Position;
	}
	else
	{
		m_player->m_orientation = setting->m_cam2Orientation;
		m_player->m_position = setting->m_cam2Position;
	}

	m_nearPlane = setting->m_nearPlane;
	m_farPlane = setting->m_farPlane;
	m_worldCamera.SetZNearZFar( setting->m_nearPlane, setting->m_farPlane );
	
	m_cascadeData.numSamples = setting->m_numSamples;

	m_worldCamera.SetCameraPositionAndOrientation( setting->m_cam1Position, setting->m_cam1Orientation );
	m_worldCamera2.SetCameraPositionAndOrientation( setting->m_cam2Position, setting->m_cam2Orientation );
}

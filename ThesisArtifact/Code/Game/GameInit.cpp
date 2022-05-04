#include "Game.hpp"

#include "Game/App.hpp"
#include "Game/Player.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Lighting/LightCamera.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/VertexData/VertexUtils.hpp"


//----------------------------------------------------------------------------------------------------
void Game::Startup()
{
	g_theGame = this;

	CreateMouseConfigs();
	CreateEntities();
	CreateLightConfigs();
	ReloadXMLData();

	m_worldCamera.SetCameraType( CameraType::PERSPECTIVE );
	m_worldCamera.SetGameSpace( Vec3( 0.0f, -1.0f, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ), Vec3( 1.0f, 0.0f, 0.0f ) );
	m_worldCamera.SetAspect( 2.0f );
	m_worldCamera.SetFieldOfView( 45.0f );
	m_worldCamera.SetZNearZFar( m_nearPlane, m_farPlane );

	m_worldCamera2.SetCameraType( CameraType::PERSPECTIVE );
	m_worldCamera2.SetGameSpace( Vec3( 0.0f, -1.0f, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ), Vec3( 1.0f, 0.0f, 0.0f ) );
	m_worldCamera2.SetAspect( 2.0f );
	m_worldCamera2.SetFieldOfView( 45.0f );
	m_worldCamera2.SetZNearZFar( 0.001f, 1000.0f );

	m_worldCamera.SetOrthoView( Vec3( -2.0f, -2.0f, -2.0f ), Vec3( 2.0f, 2.0f, 2.0f ) );
	m_screenCamera.SetOrthoView( Vec3( -400.0f, -200.0f, 0.0f ), Vec3( 400.0f, 200.f, 1.0f ) );

	std::vector<std::string> texPaths;
	texPaths.push_back( "Data/Textures/0.png" );
	texPaths.push_back( "Data/Textures/1.png" );
	texPaths.push_back( "Data/Textures/3.png" );
	texPaths.push_back( "Data/Textures/2.png" );
	texPaths.push_back( "Data/Textures/4.png" );
	texPaths.push_back( "Data/Textures/5.png" );

	AABB3 aabb( -1.5f, -1.5f, -1.5f, 1.5f, 1.5f, 1.5f );
	AddVertsForAABBZ3D( m_skyBoxVerts, aabb );

	m_skybox = g_theRenderer->CreateOrGetCubeTextureFromFiles( texPaths );
}


//----------------------------------------------------------------------------------------------------
void Game::CreateEntities()
{
	m_player = new Player( this, Vec3( 0.0f, 0.f, 0.0f ), EulerAngles() );
	AABB3 floorBound = AABB3( -25.0f, -25.0f, -0.750f, 25.0f, 25.0f, -0.7f );
	AABB3 wallBounds =  AABB3(  5.0f, -5.0f, -5.0f, 6.0f, 5.0f, 5.0f );
	AABB3 cubeBounds = AABB3( -0.50f, -0.5f, -0.50f, 0.50f, 0.5f, 0.50f );

	m_cube1transform.AppendTranslation3D( Vec3( 1.00f, 0.0f, 0.0f ) );
	m_cubeTransforms[0].AppendTranslation3D( Vec3( 1.00f, -2.0f, 0.0f ) );
	//m_cubeTransforms[1].AppendTranslation3D( Vec3( 2.00f, 1.0f, 0.0f ) );
	//m_cubeTransforms[2].AppendTranslation3D( Vec3( 3.00f, -3.0f, 0.0f ) );
	//m_cubeTransforms[3].AppendTranslation3D( Vec3( 1.00f, -4.0f, 0.0f ) );

	for ( int cubeNum = 0; cubeNum < 1; cubeNum++ )
	{
		float yaw   = g_rng.GetRandomFloatInRange( 0.0f, 360.0f );
		float pitch = g_rng.GetRandomFloatInRange( -90.0f, 90.0f );

		m_cubeTransforms[ cubeNum ].AppendZRotation( yaw );
		m_cubeTransforms[ cubeNum ].AppendYRotation( pitch );
	}

	m_rotationMatrixPlusX = Mat44::CreateXRotationDegrees( 30.0f );   
	m_rotationMatrixPlusY = Mat44::CreateYRotationDegrees( 30.0f );   
	m_rotationMatrixMinusX = Mat44::CreateXRotationDegrees( -30.0f ); 
	m_rotationMatrixMinusY = Mat44::CreateYRotationDegrees( -30.0f ); 

	AddVertsForAABB3TBN( m_cubeVerts1, cubeBounds );
	AddVertsForAABB3TBN( m_wall, wallBounds );
	AddVertsForAABB3TBN( m_floor, floorBound );

	m_cubeBuffer = g_theRenderer->CreateDynamicVertexBuffer( m_cubeVerts1.size() * sizeof( Vertex_PCUTBN ) );
	m_cubeBuffer->CopyVertexData( m_cubeVerts1.data(), m_cubeVerts1.size() * sizeof( Vertex_PCUTBN ), sizeof( Vertex_PCUTBN ) );
}


//----------------------------------------------------------------------------------------------------
void Game::CreateOrigin()
{
	Mat44 xRotation;
	xRotation.AppendXRotation( 90 );

	Mat44 yRotation;
	yRotation.AppendTranslation3D( Vec3( 0.0f, 1.325f, 0.0f ) );
	yRotation.AppendZRotation( -90 );
	yRotation.AppendXRotation( 90 );

	Mat44 zRotation;
	zRotation.AppendTranslation3D( Vec3( -0.01f, -0.295f, 0.01f ) );
	zRotation.AppendYRotation( -90 );

	DebugAddWorldText( "X-Forward", 0.25f, Vec2( 0.f, 0.f ), Rgba8::RED, xRotation, -1.f );
	DebugAddWorldText( "Y-Left", 0.25f, Vec2( 0.f, 0.f ), Rgba8::GREEN, yRotation, -1.f );
	DebugAddWorldText( "Z-Up", 0.25f, Vec2( 0.f, 0.f ), Rgba8::BLUE, zRotation, -1.f );
}


//----------------------------------------------------------------------------------------------------
void Game::CreateMouseConfigs()
{
	m_playingConfig.m_priority = 0;
	m_playingConfig.m_isHidden = true;
	m_playingConfig.m_isLocked = true;
	m_playingConfig.m_isRelative = true;

	m_ImGuiConfig.m_priority   = 10;
	m_ImGuiConfig.m_isHidden   = false;
	m_ImGuiConfig.m_isLocked   = true;
	m_ImGuiConfig.m_isRelative = false;

	g_theInput->PushMouseConfig( &m_playingConfig );
}


//----------------------------------------------------------------------------------------------------
void Game::CreateLightConfigs()
{
	m_lightPresets[ 1 ].m_worldPosition       = Vec3::ZERO;
	m_lightPresets[ 1 ].m_lightType           = DIRECTIONAL_LIGHT;
	m_lightPresets[ 1 ].m_color               = ( Vec3::ONE ).GetNormalized();
	m_lightPresets[ 1 ].m_intensity           = 0.50f;
	m_lightPresets[ 1 ].m_direction           = Vec3( 1.0f, 0.0f, 0.0f );
	m_lightPresets[ 1 ].m_directionFactor     = 1.0f;
	m_lightPresets[ 1 ].m_attenuation         = Vec3( 0.0f, 1.0f, 0.0f );
	m_lightPresets[ 1 ].m_dotInnerAngle       = 0.866f;
	m_lightPresets[ 1 ].m_specularAttenuation = Vec3( 0.0f, 0.0f, 1.0f );
	m_lightPresets[ 1 ].m_dotOuterAngle       = 0.50f;

	m_lightPresets[ 2 ].m_worldPosition       = Vec3::ZERO;
	m_lightPresets[ 2 ].m_lightType           = SPOT_LIGHT;
	m_lightPresets[ 2 ].m_color               = ( Vec3::ONE ).GetNormalized();
	m_lightPresets[ 2 ].m_intensity           = 0.50f;
	m_lightPresets[ 2 ].m_direction           = Vec3( 1.0f, 0.0f, 0.0f );
	m_lightPresets[ 2 ].m_directionFactor     = 1.0f;
	m_lightPresets[ 2 ].m_attenuation         = Vec3( 0.0f, 1.0f, 0.0f );
	m_lightPresets[ 2 ].m_dotInnerAngle       = 0.866f;
	m_lightPresets[ 2 ].m_specularAttenuation = Vec3( 0.0f, 0.0f, 1.0f );
	m_lightPresets[ 2 ].m_dotOuterAngle       = 0.50f;

	m_lightPresets[ 3 ].m_worldPosition       = Vec3::ZERO;
	m_lightPresets[ 3 ].m_lightType           = POINT_LIGHT;
	m_lightPresets[ 3 ].m_color               = ( Vec3::ONE ).GetNormalized();
	m_lightPresets[ 3 ].m_intensity           = 0.50f;
	m_lightPresets[ 3 ].m_direction           = Vec3( 0.0f, 1.0f, 0.0f );
	m_lightPresets[ 3 ].m_directionFactor     = 0.0f;
	m_lightPresets[ 3 ].m_attenuation         = Vec3( 0.0f, 1.0f, 0.0f );
	m_lightPresets[ 3 ].m_dotInnerAngle       = -1.0f;
	m_lightPresets[ 3 ].m_specularAttenuation = Vec3( 0.0f, 0.0f, 1.0f );
	m_lightPresets[ 3 ].m_dotOuterAngle       = -1.0f;
	
	m_lightCameraArray = new LightCamera*[ MAXLIGHTS ];
	
	for ( int lightNum = 0; lightNum < MAXLIGHTS; lightNum++ )
	{
		m_lightCamFollow[ lightNum ] = false;
		m_lightCamDir[ lightNum ] = false;
		m_lightCameraArray[ lightNum ] = new LightCamera();
		m_lightCameraArray[ lightNum ]->SetLightValues( m_shaderLightData.m_lights[ lightNum ] );
		m_lightCameraArray[ lightNum ]->SetGameSpace( Vec3( 0.0f, -1.0f, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ), Vec3( 1.0f, 0.0f, 0.0f ) );
	}

	for ( uint cascadeNum = 0; cascadeNum < NUM_CASCADES; cascadeNum++ )
	{
		m_cascadeDepthPercent[cascadeNum] = ( cascadeNum + 1 ) * 100 / NUM_CASCADES;
	}
	m_cascadeData.dimensionOfDepthTexture = DEPTH_TEXTURE_SIZE;

	m_debugPrintConstantBuffer   = g_theRenderer->CreateConstantBuffer( sizeof( DebugCascadePrint ) );
	m_cascadeDepthConstantBuffer = g_theRenderer->CreateConstantBuffer( sizeof( CascadeConstantsData ) );
	m_cam1ConstantBuffer         = g_theRenderer->CreateConstantBuffer( sizeof( CameraConstantsForCamera1 ) );
}


//----------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	for ( int lightNum = 0; lightNum < MAXLIGHTS; lightNum++ )
	{
		delete m_lightCameraArray[ lightNum ];
		m_lightCameraArray[ lightNum ] = nullptr;
	}

	delete m_lightCameraArray;
	m_lightCameraArray = nullptr;

	g_theRenderer->DestroyVertexBuffer( m_cubeBuffer );
	g_theRenderer->DestroyConstantBuffer( m_debugPrintConstantBuffer );
	g_theRenderer->DestroyConstantBuffer( m_cascadeDepthConstantBuffer );
	g_theRenderer->DestroyConstantBuffer( m_cam1ConstantBuffer );
}
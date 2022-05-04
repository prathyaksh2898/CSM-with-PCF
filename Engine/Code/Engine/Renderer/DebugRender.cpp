#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/Mathutils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/VertexData/VertexUtils.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"

#include <vector>
#include <atomic>
#include <mutex>


#if defined(ENGINE_DEBUG_RENDERING)

//------------------------------------------------------------------------
enum class DebugWorldObjectType
{
	POINT,
	LINE,
	CYLINDER,
	SPHERE,
	SOLIDSPHERE,
	FRUSTUM,
	WIREAABB,
	ARROW,
	BASIS,
	TEXT,
	BILLBOARDTEXT
};


//------------------------------------------------------------------------
enum class DebugScreenObjectType
{
	TEXT,
	MESSAGE
};


//------------------------------------------------------------------------
struct DebugScreenObject
{
	std::vector<Vertex_PCU> m_localVerts;
	Stopwatch               m_lifeTimeStopWatch;
	Rgba8                   m_startColor;
	Rgba8                   m_endColor;
	DebugRenderMode         m_renderMode;
	Mat44                   m_transformMatrix;
	DebugScreenObjectType   m_objectType;
	Texture const* m_texture = nullptr;
};


//------------------------------------------------------------------------
struct DebugWorldObject
{
	std::vector<Vertex_PCU> m_localVerts;
	Stopwatch               m_lifeTimeStopWatch;
	Rgba8                   m_startColor;
	Rgba8                   m_endColor;
	DebugRenderMode         m_renderMode;
	Mat44                   m_modelMatrix;
	DebugWorldObjectType    m_objectType;
	RasterState             m_rasterState;
	Texture const* m_texture = nullptr;
};


//------------------------------------------------------------------------
class DebugRenderer
{
public:
	Clock* m_debugClock = nullptr;
	DebugRenderConfig              m_config;
	std::atomic<bool>              m_isVisible = false;
	std::vector<DebugWorldObject>  m_worldObjects;
	std::vector<DebugScreenObject> m_screenObjects;
};


//-----------------------------------------------------------------------------------------------
DebugRenderer g_theDebugRenderer;
std::mutex g_debugRenderLock;


//-----------------------------------------------------------------------------------------------
bool DebugRenderClear( EventArgs& args )
{
	UNUSED( args );
	DebugRenderClear();
	return true;
}


//-----------------------------------------------------------------------------------------------
bool DebugRenderToggle( EventArgs& args )
{
	UNUSED( args );

	if ( !g_theDebugRenderer.m_isVisible )
		DebugRenderSetVisible();

	else
		DebugRenderSetHidden();

	return true;
}


//-----------------------------------------------------------------------------------------------
bool DebugRenderSetTimeScale( EventArgs& args )
{
	float scale = args.GetValue( "Scale", 1.0f );

	g_theDebugRenderer.m_debugClock->SetTimeDilation( scale );

	return true;
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSystemStartup( DebugRenderConfig const& config )
{
	g_theDebugRenderer.m_config = config;
	g_theDebugRenderer.m_isVisible = config.m_startHidden;
	g_theDebugRenderer.m_debugClock = new Clock();

	SubscribeEventCallbackFunction( "DebugRenderToggle", DebugRenderToggle );
	SubscribeEventCallbackFunction( "DebugRenderClear", DebugRenderClear );
	SubscribeEventCallbackFunction( "DebugRenderSetTimeScale", DebugRenderSetTimeScale );
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSystemShutdown()
{
	DebugRenderClear();
	delete( g_theDebugRenderer.m_debugClock );
	g_theDebugRenderer.m_debugClock = nullptr;
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSetVisible()
{
	g_debugRenderLock.lock();
	g_theDebugRenderer.m_isVisible = true;
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugRenderSetHidden()
{
	g_debugRenderLock.lock();
	g_theDebugRenderer.m_isVisible = false;
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugRenderClear()
{
	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.erase( g_theDebugRenderer.m_worldObjects.begin(), g_theDebugRenderer.m_worldObjects.end() );
	g_theDebugRenderer.m_screenObjects.erase( g_theDebugRenderer.m_screenObjects.begin(), g_theDebugRenderer.m_screenObjects.end() );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugRenderBeginFrame()
{

}


//-----------------------------------------------------------------------------------------------
void DebugRenderWorldToCamera( Camera const& camera )
{
	if ( !g_theDebugRenderer.m_isVisible )
		return;

	Renderer* renderer = g_theDebugRenderer.m_config.m_renderer;
	renderer->SetBlendMode( BlendMode::ALPHA );
	renderer->BindShader( nullptr );
	int numWorldObjects = static_cast< int >( g_theDebugRenderer.m_worldObjects.size() );
	for ( int objNum = 0; objNum < numWorldObjects; objNum++ )
	{
		DebugWorldObject const& obj = g_theDebugRenderer.m_worldObjects[ objNum ];
		if ( !obj.m_lifeTimeStopWatch.HasElapsed() || obj.m_lifeTimeStopWatch.GetDuration() <= 0.0 )
		{
			ModelTransformationData data;
			Rgba8::LerpColor( obj.m_startColor, obj.m_endColor, obj.m_lifeTimeStopWatch.GetDuration() <= 0.0 ? 1.0f : static_cast< float >( obj.m_lifeTimeStopWatch.GetElapsedFraction() ) ).GetAsFloats( data.tint );
			data.modelMatrix = obj.m_modelMatrix;

			if ( obj.m_objectType == DebugWorldObjectType::BILLBOARDTEXT )
			{
				Mat44 cameraToText = camera.GetCameraOrientationMatrix();
				cameraToText.SetIJK3D( -cameraToText.GetJBasis3D(), cameraToText.GetKBasis3D(), -cameraToText.GetIBasis3D() );

				data.modelMatrix = Mat44();
				data.modelMatrix.AppendTranslation3D( obj.m_modelMatrix.GetTranslation3D() );
				data.modelMatrix.Append( cameraToText );
			}

			renderer->SetModelBuffer( data );
			renderer->SetRasterState( obj.m_rasterState );

			if ( obj.m_renderMode == DebugRenderMode::ALWAYS )
			{
				renderer->SetDepthOptions( DepthTest::ALWAYS, false );
			}

			else
			{
				renderer->SetDepthOptions( DepthTest::LESS_EQUAL, true );
			}

			renderer->BindTexture( obj.m_texture );
			renderer->DrawVertexArray( static_cast< int >( obj.m_localVerts.size() ), obj.m_localVerts.data() );

			if ( obj.m_renderMode == DebugRenderMode::XRAY )
			{
				renderer->SetDepthOptions( DepthTest::GREATER, false );
				data.tint[ 0 ] *= 0.5f;
				data.tint[ 1 ] *= 0.5f;
				data.tint[ 2 ] *= 0.5f;

				renderer->SetModelBuffer( data );
				renderer->BindTexture( obj.m_texture );
				renderer->DrawVertexArray( static_cast< int >( obj.m_localVerts.size() ), obj.m_localVerts.data() );
			}
		}
	}

}


//-----------------------------------------------------------------------------------------------
void DebugRenderScreenToCamera( Camera const& camera )
{
	if ( !g_theDebugRenderer.m_isVisible )
		return;

	Renderer* renderer = g_theDebugRenderer.m_config.m_renderer;
	int numScreenObjects = static_cast< int >( g_theDebugRenderer.m_screenObjects.size() );

	int messageNum = 3;

	renderer->SetBlendMode( BlendMode::ALPHA );
	renderer->BindShader( nullptr );

	RasterState state;
	state.m_cullmode = CullMode::BACK;
	state.m_fillMode = FillMode::SOLID;
	state.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;
	renderer->SetRasterState( state );

	for ( int objNum = 0; objNum < numScreenObjects; objNum++ )
	{
		DebugScreenObject const& obj = g_theDebugRenderer.m_screenObjects[ objNum ];
		if ( !obj.m_lifeTimeStopWatch.HasElapsed() || obj.m_lifeTimeStopWatch.GetDuration() <= 0.0 )
		{
			ModelTransformationData data;
			Rgba8::LerpColor( obj.m_startColor, obj.m_endColor, obj.m_lifeTimeStopWatch.GetDuration() <= 0.0 ? 0.0f : static_cast< float >( obj.m_lifeTimeStopWatch.GetElapsedFraction() ) ).GetAsFloats( data.tint );
			data.modelMatrix = obj.m_transformMatrix;

			if ( obj.m_objectType == DebugScreenObjectType::MESSAGE )
			{
				Vec2 translationAmount = Vec2( camera.GetOrthoTopRight().x, camera.GetOrthoTopRight().y ) - static_cast< float >( messageNum ) * Vec2( 0.0f, 7.0f );
				data.modelMatrix.AppendTranslation2D( translationAmount );
				messageNum++;
			}

			renderer->SetModelBuffer( data );
			renderer->BindTexture( obj.m_texture );
			renderer->DrawVertexArray( static_cast< int >( obj.m_localVerts.size() ), obj.m_localVerts.data() );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DebugRenderEndFrame()
{
	for ( int objNum = 0; objNum < static_cast< int >( g_theDebugRenderer.m_worldObjects.size() ); objNum++ )
	{
		DebugWorldObject const& obj = g_theDebugRenderer.m_worldObjects[ objNum ];
		if ( obj.m_lifeTimeStopWatch.HasElapsed() && !( obj.m_lifeTimeStopWatch.GetDuration() < 0.0 ) )
		{
			g_theDebugRenderer.m_worldObjects.erase( g_theDebugRenderer.m_worldObjects.begin() + objNum );
			objNum--;
		}
	}

	for ( int objNum = 0; objNum < static_cast< int >( g_theDebugRenderer.m_screenObjects.size() ); objNum++ )
	{
		DebugScreenObject const& obj = g_theDebugRenderer.m_screenObjects[ objNum ];
		if ( obj.m_lifeTimeStopWatch.HasElapsed() && !( obj.m_lifeTimeStopWatch.GetDuration() < 0.0 ) )
		{
			g_theDebugRenderer.m_screenObjects.erase( g_theDebugRenderer.m_screenObjects.begin() + objNum );
			objNum--;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::POINT;

	obj.m_rasterState.m_cullmode = CullMode::BACK;
	obj.m_rasterState.m_fillMode = FillMode::SOLID;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	obj.m_startColor = startColor;
	obj.m_endColor = endColor;

	obj.m_renderMode = mode;

	AABB3 pointBounds( -size * 0.5f, -size * 0.5f, -size * 0.5f, size * 0.5f, size * 0.5f, size * 0.5f );

	AddVertsForAABBZ3D( obj.m_localVerts, pointBounds, startColor );

	obj.m_modelMatrix.AppendTranslation3D( pos );
	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 color, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldPoint( pos, size, color, Rgba8( 0, 0, 0, 0 ), duration, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldPoint( Vec3 pos, Rgba8 color, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldPoint( pos, 0.1250f, color, Rgba8( 0, 0, 0, 255 ), duration, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldPoint( Vec3 pos, Rgba8 color, Rgba8 endColor, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldPoint( pos, 0.1250f, color, endColor, duration, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldLine( Vec3 start, Vec3 end, Rgba8 startColor, Rgba8 endColor, float thickness, float duration, DebugRenderMode mode )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::LINE;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::BACK;
	obj.m_rasterState.m_fillMode = FillMode::SOLID;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_startColor = startColor;
	obj.m_endColor = endColor;

	Vec3 startToEnd = end - start;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	AddVertsForCylinderZ3D( obj.m_localVerts, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, startToEnd.GetLength() ), thickness * 0.5f, 16.f );

	Vec3 kBasis( startToEnd );

	kBasis = kBasis.GetNormalized();

	Vec3 iBasis;
	Vec3 jBasis = CrossProduct3D( kBasis, Vec3( 1.0f, 0.0f, 0.0f ) ).GetNormalized();

	if ( jBasis.GetLength() == 0 )
	{
		iBasis = CrossProduct3D( Vec3( 0.0f, 1.0f, 0.0f ), kBasis );
		jBasis = CrossProduct3D( kBasis, iBasis );
	}

	else
	{
		iBasis = CrossProduct3D( jBasis, kBasis );
	}

	obj.m_modelMatrix.SetIJKT3D( iBasis, jBasis, kBasis, start );
	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldLine( Vec3 start, Vec3 end, Rgba8 color /*= Rgba8::WHITE*/, float thickness /*= 0.125f*/, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldLine( start, end, color, color, thickness, duration, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireCylinder( Vec3 base, Vec3 top, float radius, float duration, Rgba8 color, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::CYLINDER;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::NONE;
	obj.m_rasterState.m_fillMode = FillMode::WIREFRAME;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_startColor = color;
	obj.m_endColor = Rgba8::RED;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	Vec3 startToEnd = top - base;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	AddVertsForCylinderZ3D( obj.m_localVerts, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, startToEnd.GetLength() ), radius, 16.f );

	Vec3 kBasis( startToEnd );

	kBasis = kBasis.GetNormalized();

	Vec3 iBasis;
	Vec3 jBasis = CrossProduct3D( kBasis, Vec3( 1.0f, 0.0f, 0.0f ) ).GetNormalized();

	if ( jBasis.GetLength() == 0 )
	{
		iBasis = CrossProduct3D( Vec3( 0.0f, 1.0f, 0.0f ), kBasis );
		jBasis = CrossProduct3D( kBasis, iBasis );
	}

	else
	{
		iBasis = CrossProduct3D( jBasis, kBasis );
	}

	obj.m_modelMatrix.SetIJKT3D( iBasis, jBasis, kBasis, base );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


void DebugAddWorldWireCylinder( Vec3 base, Vec3 top, float radius, float duration, Rgba8 startcolor, Rgba8 endColor, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::CYLINDER;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::NONE;
	obj.m_rasterState.m_fillMode = FillMode::WIREFRAME;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_startColor = startcolor;
	obj.m_endColor = endColor;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	Vec3 startToEnd = top - base;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	AddVertsForCylinderZ3D( obj.m_localVerts, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, startToEnd.GetLength() ), radius, 16.f );

	Vec3 kBasis( startToEnd );

	kBasis = kBasis.GetNormalized();

	Vec3 iBasis;
	Vec3 jBasis = CrossProduct3D( kBasis, Vec3( 1.0f, 0.0f, 0.0f ) ).GetNormalized();

	if ( jBasis.GetLength() == 0 )
	{
		iBasis = CrossProduct3D( Vec3( 0.0f, 1.0f, 0.0f ), kBasis );
		jBasis = CrossProduct3D( kBasis, iBasis );
	}

	else
	{
		iBasis = CrossProduct3D( jBasis, kBasis );
	}

	obj.m_modelMatrix.SetIJKT3D( iBasis, jBasis, kBasis, base );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//------------------------------------------------------------------------------------------------
void DebugAddWorldWireAABB3( AABB3 aabb, Mat44 matrix, float duration, Rgba8 startcolor, Rgba8 endColor, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::CYLINDER;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::NONE;
	obj.m_rasterState.m_fillMode = FillMode::WIREFRAME;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_startColor = startcolor;
	obj.m_endColor = endColor;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	AddVertsForAABBZ3D( obj.m_localVerts, aabb );

	obj.m_modelMatrix = matrix;

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//------------------------------------------------------------------------------------------------
void DebugAddWorldWireAABB3( AABB3 aabb, Mat44 matrix, float duration, Rgba8 color, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldWireAABB3( aabb, matrix, duration, color, color, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldSphere( Vec3 center, float radius, float duration, Rgba8 color, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::SOLIDSPHERE;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::BACK;
	obj.m_rasterState.m_fillMode = FillMode::SOLID;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_startColor = color;
	obj.m_endColor = color;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	AddVertsForUVSphereZ3D( obj.m_localVerts, Vec3( 0.0f, 0.0f, 0.0f ), radius, 16.f, 8.f );

	obj.m_modelMatrix.SetTranslation3D( center );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldWireSphere( Vec3 center, float radius, float duration, Rgba8 color, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::SPHERE;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::BACK;
	obj.m_rasterState.m_fillMode = FillMode::WIREFRAME;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_startColor = color;
	obj.m_endColor = Rgba8::RED;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	AddVertsForUVSphereZ3D( obj.m_localVerts, Vec3( 0.0f, 0.0f, 0.0f ), radius, 16.f, 8.f );

	obj.m_modelMatrix.SetTranslation3D( center );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//------------------------------------------------------------------------------------------------
void DebugAddFrustum( Vec3 points[ 8 ] )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::FRUSTUM;
	obj.m_renderMode = DebugRenderMode::USE_DEPTH;

	obj.m_rasterState.m_cullmode = CullMode::BACK;
	obj.m_rasterState.m_fillMode = FillMode::WIREFRAME;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_startColor = Rgba8::WHITE;
	obj.m_endColor = Rgba8::WHITE;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( 0 );

	AddVertsForLine3D( obj.m_localVerts, points[ 0 ], points[ 1 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 0 ], points[ 2 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 2 ], points[ 3 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 1 ], points[ 3 ], 0.0635f );

	AddVertsForLine3D( obj.m_localVerts, points[ 0 ], points[ 4 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 1 ], points[ 5 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 2 ], points[ 6 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 3 ], points[ 7 ], 0.0635f );

	AddVertsForLine3D( obj.m_localVerts, points[ 4 ], points[ 5 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 5 ], points[ 7 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 4 ], points[ 6 ], 0.0635f );
	AddVertsForLine3D( obj.m_localVerts, points[ 6 ], points[ 7 ], 0.0635f );

	obj.m_modelMatrix = Mat44();

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldArrow( Vec3 start, Vec3 end, Rgba8 color /*= Rgba8::WHITE*/, float thickness /*= 0.125f*/, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	float arrowLength = ( end - start ).GetLength();
	float baseLength = 0.8f * arrowLength;
	float coneLength = 0.2f * arrowLength;

	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::ARROW;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::BACK;
	obj.m_rasterState.m_fillMode = FillMode::SOLID;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	obj.m_startColor = color;
	obj.m_endColor = color;

	AddVertsForCylinderZ3D( obj.m_localVerts, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, baseLength ), thickness * 0.25f, 16.f );
	AddVertsForConeZ3D( obj.m_localVerts, Vec2( 0.0f, 0.0f ), FloatRange( baseLength, coneLength + baseLength ), thickness * 0.5f, 16.f );

	Vec3 kBasis( end - start );

	kBasis = kBasis.GetNormalized();

	Vec3 iBasis;
	Vec3 jBasis = CrossProduct3D( kBasis, Vec3( 1.0f, 0.0f, 0.0f ) ).GetNormalized();

	if ( jBasis.GetLength() == 0 )
	{
		iBasis = CrossProduct3D( Vec3( 0.0f, 1.0f, 0.0f ), kBasis );
		jBasis = CrossProduct3D( kBasis, iBasis );
	}

	else
	{
		iBasis = CrossProduct3D( jBasis, kBasis );
	}

	obj.m_modelMatrix.SetIJKT3D( iBasis, jBasis, kBasis, start );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldBasis( Mat44 basis, Rgba8 startTint, Rgba8 endTint, float duration, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::BASIS;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::BACK;
	obj.m_rasterState.m_fillMode = FillMode::SOLID;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	obj.m_startColor = startTint;
	obj.m_endColor = endTint;

	std::vector<Vertex_PCU> zAxis;
	AddVertsForCylinderZ3D( zAxis, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, 0.8f ), 0.150f * 0.25f, 16.f, Rgba8::BLUE );
	AddVertsForConeZ3D( zAxis, Vec2( 0.0f, 0.0f ), FloatRange( 0.8f, 1.f ), 0.150f * 0.5f, 16.f, Rgba8::BLUE );

	Mat44 transformMatrix;
	transformMatrix.AppendYRotation( 90.0f );
	std::vector<Vertex_PCU> xAxis;
	AddVertsForCylinderZ3D( xAxis, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, 0.8f ), 0.150f * 0.25f, 16.f, Rgba8::RED );
	AddVertsForConeZ3D( xAxis, Vec2( 0.0f, 0.0f ), FloatRange( 0.8f, 1.f ), 0.150f * 0.5f, 16.f, Rgba8::RED );
	TransformVertexArrayUsingMatrix( static_cast< int >( xAxis.size() ), xAxis.data(), transformMatrix );

	transformMatrix = Mat44();
	transformMatrix.AppendXRotation( -90.0f );
	std::vector<Vertex_PCU> yAxis;
	AddVertsForCylinderZ3D( yAxis, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, 0.8f ), 0.150f * 0.25f, 16.f, Rgba8::GREEN );
	AddVertsForConeZ3D( yAxis, Vec2( 0.0f, 0.0f ), FloatRange( 0.8f, 1.f ), 0.150f * 0.5f, 16.f, Rgba8::GREEN );
	TransformVertexArrayUsingMatrix( static_cast< int >( yAxis.size() ), yAxis.data(), transformMatrix );

	obj.m_localVerts.insert( obj.m_localVerts.end(), xAxis.begin(), xAxis.end() );
	obj.m_localVerts.insert( obj.m_localVerts.end(), yAxis.begin(), yAxis.end() );
	obj.m_localVerts.insert( obj.m_localVerts.end(), zAxis.begin(), zAxis.end() );

	obj.m_modelMatrix = basis;
	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldBasis( Mat44 basis, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldBasis( basis, Rgba8::WHITE, Rgba8::WHITE, duration, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldText( std::string const& text, float textHeight, Vec2 alignment, Rgba8 startColor, Rgba8 endColor, Mat44 const& transform /*= Mat44()*/, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	float textAspect = 0.8f;
	float textWidth = text.length() * textHeight * textAspect;

	AABB2 bounds;

	bounds.m_mins = -Vec2( alignment.x * textWidth, alignment.y * textHeight );
	bounds.m_maxs = Vec2( ( 1.f - alignment.x ) * textWidth, ( 1.f - alignment.y ) * textHeight );

	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::TEXT;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::NONE;
	obj.m_rasterState.m_fillMode = FillMode::SOLID;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	obj.m_startColor = startColor;
	obj.m_endColor = endColor;

	BitmapFont* fontForWorldTextAttract = g_theDebugRenderer.m_config.m_renderer->CreateOrGetBitmapFontFromFile( "Data/Fonts/FixedFont" );
	obj.m_texture = &fontForWorldTextAttract->GetTexture();

	fontForWorldTextAttract->AddVertsForTextInBox2D( obj.m_localVerts, bounds, textHeight, text, startColor, textAspect, alignment );

	obj.m_modelMatrix = transform;

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldText( std::string const& text, float textHeight /*= 0.25f*/, Vec2 alignment /*= Vec2( 0.5f, 0.5f )*/, Rgba8 color /*= Rgba8::WHITE*/, Mat44 const& transform /*= Mat44()*/, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldText( text, textHeight, alignment, color, color, transform, duration, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldBillboardText( Vec3 origin, std::string const& text, float textHeight, Vec2 alignment, Rgba8 startcolor, Rgba8 endColor, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugWorldObject obj;
	obj.m_objectType = DebugWorldObjectType::BILLBOARDTEXT;
	obj.m_renderMode = mode;

	obj.m_rasterState.m_cullmode = CullMode::NONE;
	obj.m_rasterState.m_fillMode = FillMode::SOLID;
	obj.m_rasterState.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	obj.m_startColor = startcolor;
	obj.m_endColor = endColor;

	float textAspect = 0.8f;
	float textWidth = text.length() * textHeight * textAspect;

	AABB2 bounds;
	bounds.m_mins = -Vec2( alignment.x * textWidth, alignment.y * textHeight );
	bounds.m_maxs = Vec2( ( 1.f - alignment.x ) * textWidth, ( 1.f - alignment.y ) * textHeight );

	BitmapFont* fontForWorldTextAttract = g_theDebugRenderer.m_config.m_renderer->CreateOrGetBitmapFontFromFile( "Data/Fonts/FixedFont" );
	obj.m_texture = &fontForWorldTextAttract->GetTexture();

	fontForWorldTextAttract->AddVertsForTextInBox2D( obj.m_localVerts, bounds, textHeight, text, Rgba8::WHITE, textAspect, alignment );

	obj.m_modelMatrix.AppendTranslation3D( origin );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_worldObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddWorldBillboardText( Vec3 origin, std::string const& text, float textHeight /*= 0.5f*/, Vec2 alignment /*= Vec2( 0.5f, 0.5f )*/, Rgba8 color /*= Rgba8::WHITE*/, float duration /*= 0.0f*/, DebugRenderMode mode /*= DebugRenderMode::USE_DEPTH */ )
{
	DebugAddWorldBillboardText( origin, text, textHeight, alignment, color, Rgba8::RED, duration, mode );
}


//-----------------------------------------------------------------------------------------------
void DebugAddScreenText( std::string const& text, Vec2 position, float duration, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor )
{
	DebugScreenObject obj;
	obj.m_objectType = DebugScreenObjectType::TEXT;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	obj.m_startColor = startColor;
	obj.m_endColor = endColor;

	float textAspect = 0.75f;
	float textWidth = text.length() * size * textAspect;

	AABB2 bounds;
	bounds.m_mins = -Vec2( pivot.x * textWidth, pivot.y * size );
	bounds.m_maxs = Vec2( ( 1.f - pivot.x ) * textWidth, ( 1.f - pivot.y ) * size );

	BitmapFont* fontForWorldTextAttract = g_theDebugRenderer.m_config.m_renderer->CreateOrGetBitmapFontFromFile( "Data/Fonts/FixedFont" );
	obj.m_texture = &fontForWorldTextAttract->GetTexture();

	fontForWorldTextAttract->AddVertsForTextInBox2D( obj.m_localVerts, bounds, size, text, Rgba8::WHITE, textAspect, pivot );

	obj.m_transformMatrix.AppendTranslation2D( position );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_screenObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddScreenText( std::string const& text, Vec2 position, float duration /*= 0.0f*/, Vec2 pivot /*= Vec2( 0.0f, 0.0f )*/, float size /*= 32.0f*/, Rgba8 color /*= Rgba8::WHITE */ )
{
	DebugAddScreenText( text, position, duration, pivot, size, color, Rgba8::RED );
}


//-----------------------------------------------------------------------------------------------
void DebugAddMessage( std::string const& text, float duration, Rgba8 startColor, Rgba8 endColor )
{
	DebugScreenObject obj;
	obj.m_objectType = DebugScreenObjectType::MESSAGE;

	obj.m_lifeTimeStopWatch.SetClock( *( g_theDebugRenderer.m_debugClock ) );
	obj.m_lifeTimeStopWatch.Start( duration );

	obj.m_startColor = startColor;
	obj.m_endColor = endColor;

	float textSize = 7.f;
	float textAspect = 0.75f;
	float textWidth = text.length() * textSize * textAspect;
	Vec2 pivot = Vec2( 1.0f, 1.0f );

	AABB2 bounds;
	bounds.m_mins = -Vec2( pivot.x * textWidth, pivot.y * textSize );
	bounds.m_maxs = Vec2( ( 1.f - pivot.x ) * textWidth, ( 1.f - pivot.y ) * textSize );

	BitmapFont* fontForWorldTextAttract = g_theDebugRenderer.m_config.m_renderer->CreateOrGetBitmapFontFromFile( "Data/Fonts/FixedFont" );
	obj.m_texture = &fontForWorldTextAttract->GetTexture();

	fontForWorldTextAttract->AddVertsForTextInBox2D( obj.m_localVerts, bounds, textSize, text, Rgba8::WHITE, textAspect, pivot );

	g_debugRenderLock.lock();
	g_theDebugRenderer.m_screenObjects.push_back( obj );
	g_debugRenderLock.unlock();
}


//-----------------------------------------------------------------------------------------------
void DebugAddMessage( std::string const& text, float duration /*= 0.0f*/, Rgba8 color /*= Rgba8::WHITE */ )
{
	DebugAddMessage( text, duration, color, Rgba8::RED );
}


#endif

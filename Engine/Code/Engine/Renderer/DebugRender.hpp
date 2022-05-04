#pragma once

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#include <string>


#if defined(ENGINE_DEBUG_RENDERING)

//------------------------------------------------------------------------
struct Camera;
class Renderer;


//------------------------------------------------------------------------
enum class DebugRenderMode
{
	ALWAYS,          // what is rendered always shows up (newer objects usually in front of older objects)
	USE_DEPTH,       // respect the depth buffer
	XRAY,
};


//------------------------------------------------------------------------
struct DebugRenderConfig
{
	Renderer* m_renderer = nullptr;
	bool m_startHidden = false;
};


//------------------------------------------------------------------------
// setup
void DebugRenderSystemStartup( DebugRenderConfig const& config );    // setup the debug render system (call from Renderer::Startup)
void DebugRenderSystemShutdown();   // cleans up the system to prevent leaks (call from Renderer::Shutdown)

// control
void DebugRenderSetVisible();		// enables the debug render system 
void DebugRenderSetHidden();		// disable the debug render system
void DebugRenderClear();			// clears all current debug render instructions

// output
void DebugRenderBeginFrame();								// ...handle any pre-frame needs of the system, call from `Renderer::BeginFrame`
void DebugRenderWorldToCamera( Camera const& camera );		// Draws all world objects to this camera 
void DebugRenderScreenToCamera( Camera const& camera );		// Draws all screen objects onto this texture (screen coordinate system is up to you.  I like a 1080p default)
void DebugRenderEndFrame();									// ...handle any post frame needs of the system, call from `Renderer::EndFrame`

// points
void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 startColor, Rgba8 endColor, float duration, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldPoint( Vec3 pos, float size, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldPoint( Vec3 pos, Rgba8 color, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldPoint( Vec3 pos, Rgba8 color, Rgba8 endColor, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH  );

// lines
void DebugAddWorldLine( Vec3 start, Vec3 end, Rgba8 startColor, Rgba8 endColor,	float thickness, float duration, DebugRenderMode mode );
void DebugAddWorldLine( Vec3 start, Vec3 end, Rgba8 color = Rgba8::WHITE, float thickness = 0.125f, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

// cylinder
void DebugAddWorldWireCylinder( Vec3 base,	Vec3 top, float radius,	float duration,	Rgba8 color, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldWireCylinder( Vec3 base,	Vec3 top, float radius,	float duration,	Rgba8 startcolor, Rgba8 endColor, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

// AABB
void DebugAddWorldWireAABB3( AABB3 aabb, Mat44 matrix, float duration, Rgba8 color, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldWireAABB3( AABB3 aabb, Mat44 matrix, float duration, Rgba8 startcolor, Rgba8 endColor, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

// sphere
void DebugAddWorldSphere( Vec3 center, float radius, float duration, Rgba8 color, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldWireSphere( Vec3 center, float radius, float duration, Rgba8 color, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

//Frustum
void DebugAddFrustum( Vec3 points[8]);

// arrows
void DebugAddWorldArrow( Vec3 start, Vec3 end, Rgba8 color = Rgba8::WHITE, float thickness = 0.125f, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

// basis
void DebugAddWorldBasis( Mat44 basis, Rgba8 startTint, Rgba8 endTint, float duration, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldBasis( Mat44 basis, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

// text
void DebugAddWorldText( std::string const& text, float textHeight, Vec2 alignment, Rgba8 startColor, Rgba8 endColor, Mat44 const& transform = Mat44(), float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldText( std::string const& text, float textHeight = 0.25f, Vec2 alignment = Vec2( 0.5f, 0.5f ), Rgba8 color = Rgba8::WHITE, Mat44 const& transform = Mat44(), float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

void DebugAddWorldBillboardText( Vec3 origin, std::string const& text, float textHeight, Vec2 alignment, Rgba8 startcolor, Rgba8 endColor, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );
void DebugAddWorldBillboardText( Vec3 origin, std::string const& text, float textHeight = 0.5f, Vec2 alignment = Vec2( 0.5f, 0.5f ), Rgba8 color = Rgba8::WHITE, float duration = 0.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH );

// text - put anywhere on the screen.  Useful for debugging values.
void DebugAddScreenText( std::string const& text, Vec2 position, float duration, Vec2 pivot, float size, Rgba8 startColor, Rgba8 endColor );
void DebugAddScreenText( std::string const& text, Vec2 position, float duration = 0.0f, Vec2 pivot = Vec2( 0.0f, 0.0f ), float size = 32.0f, Rgba8 color = Rgba8::WHITE );

// messages - messages display after text, at the top of the screen, most recent on top.  
void DebugAddMessage( std::string const& text, float duration, Rgba8 startColor, Rgba8 endColor );
void DebugAddMessage( std::string const& text, float duration = 0.0f, Rgba8 color = Rgba8::WHITE );

#endif


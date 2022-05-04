#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"

#include <vector>


//----------------------------------------------------------------------------------------------------
struct Vec2;
struct Vec3;
struct AABB3;
struct IntVec2;
struct OBB2;
struct Capsule2;
struct LineSegment2;

class Mat44;


//----------------------------------------------------------------------------------------------------
void TransformVertexArray2D( int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotatingDegreesAboutZ, Vec2 const& translationXY );
void TransformVertexArrayXY3D( int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotatingDegreesAboutZ, Vec2 const& translationXY );

void TransformVertexArrayUsingMatrix( int numVerts, Vertex_PCU* verts, Mat44 const& transformMatrix );
void TransformVertexArrayPositionsUsingMatrix( int numVerts, Vertex_PCU* verts, Mat44 const& transformMatrix );

void TransformVertexArrayUsingMatrix( int numVerts, Vertex_PCUTBN* verts, Mat44 const& transformMatrix );
void TransformVertexArrayPositionsUsingMatrix( int numVerts, Vertex_PCUTBN* verts, Mat44 const& transformMatrix );

void AddVertsForAABB2D( std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, AABB2 const& uvBounds = AABB2::ZEROTOONE );
void AddVertsForCapsule2D( std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color );
void AddVertsForCapsule2D( std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color );
void AddVertsForDisc2D( std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color );
void AddVertsForOBB2D( std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color );

void AddVertsForLineSegment2D( std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color );
void AddVertsForLineSegment2D( std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color );
void AddVertsForLineSegment2DWithArrow( std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color );
void AddVertsForLineSegment2DWithArrow( std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color );

void AddVertsForAABBZ3D( std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE );
void AddVertsForUVSphereZ3D( std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices = 64, float numStacks = 32.0f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE );
void AddVertsForCylinderZ3D( std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices = 64.f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE );

void AddVertsForLine3D( std::vector<Vertex_PCU>& verts, Vec3 start, Vec3 end, float thickness = 0.125f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE );
void AddVertsForLine3DWithArrow( std::vector<Vertex_PCU>& verts, Vec3 start, Vec3 end, float thickness = 0.125f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE );

void AddVertsForAABBZ3DWireFrame( std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE, float thickness = 0.03125f );
void AddVertsForUVSphereZ3DWireFrame( std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices = 64, float numStacks = 32.0f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE, float thickness = 0.03125f );
void AddVertsForCylinderZ3DWireFrame( std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices = 64.f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE, float thickness = 0.03125f );

void AddVertsForConeZ3D( std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices = 64.f, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE );

void AddVertsForQuad3D( std::vector<Vertex_PCU>& verts, Vec3 bottomLeft, Vec3 bottomRight, Vec3 topLeft, Vec3 topRight, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& uvBounds = AABB2::ZEROTOONE );

void AddVertsForAABB3TBN( std::vector<Vertex_PCUTBN>& verts, AABB3 const& bounds, Rgba8 const& tint = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZEROTOONE );
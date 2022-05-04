#pragma once
#include "Vec2.hpp"
#include "Vec3.hpp"

//-----------------------------------------------------------------------------------------------
struct LineSegment2;
struct AABB2;
struct FloatRange;
struct AABB3;


//-----------------------------------------------------------------------------------------------
struct BaseRaycastResult2D
{
	Vec2    m_start               = Vec2::ZERO;
	Vec2    m_direction           = Vec2::ZERO;
	float   m_distance            = 0.0f;
	bool    m_didImpact           = false;
	Vec2    m_impactPosition      = Vec2::ZERO;
	float   m_impactFraction      = 0.0f;
	float   m_impactDistance      = 0.0f;
	Vec2    m_impactSurfaceNormal = Vec2::ZERO;
};


//-----------------------------------------------------------------------------------------------
struct BaseRaycastResult3D
{
	Vec3    m_start               = Vec3::ZERO;
	Vec3    m_direction           = Vec3::ZERO;
	float   m_distance            = 0.0f;
	bool    m_didImpact           = false;
	Vec3    m_impactPosition      = Vec3::ZERO;
	float   m_impactFraction      = 0.0f;
	float   m_impactDistance      = 0.0f;
	Vec3    m_impactSurfaceNormal = Vec3::ZERO;
};


//-----------------------------------------------------------------------------------------------
BaseRaycastResult2D RaycastVSDisc2D( Vec2 startLocation, Vec2 direction, float distance, Vec2 centerXY, float radius );
BaseRaycastResult2D RaycastVSLineSegment2D( Vec2 startLocation, Vec2 direction, float distance, LineSegment2 const& line );
BaseRaycastResult2D RaycastVSAABB2D( Vec2 startLocation, Vec2 direction, float distance, AABB2 const& aabb2 );

BaseRaycastResult3D RaycastVSAABB3D( Vec3 startLocation, Vec3 direction, float distance, AABB3 const& aabb3 );
BaseRaycastResult3D RaycastVSSpheres( Vec3 startLocation, Vec3 direction, float distance, Vec3 center, float radius );
BaseRaycastResult3D RaycastVSCylinders( Vec3 startLocation, Vec3 direction, float distance, Vec2 centerXY, float radius, FloatRange const& height );

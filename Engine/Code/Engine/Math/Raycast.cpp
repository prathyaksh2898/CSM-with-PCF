#include "Raycast.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/FloatRange.hpp"

#include <math.h>

//-----------------------------------------------------------------------------------------------
BaseRaycastResult2D RaycastVSDisc2D( Vec2 startLocation, Vec2 direction, float distance, Vec2 centerXY, float radius )
{
	Vec2 i = direction;
	Vec2 j = i.GetRotated90Degrees();

	BaseRaycastResult2D result;

	Vec2 center = centerXY;
	Vec2 SC = center - startLocation;

	float SCj = DotProduct2D( SC, j );

	if ( SCj > radius || SCj < -radius )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}

	float SCi = DotProduct2D( SC, i );

	if ( SCi > distance + radius || SCi < -radius )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}
		

	if ( IsPointInsideDisc2D( startLocation, center, radius) )
	{
		if ( !result.m_didImpact )
		{
			result.m_start               = startLocation;
			result.m_direction           = direction;
			result.m_distance            = distance;
			result.m_didImpact           = true;
			result.m_impactPosition      = startLocation;
			result.m_impactFraction      = 0.0f;
			result.m_impactDistance      = 0.0f;
			result.m_impactSurfaceNormal = -direction;
			return result;
		}
	}

	float m = sqrtf( radius * radius - SCj * SCj );
	float impactDist = SCi - m;
	float t = impactDist / distance;

	if ( impactDist <= 0 || impactDist > distance )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}

	Vec2 impactPoint = startLocation + i * impactDist;
	Vec2 normal      = ( impactPoint - center ).GetNormalized();

	result.m_start               = startLocation;
	result.m_direction           = direction;
	result.m_distance            = distance;
	result.m_didImpact           = true;
	result.m_impactPosition      = impactPoint;
	result.m_impactFraction      = t;
	result.m_impactDistance      = impactDist;
	result.m_impactSurfaceNormal = normal;

	return result;
}


//-----------------------------------------------------------------------------------------------
BaseRaycastResult2D RaycastVSLineSegment2D( Vec2 startLocation, Vec2 direction, float distance, LineSegment2 const& line )
{
	Vec2 i = direction;
	Vec2 j = i.GetRotated90Degrees();
	
	BaseRaycastResult2D result;

	Vec2 startU = line.m_start - startLocation;
	Vec2 startV = line.m_end - startLocation;

	float uj = DotProduct2D( startU, j );
	float vj = DotProduct2D( startV, j );

	if ( uj * vj >= 0.0f )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}

	float ui = DotProduct2D( startU, i );
	float vi = DotProduct2D( startV, i );

	if ( ui > distance && vi > distance || ui < 0.0f && vi < 0.0f )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}

	float lengthOfLine = vi - ui;

	float piMinusUi;

	piMinusUi = fabsf( uj ) * lengthOfLine / (fabsf( vj )+ fabsf( uj ) );

	float pi = ui + piMinusUi;

	if ( pi < 0 || pi > distance )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}

	Vec2 lineDirection = ( line.m_end - line.m_start ).GetNormalized();
	Vec2 lineNormal = lineDirection.GetRotated90Degrees();
	
	result.m_start               = startLocation;
	result.m_direction           = direction;
	result.m_distance            = distance;
	result.m_didImpact           = true;
	result.m_impactPosition      = startLocation + pi * direction;
	result.m_impactFraction      = pi / distance;
	result.m_impactDistance      = pi;
	result.m_impactSurfaceNormal = DotProduct2D(lineNormal, direction) > 0 ? -lineNormal : lineNormal;

	return result;
}


//-----------------------------------------------------------------------------------------------
BaseRaycastResult2D RaycastVSAABB2D( Vec2 startLocation, Vec2 direction, float distance, AABB2 const& aabb2 )
{
	Vec2 endLocation = startLocation + direction * distance;

	AABB2 rayAABB;

	if ( direction.x < 0.0f && direction.y < 0.0f )
	{
		rayAABB = AABB2( endLocation, startLocation );
	}

	else if ( direction.x < 0.0f )
	{
		rayAABB = AABB2( startLocation + Vec2( direction.x, 0.0f ) * distance, endLocation - Vec2( direction.x, 0.0f ) * distance );
	}

	else if ( direction.y < 0.0f )
	{
		rayAABB = AABB2( startLocation + Vec2( 0.0f, direction.y ) * distance, endLocation - Vec2( 0.0f, direction.y ) * distance );
	}

	else
	{
		rayAABB = AABB2( startLocation, endLocation );
	}

	BaseRaycastResult2D result;

	if ( IsPointInsideAABB2D( startLocation, aabb2 ) )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = true;
		result.m_impactPosition      = startLocation;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = -direction;

		return result;
	}

	if ( !DoAABBsOverlap( aabb2, rayAABB ) )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}

	float startX = startLocation.x;
	float endX = startLocation.x + direction.x * distance;

	float startY = startLocation.y;
	float endY = startLocation.y + direction.y * distance;
	
	float overlapMinX = GetFractionWithin( aabb2.m_mins.x, startX, endX );
	float overlapMaxX = GetFractionWithin( aabb2.m_maxs.x, startX, endX );

	float overlapMinY = GetFractionWithin( aabb2.m_mins.y, startY, endY );
	float overlapMaxY = GetFractionWithin( aabb2.m_maxs.y, startY, endY );

	if ( startX == endX )
	{ 
		overlapMinX = 0.0f;
		overlapMaxX = 1.0f;
	}

	if ( startY == endY )
	{
		overlapMinY = 0.0f;
		overlapMaxY = 1.0f;
	}

	FloatRange txOverlap( overlapMinX, overlapMaxX );
	FloatRange tyOverlap( overlapMinY, overlapMaxY );
	FloatRange overlap;

	bool wasThereOverlap = txOverlap.GetOverlap( tyOverlap, overlap );


	if ( !wasThereOverlap || overlap.m_min > 1.0f || overlap.m_max < 0.0f )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec2::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec2::ZERO;

		return result;
	}

	Vec2 normal;

	if ( overlap.m_min == overlapMinX )
	{
		normal = Vec2( -1.0f, 0.0f );
	}

	else if ( overlap.m_min == overlapMaxX )
	{
		normal = Vec2( 1.0f, 0.0f );
	}

	else if ( overlap.m_min == overlapMinY )
	{
		normal = Vec2( 0.0f, -1.0f );
	}

	else
	{
		normal = Vec2( 0.0f, 1.0f );
	}

	result.m_start               = startLocation;
	result.m_direction           = direction;
	result.m_distance            = distance;
	result.m_didImpact           = true;
	result.m_impactPosition      = startLocation + overlap.m_min * direction * distance;
	result.m_impactFraction      = overlap.m_min;
	result.m_impactDistance      = overlap.m_min * distance;
	result.m_impactSurfaceNormal = normal; 

	return result;
}


//-----------------------------------------------------------------------------------------------
BaseRaycastResult3D RaycastVSAABB3D( Vec3 startLocation, Vec3 direction, float distance, AABB3 const& aabb3 )
{
	Vec3 endLocation = startLocation + direction * distance;

	AABB3 rayAABB;

	Vec3 boxStart = startLocation;
	Vec3 boxEnd = endLocation;

	if ( direction.x < 0.0f )
	{
		boxStart = boxStart + Vec3( direction.x, 0.0f, 0.0f ) * distance;
		boxEnd = boxEnd - Vec3( direction.x, 0.0f, 0.0f ) * distance;
	}

	if ( direction.y < 0.0f )
	{
		boxStart = boxStart + Vec3( 0.0f, direction.y, 0.0f ) * distance;
		boxEnd = boxEnd - Vec3( 0.0f, direction.y, 0.0f ) * distance;
	}

	if ( direction.z < 0.0f )
	{
		boxStart = boxStart + Vec3( 0.0f, 0.0f, direction.z ) * distance;
		boxEnd = boxEnd - Vec3( 0.0f, 0.0f, direction.z ) * distance;
	}

	rayAABB = AABB3( boxStart, boxEnd );

	BaseRaycastResult3D result;

	if ( IsPointInsideAABB3D( startLocation, aabb3 ) )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = true;
		result.m_impactPosition      = startLocation;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = -direction;

		return result;
	}

	if ( !DoAABB3sOverlap( aabb3, rayAABB ) )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	float startX = startLocation.x;
	float endX   = startLocation.x + direction.x * distance;

	float startY = startLocation.y;
	float endY   = startLocation.y + direction.y * distance;

	float startZ = startLocation.z;
	float endZ   = startLocation.z + direction.z * distance;

	float overlapMinX = GetFractionWithin( aabb3.m_mins.x, startX, endX );
	float overlapMaxX = GetFractionWithin( aabb3.m_maxs.x, startX, endX );

	float overlapMinY = GetFractionWithin( aabb3.m_mins.y, startY, endY );
	float overlapMaxY = GetFractionWithin( aabb3.m_maxs.y, startY, endY );

	float overlapMinZ = GetFractionWithin( aabb3.m_mins.z, startZ, endZ );
	float overlapMaxZ = GetFractionWithin( aabb3.m_maxs.z, startZ, endZ );

	if ( startX == endX )
	{
		overlapMinX = 0.0f;
		overlapMaxX = 1.0f;
	}

	if ( startY == endY )
	{
		overlapMinY = 0.0f;
		overlapMaxY = 1.0f;
	}

	if ( startZ == endZ )
	{
		overlapMinZ = 0.0f;
		overlapMaxZ = 1.0f;
	}

	FloatRange txOverlap( overlapMinX, overlapMaxX );
	FloatRange tyOverlap( overlapMinY, overlapMaxY );
	FloatRange tzOverlap( overlapMinZ, overlapMaxZ );
	FloatRange xYOverlap;
	FloatRange overlap;

	bool wasThereOverlap = txOverlap.GetOverlap( tyOverlap, xYOverlap );

	if ( !wasThereOverlap )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	wasThereOverlap = xYOverlap.GetOverlap( tzOverlap, overlap );

	if ( !wasThereOverlap || overlap.m_min > 1.0f || overlap.m_max < 0.0f )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	Vec3 normal;

	if ( overlap.m_min == overlapMinX )
	{
		normal = Vec3( -1.0f, 0.0f, 0.0f );
	}

	else if ( overlap.m_min == overlapMaxX )
	{
		normal = Vec3( 1.0f, 0.0f, 0.0f );
	}

	else if ( overlap.m_min == overlapMinY )
	{
		normal = Vec3( 0.0f, -1.0f, 0.0f );
	}

	else if ( overlap.m_min == overlapMaxY )
	{
		normal = Vec3( 0.0f, 1.0f, 0.0f );
	}

	else if ( overlap.m_min == overlapMinZ )
	{
		normal = Vec3( 0.0f, 0.0f, -1.0f );
	}

	else
	{
		normal = Vec3( 0.0f, 0.0f, 1.0f );
	}

	result.m_start               = startLocation;
	result.m_direction           = direction;
	result.m_distance            = distance;
	result.m_didImpact           = true;
	result.m_impactPosition      = startLocation + overlap.m_min * direction * distance;
	result.m_impactFraction      = overlap.m_min;
	result.m_impactDistance      = overlap.m_min * distance;
	result.m_impactSurfaceNormal = normal;

	return result;
}


//-----------------------------------------------------------------------------------------------
BaseRaycastResult3D RaycastVSSpheres( Vec3 startLocation, Vec3 direction, float distance, Vec3 center, float radius )
{
	Vec3 i = direction;

	BaseRaycastResult3D result;

	Vec3 SC = center - startLocation;

	float SCi = DotProduct3D( SC, i );

	if ( SCi >= radius + distance || SCi <= -radius )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	Vec3 vecSCi = SCi * i;

	Vec3 vecNotSCi = SC - vecSCi;

	if ( vecNotSCi.GetLengthSquared() >= radius * radius )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	if ( IsPointInsideSphere3D( startLocation, center, radius ) )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = true;
		result.m_impactPosition      = startLocation;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = -i;

		return result;
	}

	float m = sqrtf( radius * radius - vecNotSCi.GetLengthSquared() );

	float Pi = SCi - m;

	if ( Pi >= distance || Pi <= 0.0f )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	Vec3 impactPoint = startLocation + i * Pi;
	Vec3 normal = ( impactPoint - center ).GetNormalized();

	result.m_start               = startLocation;
	result.m_direction           = direction;
	result.m_distance            = distance;
	result.m_didImpact           = true;
	result.m_impactPosition      = impactPoint;
	result.m_impactFraction      = Pi / distance;
	result.m_impactDistance      = Pi;
	result.m_impactSurfaceNormal = normal;

	return result;
}


//-----------------------------------------------------------------------------------------------
BaseRaycastResult3D RaycastVSCylinders( Vec3 startLocation, Vec3 direction, float distance, Vec2 centerXY, float radius, FloatRange const& height )
{
	BaseRaycastResult3D result;

	Vec2 startLoc  = Vec2( startLocation.x, startLocation.y );
	Vec2 dir       = Vec2( direction.x, direction.y );

	float dist = distance * dir.GetLength();

	dir.Normalize();

	Vec2 i = dir;
	Vec2 j = i.GetRotated90Degrees();

	Vec2 center = centerXY;
	Vec2 SC = center - startLoc;

	float SCj = DotProduct2D( SC, j );

	if ( SCj > radius || SCj < -radius )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	float SCi = DotProduct2D( SC, i );

	if ( SCi > dist + radius || SCi < -radius )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	if ( IsPointInsideZCylinder( startLocation, centerXY, radius, height ) )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = true;
		result.m_impactPosition      = startLocation;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = -direction;

		return result;
	}

	float startZ = startLocation.z;
	float endZ = startLocation.z + direction.z * distance;

	float overlapMinZ = GetFractionWithin( height.m_min, startZ, endZ );
	float overlapMaxZ = GetFractionWithin( height.m_max, startZ, endZ );

	if ( startZ == endZ )
	{
		if ( height.IsOnRange( startZ ) )
		{
			overlapMinZ = 0.0f;
			overlapMaxZ = 1.0f;
		}

		else
		{
			result.m_start               = startLocation;
			result.m_direction           = direction;
			result.m_distance            = distance;
			result.m_didImpact           = false;
			result.m_impactPosition      = Vec3::ZERO;
			result.m_impactFraction      = 0.0f;
			result.m_impactDistance      = 0.0f;
			result.m_impactSurfaceNormal = Vec3::ZERO;

			return result;
		}

	}

	FloatRange tzOverlap( overlapMinZ, overlapMaxZ );

	float m = sqrtf( radius * radius - SCj * SCj );
	float impactDistEntry = SCi - m;
	float impactDistExit  = SCi + m;
	float tEntry = impactDistEntry / dist;
	float tExit  = impactDistExit / dist;

	FloatRange overlap;
	bool wasThereOverlap = tzOverlap.GetOverlap( FloatRange( tEntry, tExit ), overlap );

	if ( !wasThereOverlap || overlap.m_min >= 1.0f || overlap.m_min <= 0.0f )
	{
		result.m_start               = startLocation;
		result.m_direction           = direction;
		result.m_distance            = distance;
		result.m_didImpact           = false;
		result.m_impactPosition      = Vec3::ZERO;
		result.m_impactFraction      = 0.0f;
		result.m_impactDistance      = 0.0f;
		result.m_impactSurfaceNormal = Vec3::ZERO;

		return result;
	}

	Vec3 normal;

	if ( overlap.m_min == overlapMinZ )
	{
		normal = Vec3( 0.0f, 0.0f, -1.0f );
	}

	else if ( overlap.m_min == overlapMaxZ )
	{
		normal = Vec3( 0.0f, 0.0f, 1.0f );
	
	}

	else
	{
		float impactDist = ( overlap.m_min * direction ).GetLength();

		Vec2 impactPoint = startLoc + i * impactDist;
		Vec2 norm = ( impactPoint - center ).GetNormalized();

		normal = Vec3( norm );
	}

	result.m_start               = startLocation;
	result.m_direction           = direction;
	result.m_distance            = distance;
	result.m_didImpact           = true;
	result.m_impactPosition      = startLocation + direction * distance * overlap.m_min;
	result.m_impactFraction      = overlap.m_min;
	result.m_impactDistance      = overlap.m_min * distance;
	result.m_impactSurfaceNormal = normal;

	return result;

}


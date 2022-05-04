#include <math.h>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


//-----------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees( float radians )
{
	return radians * ( 180.f / 3.1415926535897932384626433832795f );
}


//-----------------------------------------------------------------------------------------------
float ConvertDegreesToRadians( float degrees )
{
	return degrees * ( 3.1415926535897932384626433832795f / 180.f );
}


//-----------------------------------------------------------------------------------------------
float SinDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	return sinf( radians );
}


//-----------------------------------------------------------------------------------------------
float CosDegrees( float degrees )
{
	float radians = ConvertDegreesToRadians( degrees );
	return cosf( radians );
}


//-----------------------------------------------------------------------------------------------
float Atan2Degrees( float y, float x )
{
	return ConvertRadiansToDegrees( atan2f( y, x ) );
}


//-----------------------------------------------------------------------------------------------
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB )
{
	float differenceInX = positionA.x - positionB.x;
	float differenceInY = positionA.y - positionB.y;
	
	return sqrtf( differenceInX * differenceInX + differenceInY * differenceInY );
}


//-----------------------------------------------------------------------------------------------
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB )
{
	float differenceInX = positionA.x - positionB.x;
	float differenceInY = positionA.y - positionB.y;
	
	return ( differenceInX * differenceInX + differenceInY * differenceInY );
}


//-----------------------------------------------------------------------------------------------
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float differenceInX = positionA.x - positionB.x;
	float differenceInY = positionA.y - positionB.y;
	float differenceInZ = positionA.z - positionB.z;
	
	return sqrtf( differenceInX * differenceInX + differenceInY * differenceInY + differenceInZ * differenceInZ );
}

//parantheses for indi
//-----------------------------------------------------------------------------------------------
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float differenceInX = positionA.x - positionB.x;
	float differenceInY = positionA.y - positionB.y;
	float differenceInZ = positionA.z - positionB.z;
	
	return ( differenceInX * differenceInX + differenceInY * differenceInY + differenceInZ * differenceInZ );
//	return (dx*dx) + (dy*dy) + (d*dz)
}


//-----------------------------------------------------------------------------------------------
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float differenceInX = positionA.x - positionB.x;
	float differenceInY = positionA.y - positionB.y;
	
	return sqrtf( differenceInX * differenceInX + differenceInY * differenceInY );
}


//-----------------------------------------------------------------------------------------------
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB )
{
	float differenceInX = positionA.x - positionB.x;
	float differenceInY = positionA.y - positionB.y;
	
	return ( differenceInX * differenceInX + differenceInY * differenceInY );
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius )
{
	if ( GetDistanceSquared2D( point, discCenter ) < discRadius * discRadius )
		return true;
	
	return false;
}


//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB )
{
	float distanceBetweenCenters = GetDistanceSquared2D( centerA, centerB );
	//sum of radii and then square
	if ( distanceBetweenCenters < ( radiusA + radiusB ) * ( radiusA + radiusB ) )
		return true;
	
	return false;
}


//-----------------------------------------------------------------------------------------------
bool DoAABBsOverlap( AABB2 const& first, AABB2 const& second )
{
	if ( first.m_maxs.x <= second.m_mins.x )
		return false;

	if ( second.m_maxs.x <= first.m_mins.x )
		return false;

	if ( first.m_maxs.y <= second.m_mins.y )
		return false;

	if ( second.m_maxs.y <= first.m_mins.y )
		return false;

	return true;
}


//-----------------------------------------------------------------------------------------------
bool DoesAABB2AndDiscOverlap( Vec2 const& center, float radius, AABB2 const& aabb )
{
	Vec2 point = GetNearestPointOnAABB2D( center, aabb );

	return IsPointInsideDisc2D( point, center, radius );
}


//-----------------------------------------------------------------------------------------------
bool DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB )
{
	float distanceBetweenCenters = GetDistanceSquared3D( centerA, centerB );
	
	if ( distanceBetweenCenters < ( radiusA + radiusB ) * ( radiusA + radiusB ) )
		return true;
	
	return false;
}


//-----------------------------------------------------------------------------------------------
bool DoesSphereAndCylinderOverlap( Vec3 const& centerSphere, float radiusSphere, Vec2 const& centerCylinder, float radiusCylinder, FloatRange const& heightCylinder )
{
	Vec3 point = GetNearestPointOnCylinder3D( centerSphere, centerCylinder, radiusCylinder, heightCylinder );
	
	return IsPointInsideSphere3D( point, centerSphere, radiusSphere );
}


//-----------------------------------------------------------------------------------------------
bool DoesSphereAndAABB3Overlap( Vec3 const& centerSphere, float radiusSphere, AABB3 aabb )
{
	Vec3 point = GetNearestPointOnAABB3D( centerSphere, aabb );

	return IsPointInsideSphere3D( point, centerSphere, radiusSphere );
}


//-----------------------------------------------------------------------------------------------
bool DoAABB3sOverlap( AABB3 const& first, AABB3 const& second )
{
	if ( first.m_maxs.x <= second.m_mins.x )
		return false;

	if ( second.m_maxs.x <= first.m_mins.x )
		return false;

	if ( first.m_maxs.y <= second.m_mins.y )
		return false;

	if ( second.m_maxs.y <= first.m_mins.y )
		return false;

	if ( first.m_maxs.z <= second.m_mins.z )
		return false;

	if ( second.m_maxs.z <= first.m_mins.z )
		return false;

	return true;
}


//-----------------------------------------------------------------------------------------------
bool DoCylindersZOverlap( Vec2 const& center1XY, float radius1, FloatRange const& height1, Vec2 const& center2XY, float radius2, FloatRange const& height2 )
{
	if ( !DoDiscsOverlap( center1XY, radius1, center2XY, radius2 ) )
		return false;

	if ( height1.m_max <= height2.m_min )
		return false;

	if ( height2.m_max <= height1.m_min )
		return false;

	return true;
}


//-----------------------------------------------------------------------------------------------
bool DoesCylindersZAndAABB3Overlap( Vec2 const& centerXY, float radius, FloatRange const& height, AABB3 const& aabb )
{
	if ( !DoesAABB2AndDiscOverlap( centerXY, radius, AABB2( aabb.m_mins.x, aabb.m_mins.y, aabb.m_maxs.x, aabb.m_maxs.y ) ) )
	{
		return false;
	}

	if ( height.m_max <= aabb.m_mins.z )
		return false;

	if ( aabb.m_maxs.z <= height.m_min )
		return false;

	return true;
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius )
{
	if ( IsPointInsideDisc2D( referencePos, discCenter, discRadius ) )
		return referencePos;
	// manually do the math instead of duplicate math
	Vec2 differnceVector = referencePos - discCenter;
	Vec2 directionVector = differnceVector.GetNormalized();
	
	return discCenter + discRadius * directionVector;
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnAABB2D( Vec2 const& referencePos, AABB2 const& box )
{
	return box.GetNearestPoint( referencePos );
}

//-----------------------------------------------------------------------------------------------
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius )
{
	if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
		return false;
	//vector math - 
	Vec2 centerToPointVector = point - sectorTip;
	float angleOfVector = centerToPointVector.GetOrientationDegrees();
	float angleDifference = GetShortestAngularDispDegrees( angleOfVector, sectorForwardDegrees );
	
	if ( angleDifference < 0.f )
		angleDifference *= -1.f;
	
	if ( angleDifference < sectorApertureDegrees * 0.5f )
		return true;
	
	return false;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius )
{
	if ( !IsPointInsideDisc2D( point, sectorTip, sectorRadius ) )
		return false;
	
	Vec2 centerToPointVector = point - sectorTip;
	float angleBetweenForwardNormalAndPointVector = GetAngleDegreesBetweenVectors2D( centerToPointVector, sectorForwardNormal );
	
	if ( angleBetweenForwardNormalAndPointVector < sectorApertureDegrees * 0.5f )
		return true;
	
	return false;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideAABB2D( Vec2 const& point, AABB2 const& box )
{
	return box.IsPointInside( point );
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideCapsule2D( Vec2 const& point, Capsule2 const& capsule )
{
	return IsPointInsideCapsule2D( point, capsule.m_bone.m_start, capsule.m_bone.m_end, capsule.m_radius );
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideCapsule2D( Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	Vec2 nearestPointOnBone = GetNearestPointOnLineSegment2D( point, boneStart, boneEnd );
	return IsPointInsideDisc2D( point, nearestPointOnBone, radius );
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideOBB2D( Vec2 const& point, OBB2 const& orientedBox )
{

	Vec2 vectorFromCenterToPoint = point - orientedBox.m_center;
	Vec2 iBasis = orientedBox.m_iBasisNormal;
	Vec2 jBasis = orientedBox.m_iBasisNormal.GetRotated90Degrees();
	float halfWidth = orientedBox.m_halfDimensions.x;
	float halfHeight = orientedBox.m_halfDimensions.y;

	float pointI = DotProduct2D( vectorFromCenterToPoint, iBasis );
	float pointJ = DotProduct2D( vectorFromCenterToPoint, jBasis );

	return ( pointI > -halfWidth && pointI < halfWidth&& pointJ > -halfHeight && pointJ < halfHeight );
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideSphere3D( Vec3 const& point, Vec3 const& center, float radius )
{
	if ( GetDistanceSquared3D( point, center ) < radius * radius )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool IsPointInsideAABB3D( Vec3 const& point, AABB3 const& box )
{

	return ( box.m_mins.x < point.x ) &&
		   ( box.m_mins.y < point.y ) &&
		   ( box.m_mins.z < point.z ) &&
		   ( box.m_maxs.x > point.x ) &&
		   ( box.m_maxs.y > point.y ) &&
		   ( box.m_maxs.z > point.z );
}		   


//-----------------------------------------------------------------------------------------------
bool IsPointInsideZCylinder( Vec3 const& point, Vec2 const& centerXY, float radius, FloatRange const& height )
{
	return ( IsPointInsideDisc2D( Vec2( point.x, point.y ), centerXY, radius ) &&
			 point.z < height.m_max&&
			 point.z > height.m_min );
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, LineSegment2 const& infiniteLine )
{
	return GetNearestPointOnInfiniteLine2D( referencePos, infiniteLine.m_start, infiniteLine.m_end );
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine )
{
	Vec2 startToEndVector = anotherPointOnLine - pointOnLine;
	Vec2 startToPointVector = referencePos - pointOnLine;
	Vec2 startToNearestPointVector = GetProjectedOnto2D( startToPointVector, startToEndVector );

	return pointOnLine + startToNearestPointVector;
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnLineSegment2D( Vec2 const& referencePos, LineSegment2 const& lineSegment )
{
	return GetNearestPointOnLineSegment2D( referencePos, lineSegment.m_start, lineSegment.m_end );
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnLineSegment2D( Vec2 const& referencePos, Vec2 const& lineStart, Vec2 const& lineEnd )
{
	Vec2 startToEndVector = lineEnd - lineStart;
	Vec2 endToPosVector = referencePos - lineEnd;

	if ( DotProduct2D( startToEndVector, endToPosVector ) >= 0.0f )
	{
		return lineEnd;
	}

	Vec2 startToPosVector = referencePos - lineStart;

	if ( DotProduct2D( startToEndVector, startToPosVector ) <= 0.0f )
	{
		return lineStart;
	}

	return GetNearestPointOnInfiniteLine2D( referencePos, lineStart, lineEnd );
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnCapsule2D( Vec2 const& referencePos, Capsule2 const& capsule )
{
	return GetNearestPointOnCapsule2D( referencePos, capsule.m_bone.m_start, capsule.m_bone.m_end, capsule.m_radius );
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnCapsule2D( Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius )
{
	Vec2 nearestPointOnBone = GetNearestPointOnLineSegment2D( referencePos, boneStart, boneEnd );
	return GetNearestPointOnDisc2D( referencePos, nearestPointOnBone, radius );
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnOBB2D( Vec2 const& referencePos, OBB2 const& orientedBox )
{
	Vec2 vectorFromCenterToPoint = referencePos - orientedBox.m_center;
	Vec2 iBasis = orientedBox.m_iBasisNormal;
	Vec2 jBasis = orientedBox.m_iBasisNormal.GetRotated90Degrees();
	
	float halfWidth = orientedBox.m_halfDimensions.x;
	float halfHeight = orientedBox.m_halfDimensions.y;
	float pointI = DotProduct2D( vectorFromCenterToPoint, iBasis );
	float pointJ = DotProduct2D( vectorFromCenterToPoint, jBasis );
	
	pointI = Clamp( pointI, -halfWidth, halfWidth );
	pointJ = Clamp( pointJ, -halfHeight, halfHeight );

	return orientedBox.m_center + pointI * iBasis + pointJ * jBasis;
}


//-----------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnAABB3D( Vec3 const& referencePos, AABB3 const& box )
{
	Vec3 nearestPoint;

	nearestPoint.x = Clamp( referencePos.x, box.m_mins.x, box.m_maxs.x );
	nearestPoint.y = Clamp( referencePos.y, box.m_mins.y, box.m_maxs.y );
	nearestPoint.z = Clamp( referencePos.z, box.m_mins.z, box.m_maxs.z );

	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnSphere3D( Vec3 const& referencePos, Vec3 const& sphereCenter, float sphereRadius )
{
	if ( GetDistanceSquared3D( referencePos, sphereCenter ) < sphereRadius * sphereRadius )
		return referencePos;
//duplicate math
	Vec3 centerToPointVector = referencePos - sphereCenter;

	Vec3 nearestPoint;
	nearestPoint = sphereCenter + centerToPointVector.GetClamped( sphereRadius );

	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnCylinder3D( Vec3 const& referencePos, Vec2 const& cylinderCenter, float cylinderRadius, FloatRange const& cylinderMinMaxZ )
{
	Vec3 nearestPoint( GetNearestPointOnDisc2D( Vec2( referencePos.x, referencePos.y ), cylinderCenter, cylinderRadius ) );

	nearestPoint.z = Clamp( referencePos.z, cylinderMinMaxZ.m_min, cylinderMinMaxZ.m_max );
	
	return nearestPoint;
}


//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint )
{
	if ( !IsPointInsideDisc2D( fixedPoint, mobileDiscCenter, discRadius ) )
		return false;
	
	float distanceBetweenPointAndCenter = GetDistance2D( mobileDiscCenter, fixedPoint );
	
	Vec2 directionVector = ( mobileDiscCenter - fixedPoint ).GetNormalized();
	mobileDiscCenter += ( discRadius - distanceBetweenPointAndCenter ) * directionVector;
	
	return true;
}


//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfDisc2D( Vec2& mobileDiscCenter, float mobileRadius, Vec2 const& fixedDiscCenter, float fixedRadius )
{
	if ( !DoDiscsOverlap( mobileDiscCenter, mobileRadius, fixedDiscCenter, fixedRadius ) )
		return false;
	
	float distanceBetweenCenters = GetDistance2D( mobileDiscCenter, fixedDiscCenter );
	float pushDistance = mobileRadius + fixedRadius - distanceBetweenCenters;
	
	Vec2 directionVector = ( mobileDiscCenter - fixedDiscCenter ).GetNormalized();
	mobileDiscCenter += pushDistance * directionVector;
	
	return true;
}


//-----------------------------------------------------------------------------------------------
bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius )
{
	if ( !DoDiscsOverlap( aCenter, aRadius, bCenter, bRadius ) )
		return false;
	
	float distanceBetweenCenters = GetDistance2D( aCenter, bCenter );
	float pushDistance = ( aRadius + bRadius - distanceBetweenCenters ) * 0.5f;
	
	Vec2 directionVectorBToA = ( aCenter - bCenter ).GetNormalized();
	aCenter += pushDistance * directionVectorBToA;
	bCenter -= pushDistance * directionVectorBToA;
	
	return true;
}


//-----------------------------------------------------------------------------------------------
bool PushDiscOutOfAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox )
{
	Vec2 closestPoint = fixedBox.GetNearestPoint( mobileDiscCenter );
	
	if ( !IsPointInsideDisc2D( closestPoint, mobileDiscCenter, discRadius ) )
		return false;
	
	float distanceBetweenPointAndCenter = GetDistance2D( mobileDiscCenter, closestPoint );
	Vec2 directionVector = ( mobileDiscCenter - closestPoint ).GetNormalized();
	mobileDiscCenter += ( discRadius - distanceBetweenPointAndCenter ) * directionVector;
	
	return true;
}


//-----------------------------------------------------------------------------------------------
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation )
{
	float vertex_X = uniformScale * posToTransform.x;
	float vertex_Y = uniformScale * posToTransform.y;
	
	float polarCoordsRadius = sqrtf( vertex_X * vertex_X + vertex_Y * vertex_Y );
	float polarCoordsAngle = Atan2Degrees( vertex_Y, vertex_X );
	
	vertex_X = polarCoordsRadius * CosDegrees( polarCoordsAngle + rotationDegrees );
	vertex_Y = polarCoordsRadius * SinDegrees( polarCoordsAngle + rotationDegrees );
	posToTransform = Vec2( vertex_X + translation.x, vertex_Y + translation.y );
}


//-----------------------------------------------------------------------------------------------
void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
	posToTransform = posToTransform.x * iBasis + posToTransform.y * jBasis + translation;
}


//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D( Vec3& positionToTransform, float scaleXY, float zRotationDegrees, Vec2 const& translationXY )
{
	float vertex_X = scaleXY * positionToTransform.x;
	float vertex_Y = scaleXY * positionToTransform.y;
	float polarCoordsRadius = sqrtf( vertex_X * vertex_X + vertex_Y * vertex_Y );
	float polarCoordsAngle = Atan2Degrees( vertex_Y, vertex_X );
	
	vertex_X = polarCoordsRadius * CosDegrees( polarCoordsAngle + zRotationDegrees );
	vertex_Y = polarCoordsRadius * SinDegrees( polarCoordsAngle + zRotationDegrees );
	positionToTransform = Vec3( vertex_X + translationXY.x, vertex_Y + translationXY.y, positionToTransform.z );
}


//-----------------------------------------------------------------------------------------------
void TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation )
{
	Vec2 newVector = posToTransform.x * iBasis + posToTransform.y * jBasis + translation;
	posToTransform.x = newVector.x;
	posToTransform.y = newVector.y;
}


//-----------------------------------------------------------------------------------------------
float Interpolate( float begin, float end, float t )
{
	float value = begin + t * ( end - begin );
	return value;
}


//-----------------------------------------------------------------------------------------------
float ClampZeroToOne( float value )
{
	if ( value < 0.0f )
	{
		value = 0.0f;
	}

	else if ( value > 1.0f )
	{
		value = 1.0f;
	}

	return value;
}


//-----------------------------------------------------------------------------------------------
float GetFractionWithin( float value, float begin, float end )
{
	if ( begin == end )
		return begin;

	float t = ( value - begin ) / ( end - begin );
	return t;
}


//-----------------------------------------------------------------------------------------------
float RangeMap( float inValue, float inBegin, float inEnd, float outBegin, float outEnd )
{
	float t = GetFractionWithin( inValue, inBegin, inEnd );
	float outValue = Interpolate( outBegin, outEnd, t );
	return outValue;
}


//-----------------------------------------------------------------------------------------------
float RangeMapClamped( float inValue, float inBegin, float inEnd, float outBegin, float outEnd )
{
	float t = GetFractionWithin( inValue, inBegin, inEnd );
	float clampedT = ClampZeroToOne( t );
	float outValue = Interpolate( outBegin, outEnd, clampedT );
	return outValue;
}


//-----------------------------------------------------------------------------------------------
float Clamp( float value, float begin, float end )
{
	if ( value > end )
		return end;
	if ( value < begin )
		return begin;
	return value;
}


//-----------------------------------------------------------------------------------------------
int RoundDownToInt( float value )
{
	if ( value >= 0 )
		return static_cast< int >( value );
	return static_cast< int >( value ) - 1;

}


//-----------------------------------------------------------------------------------------------
float NormalizeByte( unsigned char byteValue )
{
	float byteValInFloat = static_cast< float >( byteValue );
	return RangeMap( byteValInFloat, 0.0f, 255.0f, 0.0f, 1.0f );
}


//-----------------------------------------------------------------------------------------------
unsigned char DenormalizeByte( float zeroToOne )
{
	float denormalizedByteVal = RangeMap( zeroToOne, 0.0f, 1.0f, 0.0f, 255.0f );
	float bucketedValue = denormalizedByteVal + static_cast< float >( static_cast< unsigned char >( denormalizedByteVal ) ) / 256.f;
	return static_cast< unsigned char >( bucketedValue );
}


//-----------------------------------------------------------------------------------------------
float SmoothStep3( float t )
{
	return ( 3.f * t * t ) - ( 2.f * t * t * t );
}


//-----------------------------------------------------------------------------------------------
int GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB )
{
	return abs( pointB.x - pointA.x ) + abs( pointB.y - pointA.y );
}


//-----------------------------------------------------------------------------------------------
float DotProduct2D( Vec2 const& A, Vec2 const& B )
{
	return ( A.x * B.x ) + ( A.y * B.y );
}


//-----------------------------------------------------------------------------------------------
float DotProduct3D( Vec3 const& A, Vec3 const& B )
{
	return ( A.x * B.x ) + ( A.y * B.y ) + ( A.z * B.z );
}


//-----------------------------------------------------------------------------------------------
float DotProduct4D( Vec4 const& A, Vec4 const& B )
{
	return ( A.x * B.x ) + ( A.y * B.y ) + ( A.z * B.z ) + ( A.w * B.w );
}


//-----------------------------------------------------------------------------------------------
float CrossProduct2D( Vec2 const& A, Vec2 const& B )
{
	return ( A.x * B.y ) - ( A.y * B.x );
}


//-----------------------------------------------------------------------------------------------
Vec3 CrossProduct3D( Vec3 const& A, Vec3 const& B )
{
	Vec3 crossProduct3DResult;

	crossProduct3DResult.x = ( A.y * B.z ) - ( A.z * B.y );
	crossProduct3DResult.y = ( A.z * B.x ) - ( A.x * B.z );
	crossProduct3DResult.z = ( A.x * B.y ) - ( A.y * B.x );

	return crossProduct3DResult;
}


//-----------------------------------------------------------------------------------------------
float GetShortestAngularDispDegrees( float startAngle, float endAngle )
{
	float angDisp = endAngle - startAngle;

	while ( angDisp > 180.f )
	{
		angDisp -= 360.f;
	}

	while ( angDisp < -180.f )
	{
		angDisp += 360.f;
	}
	return angDisp;
}


//-----------------------------------------------------------------------------------------------
float GetTurnedTowardDegrees( float currentAngle, float goalAngle, float maxDeltaDegrees )
{
	float angDisp = GetShortestAngularDispDegrees( currentAngle, goalAngle );

	if ( angDisp < maxDeltaDegrees && angDisp > -maxDeltaDegrees )
	{
		return goalAngle;
	}

	if ( angDisp < 0.f )
	{
		return currentAngle - maxDeltaDegrees;
	}

	return currentAngle + maxDeltaDegrees;
}


//-----------------------------------------------------------------------------------------------
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto )
{
	return DotProduct2D( vectorToProject, vectorToProjectOnto.GetNormalized() );
}


//-----------------------------------------------------------------------------------------------
Vec2 const GetProjectedOnto2D( Vec2 const& a, Vec2 const& b )
{
	float dotProductOfAB = DotProduct2D( a, b );
	return dotProductOfAB * b / b.GetLengthSquared();
}


//-----------------------------------------------------------------------------------------------
float GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b )
{
	float dotProductOfAB = DotProduct2D( a, b );

	float lengthofA = a.GetLength();
	float lengthofB = b.GetLength();

	if ( lengthofA <= 0.000001f || lengthofB <= 0.000001f )
		return 0.0f;

	float angleInRadians = acosf( dotProductOfAB / ( a.GetLength() * b.GetLength() ) );

	return ConvertRadiansToDegrees( angleInRadians );
}


//-----------------------------------------------------------------------------------------------
bool DoVelocitiesNeedModification( Vec2 const& velocityA, Vec2 const& velocityB, Vec2 const& normal )
{
	float Vna = DotProduct2D( normal, velocityA );
	float Vnb = DotProduct2D( normal, velocityB );

	if ( Vnb - Vna > 0 )
		return false;

	return true;
}


//-----------------------------------------------------------------------------------------------
void HandleDiscVsDiscCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Vec2& centerB, float radiusB, Vec2& velocityB, float elasticityB )
{
	if ( !DoDiscsOverlap( centerA, radiusA, centerB, radiusB ) )
	{
		return;
	}
	
	PushDiscsOutOfEachOther2D( centerA, radiusA, centerB, radiusB );

	Vec2 normalAB = centerB - centerA;
	normalAB.Normalize();

	if ( DoVelocitiesNeedModification( velocityA, velocityB, normalAB ) )
	{
		float elasticity = elasticityA * elasticityB;
		
		float Vna = DotProduct2D( normalAB, velocityA );
		Vec2 VnormalA = Vna * normalAB;

		Vec2 Vta = velocityA - VnormalA;

		float Vnb = DotProduct2D( normalAB, velocityB );
		Vec2 VnormalB = Vnb * normalAB;

		Vec2 Vtb = velocityB - VnormalB;

		Vec2 Vaftera = Vta + VnormalB * elasticity;
		Vec2 Vafterb = Vtb + VnormalA * elasticity;
		
		velocityA = Vaftera;
		velocityB = Vafterb;
	}
}


//-----------------------------------------------------------------------------------------------
void HandleDiscVsImmobilePointCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Vec2 const& pointB, float elasticityPoint )
{
	HandleDiscVsImmobileDiscCollision( centerA, radiusA, velocityA, elasticityA, pointB, 0.0f, elasticityPoint );
}


//-----------------------------------------------------------------------------------------------
void HandleDiscVsImmobileDiscCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Vec2 const& constCenterB, float radiusB, float elasticityB )
{
	if ( !DoDiscsOverlap( centerA, radiusA, constCenterB, radiusB ) )
	{
		return;
	}

	PushDiscOutOfDisc2D( centerA, radiusA, constCenterB, radiusB );

	Vec2 normal = constCenterB - centerA;
	normal.Normalize();
	
	float Vn = DotProduct2D( normal, velocityA );
	Vec2 Vnormal = Vn * normal;

	Vec2 Vt = velocityA - Vnormal;
	
	Vec2 Vafter = Vt - ( Vnormal * elasticityA * elasticityB );
	
	velocityA = Vafter;
}


//-----------------------------------------------------------------------------------------------
void HandleDiscVsImmobileCapsuleCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Capsule2 const& capsule, float elasticityB )
{
	Vec2 pointOnBone = GetNearestPointOnLineSegment2D( centerA, capsule.m_bone );
	HandleDiscVsImmobileDiscCollision( centerA, radiusA, velocityA, elasticityA, pointOnBone, capsule.m_radius, elasticityB );
}


//-----------------------------------------------------------------------------------------------
void HandleDiscVsImmobileOBBCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, OBB2 const& obb, float elasticityB )
{
	Vec2 pointOnOBB = GetNearestPointOnOBB2D( centerA, obb);
	HandleDiscVsImmobilePointCollision( centerA, radiusA, velocityA, elasticityA, pointOnOBB, elasticityB );
}


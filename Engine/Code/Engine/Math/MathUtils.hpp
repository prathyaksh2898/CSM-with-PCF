#pragma once

struct Vec2;
struct Vec3;
struct Vec4;
struct IntVec2;
struct AABB2;
struct AABB3;
struct OBB2;
struct Capsule2;
struct EulerAngles;
struct FloatRange;
struct LineSegment2;


//-----------------------------------------------------------------------------------------------

// Angle Utilities
float ConvertRadiansToDegrees( float radians );
float ConvertDegreesToRadians( float degrees );
float SinDegrees( float degrees );
float CosDegrees( float degrees );
float Atan2Degrees( float y, float x );

// Basic 2D & 3D utilities
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB );

//Geometric query utilities
bool DoSpheresOverlap( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB );
bool DoesSphereAndCylinderOverlap( Vec3 const& centerSphere, float radiusSphere, Vec2 const& centerCylinder, float radiusCylinder, FloatRange const& heightCylinder );
bool DoesSphereAndAABB3Overlap( Vec3 const& centerSphere, float radiusSphere, AABB3 aabb );
bool DoAABB3sOverlap( AABB3 const& first, AABB3 const& second );
bool DoCylindersZOverlap( Vec2 const& center1XY, float radius1, FloatRange const& height1, Vec2 const& center2XY, float radius2, FloatRange const& height2 );
bool DoesCylindersZAndAABB3Overlap( Vec2 const& centerXY, float radius, FloatRange const& height, AABB3 const& aabb );

bool DoDiscsOverlap( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB );
bool DoAABBsOverlap( AABB2 const& first, AABB2 const& second );
bool DoesAABB2AndDiscOverlap( Vec2 const& center, float radius, AABB2 const& aabb );

bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorRadius, float sectorApertureDegrees );
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorRadius, float sectorApertureDegrees );
bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius );
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius );
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius );
bool IsPointInsideAABB2D( Vec2 const& point, AABB2 const& box );
bool IsPointInsideCapsule2D( Vec2 const& point, Capsule2 const& capsule );
bool IsPointInsideCapsule2D( Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius );
bool IsPointInsideOBB2D( Vec2 const& point, OBB2 const& orientedBox );

bool IsPointInsideSphere3D( Vec3 const& point, Vec3 const& center, float radius );
bool IsPointInsideAABB3D( Vec3 const& point, AABB3 const& box );
bool IsPointInsideZCylinder( Vec3 const& point, Vec2 const& centerXY, float radius, FloatRange const& height );

Vec2 const GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
Vec2 const GetNearestPointOnDisc2D( Vec2 const& referencePos, Vec2 const& discCenter, float discRadius );
Vec2 const GetNearestPointOnAABB2D( Vec2 const& referencePos, AABB2 const& box );
Vec2 const GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, LineSegment2 const& infiniteLine );
Vec2 const GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine );
Vec2 const GetNearestPointOnLineSegment2D( Vec2 const& referencePos, LineSegment2 const& lineSegment );
Vec2 const GetNearestPointOnLineSegment2D( Vec2 const& referencePos, Vec2 const& lineStart, Vec2 const& lineEnd );
Vec2 const GetNearestPointOnCapsule2D( Vec2 const& referencePos, Capsule2 const& capsule );
Vec2 const GetNearestPointOnCapsule2D( Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius );
Vec2 const GetNearestPointOnOBB2D( Vec2 const& referencePos, OBB2 const& orientedBox );

Vec3 const GetNearestPointOnAABB3D( Vec3 const& referencePos, AABB3 const& box );
Vec3 const GetNearestPointOnSphere3D( Vec3 const& referencePos, Vec3 const& sphereCenter, float sphereRadius );
Vec3 const GetNearestPointOnCylinder3D( Vec3 const& referencePos, Vec2 const& cylinderCenter, float cylinderRadius, FloatRange const& cylinderMinMaxZ );

bool PushDiscOutOfPoint2D( Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint );
bool PushDiscOutOfDisc2D( Vec2& mobileDiscCenter, float mobileRadius, Vec2 const& fixedDiscCenter, float fixedRadius );
bool PushDiscsOutOfEachOther2D( Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius );
bool PushDiscOutOfAABB2D( Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox );

//Transform utilities
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation );
void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );
void TransformPositionXY3D( Vec3& positionToTransform, float scaleXY, float zRotationDegrees, Vec2 const& translationXY );
void TransformPositionXY3D( Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );

//Conversion utilities
float         Interpolate( float begin, float end, float t );
float         ClampZeroToOne( float value );
float         Clamp( float value, float begin, float end );
float         GetFractionWithin( float value, float begin, float end );
float         RangeMap( float inValue, float inBegin, float inEnd, float outBegin, float outEnd );
float         RangeMapClamped( float inValue, float inBegin, float inEnd, float outBegin, float outEnd );
int           RoundDownToInt( float value );
float         NormalizeByte( unsigned char byteValue );
unsigned char DenormalizeByte( float zeroToOne );
float         SmoothStep3( float t );

//Geometric utilities
int   GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB );
float DotProduct2D( Vec2 const& A, Vec2 const& B );
float DotProduct3D( Vec3 const& A, Vec3 const& B );
float DotProduct4D( Vec4 const& A, Vec4 const& B );
float CrossProduct2D( Vec2 const& A, Vec2 const& B );
Vec3  CrossProduct3D( Vec3 const& A, Vec3 const& B );

float      GetShortestAngularDispDegrees( float startAngle, float endAngle );
float      GetTurnedTowardDegrees( float currentAngle, float goalAngle, float maxDeltaDegrees );
float      GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );
Vec2 const GetProjectedOnto2D( Vec2 const& a, Vec2 const& b );
float      GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b );

bool       DoVelocitiesNeedModification( Vec2 const& velocityA, Vec2 const& velocityB, Vec2 const& normal );
void       HandleDiscVsDiscCollision(Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Vec2& centerB, float radiusB, Vec2& velocityB, float elasticityB );
void       HandleDiscVsImmobilePointCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Vec2 const& pointB, float elasticityPoint );
void       HandleDiscVsImmobileDiscCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Vec2 const& constCenterB, float radiusB, float elasticityB );
void       HandleDiscVsImmobileCapsuleCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, Capsule2 const& capsule, float elasticityB );
void       HandleDiscVsImmobileOBBCollision( Vec2& centerA, float radiusA, Vec2& velocityA, float elasticityA, OBB2 const& obb, float elasticityB );


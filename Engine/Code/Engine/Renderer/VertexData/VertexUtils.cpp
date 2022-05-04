#include "VertexUtils.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Mat44.hpp"

#include <math.h>


//-------------------------------------------------------------------------------------------------
void TransformVertexArray2D( int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotatingDegreesAboutZ, Vec2 const& translationXY )
{
	for ( int vertexIndex = 0; vertexIndex < numVerts; vertexIndex++ )
	{
		Vertex_PCU* vertex = &verts[ vertexIndex ];
		float vertex_X = uniformScaleXY * vertex->m_position.x;
		float vertex_Y = uniformScaleXY * vertex->m_position.y;
		float polarCoordsRadius = sqrtf( vertex_X * vertex_X + vertex_Y * vertex_Y );
		float polarCoordsAngle = atan2f( vertex_Y, vertex_X );
		float degreesPolarAngle = ConvertRadiansToDegrees( polarCoordsAngle );
		vertex_X = polarCoordsRadius * CosDegrees( degreesPolarAngle + rotatingDegreesAboutZ );
		vertex_Y = polarCoordsRadius * SinDegrees( degreesPolarAngle + rotatingDegreesAboutZ );
		vertex->m_position = Vec3( vertex_X + translationXY.x, vertex_Y + translationXY.y, 0 );
	}
}


//-------------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D( int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotatingDegreesAboutZ, Vec2 const& translationXY )
{
	for ( int vertNum = 0; vertNum < numVerts; vertNum++ )
	{
		TransformPositionXY3D( verts[ vertNum ].m_position, uniformScaleXY, rotatingDegreesAboutZ, translationXY );
	}
}


//-------------------------------------------------------------------------------------------------
void TransformVertexArrayPositionsUsingMatrix( int numVerts, Vertex_PCU* verts, Mat44 const& transformMatrix )
{
	for ( int vertNum = 0; vertNum < numVerts; vertNum++ )
	{
		verts[ vertNum ].m_position = transformMatrix.TransformPosition3D( verts[ vertNum ].m_position );
	}
}


//-------------------------------------------------------------------------------------------------
void TransformVertexArrayUsingMatrix( int numVerts, Vertex_PCU* verts, Mat44 const& transformMatrix )
{
	for ( int vertNum = 0; vertNum < numVerts; vertNum++ )
	{
		verts[ vertNum ].m_position = transformMatrix.TransformVectorQuantity3D( verts[ vertNum ].m_position );
	}
}


//-------------------------------------------------------------------------------------------------
void TransformVertexArrayUsingMatrix( int numVerts, Vertex_PCUTBN* verts, Mat44 const& transformMatrix )
{
	for ( int vertNum = 0; vertNum < numVerts; vertNum++ )
	{
		verts[ vertNum ].m_position = transformMatrix.TransformVectorQuantity3D( verts[ vertNum ].m_position );
	}
}


//-------------------------------------------------------------------------------------------------
void TransformVertexArrayPositionsUsingMatrix( int numVerts, Vertex_PCUTBN* verts, Mat44 const& transformMatrix )
{
	for ( int vertNum = 0; vertNum < numVerts; vertNum++ )
	{
		verts[ vertNum ].m_position = transformMatrix.TransformPosition3D( verts[ vertNum ].m_position );
	}
}


//-------------------------------------------------------------------------------------------------
void AddVertsForCapsule2D( std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color )
{
	AddVertsForCapsule2D( verts, capsule.m_bone.m_start, capsule.m_bone.m_end, capsule.m_radius, color );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForCapsule2D( std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color )
{
	constexpr int noOfSides = 32;
	constexpr int noOfVerts = 3 * noOfSides;
	constexpr float degreesPerSide = 180.f / static_cast< float >( noOfSides );

	verts.reserve( verts.size() + noOfVerts * 2 + 12 );

	Vec2 startToEndVector = boneEnd - boneStart;
	Vec2 startToEndVectorNormalized = startToEndVector.GetNormalized();
	Vec2 orthogonalToStartToEndVector = startToEndVectorNormalized.GetRotated90Degrees();
	float semiCircleStartAngle = orthogonalToStartToEndVector.GetOrientationDegrees();
	std::vector<Vec2> sideEndVerts1;
	std::vector<Vec2> sideEndVerts2;

	Vertex_PCU tempPoint;
	tempPoint.m_color = color;
	tempPoint.m_uvTexCoords = Vec2( 0, 0 );

	for ( int sideIndex = 0; sideIndex <= noOfSides; ++sideIndex )
	{
		float degrees = semiCircleStartAngle + degreesPerSide * static_cast< float > ( sideIndex );
		Vec2 sideCoordinate = boneStart + Vec2( radius * CosDegrees( degrees ), radius * SinDegrees( degrees ) );
		sideEndVerts1.push_back( sideCoordinate );
	}


	for ( int sideIndex = 0; sideIndex < noOfSides; ++sideIndex )
	{
		int nextSide = ( sideIndex + 1 );
		tempPoint.m_position = Vec3( boneStart );
		verts.push_back( tempPoint );

		tempPoint.m_position = Vec3( sideEndVerts1[ sideIndex ] );
		verts.push_back( tempPoint );

		tempPoint.m_position = Vec3( sideEndVerts1[ nextSide ] );
		verts.push_back( tempPoint );
	}


	for ( int sideIndex = 0; sideIndex <= noOfSides; ++sideIndex )
	{
		float degrees = semiCircleStartAngle - degreesPerSide * static_cast< float > ( sideIndex );
		Vec2 sideCoordinate = boneEnd + Vec2( radius * CosDegrees( degrees ), radius * SinDegrees( degrees ) );
		sideEndVerts2.push_back( sideCoordinate );
	}


	for ( int sideIndex = 0; sideIndex < noOfSides; ++sideIndex )
	{
		int nextSide = ( sideIndex + 1 );
		tempPoint.m_position = Vec3( boneEnd );
		verts.push_back( tempPoint );

		tempPoint.m_position = Vec3( sideEndVerts2[ nextSide ] );
		verts.push_back( tempPoint );

		tempPoint.m_position = Vec3( sideEndVerts2[ sideIndex ] );
		verts.push_back( tempPoint );

	}

	Vec2 capsuleStartLeft = boneStart + orthogonalToStartToEndVector * radius;
	Vec2 capsuleStartRight = boneStart - orthogonalToStartToEndVector * radius;
	Vec2 capsuleEndLeft = boneEnd + orthogonalToStartToEndVector * radius;
	Vec2 capsuleEndRight = boneEnd - orthogonalToStartToEndVector * radius;

	tempPoint.m_position = Vec3( boneStart );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( capsuleEndLeft );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( capsuleStartLeft );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( boneStart );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( boneEnd );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( capsuleEndLeft );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( boneStart );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( capsuleEndRight );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( boneEnd );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( boneStart );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( capsuleStartRight );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( capsuleEndRight );
	verts.push_back( tempPoint );

}


//-------------------------------------------------------------------------------------------------
void AddVertsForDisc2D( std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color )
{
	constexpr int noOfSides = 64;
	constexpr int noOfVerts = 3 * noOfSides;
	constexpr float degreesPerSide = 360.f / static_cast< float >( noOfSides );

	std::vector<Vec2> sideEndVerts;

	verts.reserve( verts.size() + noOfVerts );

	for ( int sideIndex = 0; sideIndex < noOfSides; ++sideIndex )
	{
		float degrees = degreesPerSide * static_cast< float > ( sideIndex );
		Vec2 sideCoordinate = center + Vec2( radius * CosDegrees( degrees ), radius * SinDegrees( degrees ) );
		sideEndVerts.push_back( sideCoordinate );
	}

	Vertex_PCU tempPoint;
	tempPoint.m_color = color;
	tempPoint.m_uvTexCoords = Vec2( 0, 0 );

	for ( int sideIndex = 0; sideIndex < noOfSides; ++sideIndex )
	{
		int nextSide = ( sideIndex + 1 ) % noOfSides;
		tempPoint.m_position = Vec3( center );
		verts.push_back( tempPoint );

		tempPoint.m_position = Vec3( sideEndVerts[ sideIndex ] );
		verts.push_back( tempPoint );

		tempPoint.m_position = Vec3( sideEndVerts[ nextSide ] );
		verts.push_back( tempPoint );
	}
}


//-------------------------------------------------------------------------------------------------
void AddVertsForOBB2D( std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color )
{
	verts.reserve( verts.size() + 6 );

	Vec2 iBasis = box.m_iBasisNormal;
	Vec2 jBasis = box.m_iBasisNormal.GetRotated90Degrees();

	Vec2 topRight = box.m_center;
	topRight += box.m_halfDimensions.x * iBasis;
	topRight += box.m_halfDimensions.y * jBasis;

	Vec2 topLeft = box.m_center;
	topLeft -= box.m_halfDimensions.x * iBasis;
	topLeft += box.m_halfDimensions.y * jBasis;

	Vec2 bottomLeft = box.m_center;
	bottomLeft -= box.m_halfDimensions.x * iBasis;
	bottomLeft -= box.m_halfDimensions.y * jBasis;

	Vec2 bottomRight = box.m_center;
	bottomRight += box.m_halfDimensions.x * iBasis;
	bottomRight -= box.m_halfDimensions.y * jBasis;

	Vertex_PCU tempPoint;
	tempPoint.m_color = color;
	tempPoint.m_uvTexCoords = Vec2( 0, 0 );

	tempPoint.m_position = Vec3( topRight );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( topLeft );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( bottomLeft );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( bottomLeft );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( bottomRight );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( topRight );
	verts.push_back( tempPoint );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForAABB2D( std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, AABB2 const& uvBounds )
{
	constexpr int NUM_VERTS = 6;
	verts.reserve( verts.size() + NUM_VERTS );

	Vec3 BottomLeft = Vec3( bounds.m_mins.x, bounds.m_mins.y, 0.f );
	Vec3 BottomRight = Vec3( bounds.m_maxs.x, bounds.m_mins.y, 0.f );
	Vec3 TopRight = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, 0.f );
	Vec3 TopLeft = Vec3( bounds.m_mins.x, bounds.m_maxs.y, 0.f );

	Vec2 bottomLeftUV = uvBounds.m_mins;
	Vec2 bottomRightUV = Vec2( uvBounds.m_maxs.x, uvBounds.m_mins.y );
	Vec2 topLeftUV = Vec2( uvBounds.m_mins.x, uvBounds.m_maxs.y );
	Vec2 topRightUV = uvBounds.m_maxs;

	verts.push_back( Vertex_PCU( BottomLeft, color, bottomLeftUV ) );
	verts.push_back( Vertex_PCU( BottomRight, color, bottomRightUV ) );
	verts.push_back( Vertex_PCU( TopRight, color, topRightUV ) );

	verts.push_back( Vertex_PCU( BottomLeft, color, bottomLeftUV ) );
	verts.push_back( Vertex_PCU( TopRight, color, topRightUV ) );
	verts.push_back( Vertex_PCU( TopLeft, color, topLeftUV ) );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D( std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color )
{
	AddVertsForLineSegment2D( verts, lineSegment.m_start, lineSegment.m_end, thickness, color );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D( std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color )
{
	float halfThickness = thickness * 0.5f;

	Vec2 startToEndVector = end - start;
	float length = startToEndVector.GetLength();
	Vec2 startToEndDirectionVector = startToEndVector.GetNormalized();
	Vec2 orthogonalToStartToEndVector = startToEndDirectionVector.GetRotated90Degrees();

	Vec2 halfThicknessFromStartPoint = start - halfThickness * startToEndDirectionVector;

	Vec2 startLeftOfLine = halfThicknessFromStartPoint + halfThickness * orthogonalToStartToEndVector;
	Vec2 startRightOfLine = halfThicknessFromStartPoint - halfThickness * orthogonalToStartToEndVector;
	Vec2 endLeftOfLine = startLeftOfLine + ( 2 * halfThickness + length ) * startToEndDirectionVector;
	Vec2 endRightOfLine = startRightOfLine + ( 2 * halfThickness + length ) * startToEndDirectionVector;

	Vertex_PCU tempPoint;
	tempPoint.m_color = color;
	tempPoint.m_uvTexCoords = Vec2( 0, 0 );

	tempPoint.m_position = Vec3( startRightOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( endLeftOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( startLeftOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( startRightOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( endRightOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( endLeftOfLine );
	verts.push_back( tempPoint );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2DWithArrow( std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color )
{
	float halfThickness = thickness * 0.5f;

	Vec2 startToEndVector = end - start;
	float length = startToEndVector.GetLength();
	Vec2 startToEndDirectionVector = startToEndVector.GetNormalized();
	Vec2 orthogonalToStartToEndVector = startToEndDirectionVector.GetRotated90Degrees();

	Vec2 halfThicknessFromStartPoint = start - halfThickness * startToEndDirectionVector;

	Vec2 startLeftOfLine = halfThicknessFromStartPoint + halfThickness * orthogonalToStartToEndVector;
	Vec2 startRightOfLine = halfThicknessFromStartPoint - halfThickness * orthogonalToStartToEndVector;
	Vec2 endLeftOfLine = startLeftOfLine + ( 2 * halfThickness + length ) * startToEndDirectionVector;
	Vec2 endRightOfLine = startRightOfLine + ( 2 * halfThickness + length ) * startToEndDirectionVector;

	Vertex_PCU tempPoint;
	tempPoint.m_color = color;
	tempPoint.m_uvTexCoords = Vec2( 0, 0 );

	tempPoint.m_position = Vec3( startRightOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( endLeftOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( startLeftOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( startRightOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( endRightOfLine );
	verts.push_back( tempPoint );

	tempPoint.m_position = Vec3( endLeftOfLine );
	verts.push_back( tempPoint );

	Vec2 endToStartDirectionVector = -startToEndDirectionVector;

	Vec2 arrowOneDirection = endToStartDirectionVector.GetRotatedDegrees( 45.0f );
	AddVertsForLineSegment2D( verts, end, end + arrowOneDirection * 5.f, thickness, color );

	Vec2 arrowTwoDirection = endToStartDirectionVector.GetRotatedDegrees( -45.0f );
	AddVertsForLineSegment2D( verts, end, end + arrowTwoDirection * 5.f, thickness, color );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2DWithArrow( std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color )
{
	AddVertsForLineSegment2DWithArrow( verts, lineSegment.m_start, lineSegment.m_end, thickness, color );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForAABBZ3D( std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */ )
{
	verts.reserve( verts.size() + 36 );

	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );

	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );

	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );

	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );

	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );

	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ) ) );
	verts.push_back( Vertex_PCU( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ) ) );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForUVSphereZ3D( std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices /*= 64*/, float numStacks /*= 32.0f*/, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */ )
{
	verts.reserve( verts.size() + static_cast< size_t >( numSlices * numStacks ) * 6 );

	float yawAnglePerSlice = 360.f / numSlices;
	float pitchAnglePerStack = 180.f / numStacks;

	for ( float yawAngle = 0.0f; yawAngle < 360; yawAngle += yawAnglePerSlice )
	{
		float u = RangeMap( yawAngle, 0.0f, 360.0f, UVs.m_mins.x, UVs.m_maxs.x );
		float uPlus = RangeMap( yawAngle + yawAnglePerSlice, 0.0f, 360.0f, UVs.m_mins.x, UVs.m_maxs.x );

		float cosYaw = CosDegrees( yawAngle );
		float cosNextYaw = CosDegrees( yawAngle + yawAnglePerSlice );

		float sinYaw = SinDegrees( yawAngle );
		float sinNextYaw = SinDegrees( yawAngle + yawAnglePerSlice );

		for ( float pitchAngle = -90.0f; pitchAngle < 90; pitchAngle += pitchAnglePerStack )
		{
			float v = RangeMap( pitchAngle, 90.0f, -90.0f, UVs.m_mins.y, UVs.m_maxs.y );
			float vPlus = RangeMap( pitchAngle + pitchAnglePerStack, 90.0f, -90.0f, UVs.m_mins.y, UVs.m_maxs.y );

			float cosPitch = CosDegrees( pitchAngle );
			float cosNextPitch = CosDegrees( pitchAngle + pitchAnglePerStack );

			float sinPitch = SinDegrees( pitchAngle );
			float sinNextPitch = SinDegrees( pitchAngle + pitchAnglePerStack );

			verts.push_back( Vertex_PCU( center + radius * Vec3( cosPitch * cosNextYaw, cosPitch * sinNextYaw, -sinPitch ), tint, Vec2( uPlus, v ) ) );
			verts.push_back( Vertex_PCU( center + radius * Vec3( cosPitch * cosYaw, cosPitch * sinYaw, -sinPitch ), tint, Vec2( u, v ) ) );
			verts.push_back( Vertex_PCU( center + radius * Vec3( cosNextPitch * cosYaw, cosNextPitch * sinYaw, -sinNextPitch ), tint, Vec2( u, vPlus ) ) );

			verts.push_back( Vertex_PCU( center + radius * Vec3( cosPitch * cosNextYaw, cosPitch * sinNextYaw, -sinPitch ), tint, Vec2( uPlus, v ) ) );
			verts.push_back( Vertex_PCU( center + radius * Vec3( cosNextPitch * cosYaw, cosNextPitch * sinYaw, -sinNextPitch ), tint, Vec2( u, vPlus ) ) );
			verts.push_back( Vertex_PCU( center + radius * Vec3( cosNextPitch * cosNextYaw, cosNextPitch * sinNextYaw, -sinNextPitch ), tint, Vec2( uPlus, vPlus ) ) );
		}
	}
}


//-------------------------------------------------------------------------------------------------
void AddVertsForCylinderZ3D( std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices /*= 64.f*/, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */ )
{
	verts.reserve( verts.size() + static_cast< size_t >( numSlices ) * 12 );

	float anglePerSlice = 360.f / numSlices;
	float uRange = UVs.m_maxs.x - UVs.m_mins.x;
	float vRange = UVs.m_maxs.y - UVs.m_mins.y;

	for ( int sliceNum = 0; sliceNum < numSlices; sliceNum++ )
	{
		float sliceAngle = sliceNum * anglePerSlice;

		float sliceAngleCos = CosDegrees( sliceAngle );
		float sliceAngleSin = SinDegrees( sliceAngle );
		float nextSliceAngleCos = CosDegrees( sliceAngle + anglePerSlice );
		float nextSliceAngleSin = SinDegrees( sliceAngle + anglePerSlice );

		//One full vertical slice
		//Top circle
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ), tint, Vec2( 0.5f * uRange, 0.5f * vRange ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + radius * Vec3( sliceAngleCos, sliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( sliceAngleCos, sliceAngleSin ) ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + radius * Vec3( nextSliceAngleCos, nextSliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( nextSliceAngleCos, nextSliceAngleSin ) ) ) );

		//Bottom circle
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ), tint, Vec2( 0.5f * uRange, 0.5f * vRange ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( nextSliceAngleCos, nextSliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( nextSliceAngleCos, -nextSliceAngleSin ) ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( sliceAngleCos, sliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( sliceAngleCos, -sliceAngleSin ) ) ) );

		//Rectangle Piece
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + radius * Vec3( nextSliceAngleCos, nextSliceAngleSin, 0.0f ), tint, Vec2( UVs.m_mins.x + static_cast< float >( sliceNum + 1 ) * uRange / numSlices, UVs.m_maxs.y ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + radius * Vec3( sliceAngleCos, sliceAngleSin, 0.0f ), tint, Vec2( UVs.m_mins.x + static_cast< float >( sliceNum ) * uRange / numSlices, UVs.m_maxs.y ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( sliceAngleCos, sliceAngleSin, 0.0f ), tint, Vec2( UVs.m_mins.x + static_cast< float >( sliceNum ) * uRange / numSlices, UVs.m_mins.y ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( sliceAngleCos, sliceAngleSin, 0.0f ), tint, Vec2( UVs.m_mins.x + static_cast< float >( sliceNum ) * uRange / numSlices, UVs.m_mins.y ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( nextSliceAngleCos, nextSliceAngleSin, 0.0f ), tint, Vec2( UVs.m_mins.x + static_cast< float >( sliceNum + 1 ) * uRange / numSlices, UVs.m_mins.y ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + radius * Vec3( nextSliceAngleCos, nextSliceAngleSin, 0.0f ), tint, Vec2( UVs.m_mins.x + static_cast< float >( sliceNum + 1 ) * uRange / numSlices, UVs.m_maxs.y ) ) );

	}

}


//-------------------------------------------------------------------------------------------------
void AddVertsForLine3D( std::vector<Vertex_PCU>& verts, Vec3 start, Vec3 end, float thickness /*= 0.125f*/, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */ )
{
	std::vector<Vertex_PCU> lineLocalVerts;

	Vec3 startToEnd = end - start;

	float length = startToEnd.GetLength();

	AddVertsForCylinderZ3D( lineLocalVerts, Vec2::ZERO, FloatRange( 0.0f, length ), thickness * 0.5f, 8.f, tint, UVs );

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

	Mat44 transformMat;
	transformMat.SetIJKT3D( iBasis, jBasis, kBasis, start );

	TransformVertexArrayPositionsUsingMatrix( static_cast< int >( lineLocalVerts.size() ), lineLocalVerts.data(), transformMat );

	verts.reserve( verts.size() + lineLocalVerts.size() );

	for ( Vertex_PCU vertex : lineLocalVerts )
	{
		verts.push_back( vertex );
	}
}


//-------------------------------------------------------------------------------------------------
void AddVertsForLine3DWithArrow( std::vector<Vertex_PCU>& verts, Vec3 start, Vec3 end, float thickness /*= 0.125f*/, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */ )
{
	float arrowLength = ( end - start ).GetLength();
	float baseLength = 0.8f * arrowLength;
	float coneLength = 0.2f * arrowLength;

	int numVertsInVertsArray = static_cast< int > ( verts.size() );

	AddVertsForCylinderZ3D( verts, Vec2( 0.0f, 0.0f ), FloatRange( 0.0f, baseLength ), thickness * 0.25f, 8.f, tint, UVs );
	AddVertsForConeZ3D( verts, Vec2( 0.0f, 0.0f ), FloatRange( baseLength, coneLength + baseLength ), thickness * 0.5f, 8.f, tint, UVs );

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

	Mat44 transformMatrix;
	transformMatrix.SetIJKT3D( iBasis, jBasis, kBasis, start );

	TransformVertexArrayPositionsUsingMatrix( static_cast< int > ( verts.size() ) - numVertsInVertsArray, verts.data() + numVertsInVertsArray, transformMatrix );
}


//-------------------------------------------------------------------------------------------------
void AddVertsForAABBZ3DWireFrame( std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */, float thickness /*= 0.03125f*/ )
{
	Vec3 mins = bounds.m_mins;
	Vec3 maxs = bounds.m_maxs;

	AddVertsForLine3D( verts, mins, Vec3( mins.x, mins.y, maxs.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, mins, Vec3( mins.x, maxs.y, mins.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, mins, Vec3( maxs.x, mins.y, mins.z ), thickness, tint, UVs );

	AddVertsForLine3D( verts, maxs, Vec3( maxs.x, maxs.y, mins.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, maxs, Vec3( maxs.x, mins.y, maxs.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, maxs, Vec3( mins.x, maxs.y, maxs.z ), thickness, tint, UVs );

	AddVertsForLine3D( verts, Vec3( mins.x, mins.y, maxs.z ), Vec3( mins.x, maxs.y, maxs.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, Vec3( mins.x, mins.y, maxs.z ), Vec3( maxs.x, mins.y, maxs.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, Vec3( mins.x, maxs.y, mins.z ), Vec3( mins.x, maxs.y, maxs.z ), thickness, tint, UVs );

	AddVertsForLine3D( verts, Vec3( maxs.x, maxs.y, mins.z ), Vec3( maxs.x, mins.y, mins.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, Vec3( maxs.x, maxs.y, mins.z ), Vec3( mins.x, maxs.y, mins.z ), thickness, tint, UVs );
	AddVertsForLine3D( verts, Vec3( maxs.x, mins.y, maxs.z ), Vec3( maxs.x, mins.y, mins.z ), thickness, tint, UVs );

}


//-------------------------------------------------------------------------------------------------
void AddVertsForUVSphereZ3DWireFrame( std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float numSlices /*= 64*/, float numStacks /*= 32.0f*/, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */, float thickness /*= 0.03125f*/ )
{
	float yawAnglePerSlice = 360.f / numSlices;
	float pitchAnglePerStack = 180.f / numStacks;

	for ( float yawAngle = 0.0f; yawAngle < 360.0f; yawAngle += yawAnglePerSlice )
	{
		for ( float pitchAngle = -90.0f; pitchAngle < 90.0f; pitchAngle += pitchAnglePerStack )
		{
			Vec3 middlePoint = center + radius * Vec3( CosDegrees( pitchAngle ) * CosDegrees( yawAngle ), CosDegrees( pitchAngle ) * SinDegrees( yawAngle ), -SinDegrees( pitchAngle ) );
			Vec3 rightPoint = center + radius * Vec3( CosDegrees( pitchAngle ) * CosDegrees( yawAngle + yawAnglePerSlice ), CosDegrees( pitchAngle ) * SinDegrees( yawAngle + yawAnglePerSlice ), -SinDegrees( pitchAngle ) );
			Vec3 upPoint = center + radius * Vec3( CosDegrees( pitchAngle + pitchAnglePerStack ) * CosDegrees( yawAngle ), CosDegrees( pitchAngle + pitchAnglePerStack ) * SinDegrees( yawAngle ), -SinDegrees( pitchAngle + pitchAnglePerStack ) );

			AddVertsForLine3D( verts, middlePoint, rightPoint, thickness, tint, UVs );
			AddVertsForLine3D( verts, middlePoint, upPoint, thickness, tint, UVs );
		}
	}
}


//-------------------------------------------------------------------------------------------------
void AddVertsForCylinderZ3DWireFrame( std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices /*= 64.f*/, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */, float thickness /*= 0.03125f*/ )
{
	float anglePerSlice = 360.f / numSlices;

	for ( int sliceNum = 0; sliceNum < numSlices; sliceNum++ )
	{
		float sliceAngle = sliceNum * anglePerSlice;

		float sliceAngleCos = CosDegrees( sliceAngle );
		float sliceAngleSin = SinDegrees( sliceAngle );
		float nextSliceAngleCos = CosDegrees( sliceAngle + anglePerSlice );
		float nextSliceAngleSin = SinDegrees( sliceAngle + anglePerSlice );

		AddVertsForLine3D( verts, Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ), Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + Vec3( radius * sliceAngleCos, radius * sliceAngleSin, 0.0f ), thickness, tint, UVs );
		AddVertsForLine3D( verts, Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ), Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + Vec3( radius * sliceAngleCos, radius * sliceAngleSin, 0.0f ), thickness, tint, UVs );

		AddVertsForLine3D( verts, Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + Vec3( radius * sliceAngleCos, radius * sliceAngleSin, 0.0f ), Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + Vec3( radius * sliceAngleCos, radius * sliceAngleSin, 0.0f ), thickness, tint, UVs );

		AddVertsForLine3D( verts, Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + Vec3( radius * sliceAngleCos, radius * sliceAngleSin, 0.0f ), Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + Vec3( radius * nextSliceAngleCos, radius * nextSliceAngleSin, 0.0f ), thickness, tint, UVs );
		AddVertsForLine3D( verts, Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + Vec3( radius * sliceAngleCos, radius * sliceAngleSin, 0.0f ), Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ) + Vec3( radius * nextSliceAngleCos, radius * nextSliceAngleSin, 0.0f ), thickness, tint, UVs );
	}
}


//-------------------------------------------------------------------------------------------------
void AddVertsForConeZ3D( std::vector<Vertex_PCU>& verts, Vec2 const& centerXY, FloatRange const& minMaxZ, float radius, float numSlices /*= 64.f*/, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */ )
{
	float anglePerSlice = 360.f / numSlices;
	float uRange = UVs.m_maxs.x - UVs.m_mins.x;
	float vRange = UVs.m_maxs.y - UVs.m_mins.y;

	for ( int sliceNum = 0; sliceNum < numSlices; sliceNum++ )
	{
		float sliceAngle = sliceNum * anglePerSlice;

		float sliceAngleCos = CosDegrees( sliceAngle );
		float sliceAngleSin = SinDegrees( sliceAngle );
		float nextSliceAngleCos = CosDegrees( sliceAngle + anglePerSlice );
		float nextSliceAngleSin = SinDegrees( sliceAngle + anglePerSlice );

		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_max ), tint, Vec2( 0.5f * uRange, 0.5f * vRange ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( sliceAngleCos, sliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( sliceAngleCos, sliceAngleSin ) ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( nextSliceAngleCos, nextSliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( nextSliceAngleCos, nextSliceAngleSin ) ) ) );

		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ), tint, Vec2( 0.5f * uRange, 0.5f * vRange ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( nextSliceAngleCos, nextSliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( nextSliceAngleCos, -nextSliceAngleSin ) ) ) );
		verts.push_back( Vertex_PCU( Vec3( centerXY.x, centerXY.y, minMaxZ.m_min ) + radius * Vec3( sliceAngleCos, sliceAngleSin, 0.0f ), tint, 0.5f * ( Vec2( uRange, vRange ) + Vec2( sliceAngleCos, -sliceAngleSin ) ) ) );
	}
}


//-------------------------------------------------------------------------------------------------
void AddVertsForQuad3D( std::vector<Vertex_PCU>& verts, Vec3 bottomLeft, Vec3 bottomRight, Vec3 topLeft, Vec3 topRight, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& uvBounds /*= AABB2::ZEROTOONE */ )
{
	constexpr int NUM_VERTS = 6;
	verts.reserve( verts.size() + NUM_VERTS );

	Vec2 bottomLeftUV = uvBounds.m_mins;
	Vec2 bottomRightUV = Vec2( uvBounds.m_maxs.x, uvBounds.m_mins.y );
	Vec2 topLeftUV = Vec2( uvBounds.m_mins.x, uvBounds.m_maxs.y );
	Vec2 topRightUV = uvBounds.m_maxs;

	verts.push_back( Vertex_PCU( bottomLeft, tint, bottomLeftUV ) );
	verts.push_back( Vertex_PCU( bottomRight, tint, bottomRightUV ) );
	verts.push_back( Vertex_PCU( topRight, tint, topRightUV ) );

	verts.push_back( Vertex_PCU( bottomLeft, tint, bottomLeftUV ) );
	verts.push_back( Vertex_PCU( topRight, tint, topRightUV ) );
	verts.push_back( Vertex_PCU( topLeft, tint, topLeftUV ) );
}


//--------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB3TBN( std::vector<Vertex_PCUTBN>& verts, AABB3 const& bounds, Rgba8 const& tint /*= Rgba8::WHITE*/, AABB2 const& UVs /*= AABB2::ZEROTOONE */ )
{
	verts.reserve( verts.size() + 36 );

	Vec3 vertex1 = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 vertex2 = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z );
	Vec3 vertex3 = Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z );
	Vec3 vertex4 = Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z );

	Vec3 vertex5 = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z );
	Vec3 vertex6 = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z );
	Vec3 vertex7 = Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z );
	Vec3 vertex8 = Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			AABB3 NORMALS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 frontFaceNormal  = CrossProduct3D( vertex3 - vertex4, vertex2 - vertex4 ).GetNormalized();
	Vec3 backFaceNormal   = -frontFaceNormal;
	Vec3 leftFaceNormal  = CrossProduct3D( vertex2 - vertex4, vertex8 - vertex4 ).GetNormalized();
	Vec3 rightFaceNormal   = -leftFaceNormal;
	Vec3 topFaceNormal    = CrossProduct3D( vertex8 - vertex4, vertex3 - vertex4 ).GetNormalized();
	Vec3 bottomFaceNormal = -topFaceNormal;

//--------------------------------------------------------------------------------------------------------------------------------------------
//			AABB3 TANGENTS
//--------------------------------------------------------------------------------------------------------------------------------------------
	Vec3 frontFaceTangent  = (vertex2 - vertex1).GetNormalized();
	Vec3 backFaceTangent   = (vertex5 - vertex6).GetNormalized();
	Vec3 rightFaceTangent  = (vertex1 - vertex5).GetNormalized();
	Vec3 leftFaceTangent   = (vertex6 - vertex2).GetNormalized();
	Vec3 topFaceTangent    = (vertex3 - vertex7).GetNormalized();
	Vec3 bottomFaceTangent = (vertex2 - vertex6).GetNormalized();

//--------------------------------------------------------------------------------------------------------------------------------------------
//			Front Face
//--------------------------------------------------------------------------------------------------------------------------------------------
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), frontFaceTangent, Vec3::ZERO, frontFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), frontFaceTangent, Vec3::ZERO, frontFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), frontFaceTangent, Vec3::ZERO, frontFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), frontFaceTangent, Vec3::ZERO, frontFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), frontFaceTangent, Vec3::ZERO, frontFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), frontFaceTangent, Vec3::ZERO, frontFaceNormal ) );
					 																													 
//--------------------------------------------------------------------------------------------------------------------------------------------
//			Back Face
//--------------------------------------------------------------------------------------------------------------------------------------------
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), backFaceTangent, Vec3::ZERO, backFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), backFaceTangent, Vec3::ZERO, backFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), backFaceTangent, Vec3::ZERO, backFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), backFaceTangent, Vec3::ZERO, backFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), backFaceTangent, Vec3::ZERO, backFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), backFaceTangent, Vec3::ZERO, backFaceNormal ) );
					
//--------------------------------------------------------------------------------------------------------------------------------------------
//			Left Face
//--------------------------------------------------------------------------------------------------------------------------------------------
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), leftFaceTangent, Vec3::ZERO, leftFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), leftFaceTangent, Vec3::ZERO, leftFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), leftFaceTangent, Vec3::ZERO, leftFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), leftFaceTangent, Vec3::ZERO, leftFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), leftFaceTangent, Vec3::ZERO, leftFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), leftFaceTangent, Vec3::ZERO, leftFaceNormal ) );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			Right Face
//--------------------------------------------------------------------------------------------------------------------------------------------					 																													 
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), rightFaceTangent, Vec3::ZERO, rightFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), rightFaceTangent, Vec3::ZERO, rightFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), rightFaceTangent, Vec3::ZERO, rightFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), rightFaceTangent, Vec3::ZERO, rightFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), rightFaceTangent, Vec3::ZERO, rightFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), rightFaceTangent, Vec3::ZERO, rightFaceNormal ) );
					 																													 
//--------------------------------------------------------------------------------------------------------------------------------------------
//			Top Face
//--------------------------------------------------------------------------------------------------------------------------------------------
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), topFaceTangent, Vec3::ZERO, topFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), topFaceTangent, Vec3::ZERO, topFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), topFaceTangent, Vec3::ZERO, topFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), topFaceTangent, Vec3::ZERO, topFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), topFaceTangent, Vec3::ZERO, topFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), topFaceTangent, Vec3::ZERO, topFaceNormal ) );

//--------------------------------------------------------------------------------------------------------------------------------------------
//			Bottom Face
//--------------------------------------------------------------------------------------------------------------------------------------------
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), bottomFaceTangent, Vec3::ZERO, bottomFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_maxs.y ), bottomFaceTangent, Vec3::ZERO, bottomFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), bottomFaceTangent, Vec3::ZERO, bottomFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_mins.y ), bottomFaceTangent, Vec3::ZERO, bottomFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z ), tint, Vec2( UVs.m_maxs.x, UVs.m_maxs.y ), bottomFaceTangent, Vec3::ZERO, bottomFaceNormal ) );
	verts.push_back( Vertex_PCUTBN( Vec3( bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z ), tint, Vec2( UVs.m_mins.x, UVs.m_mins.y ), bottomFaceTangent, Vec3::ZERO, bottomFaceNormal ) );
}
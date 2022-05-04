#pragma once
#include "Engine/Math/Vec3.hpp"


struct AABB3
{
public:
	~AABB3();
	AABB3();
	explicit AABB3( Vec3 min, Vec3 max );
	explicit AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ );
	explicit AABB3( Vec3 center, float xDims, float yDims, float zDims );
	AABB3( const AABB3& copyFrom );

	void operator=( const AABB3& copyFrom );

	static const AABB3 ZEROTOONE;

	bool IsPointInside( Vec3 point ) const;
	Vec3 GetCenter() const;
	Vec3 GetDimensions() const;

	void Translate( Vec3 translation );
	void SetCenter( Vec3 center );
	void SetDimensions( Vec3 dimensions );
	void StretchToIncludePoint( Vec3 point );

	void MakeAABBCube( bool matchBiggestDimensions = true );

public:
	Vec3 m_mins;
	Vec3 m_maxs;
};
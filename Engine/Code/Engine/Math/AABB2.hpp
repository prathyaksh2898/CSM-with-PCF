#pragma once
#include "Vec2.hpp"

struct AABB2 {
public:
	~AABB2();
	AABB2();
	explicit AABB2(Vec2 min, Vec2 max);
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	AABB2(const AABB2& copyFrom);

	void operator=(const AABB2& copyFrom);

	bool IsPointInside(Vec2 point) const;
	Vec2 GetCenter() const;
	Vec2 GetDimensions() const;
	Vec2 GetNearestPoint(Vec2 point) const;
	Vec2 GetPointAtUV(Vec2 uv) const;
	Vec2 GetUVForPoint(Vec2 point) const;
	
	void Translate(Vec2 translation);
	void SetCenter(Vec2 center);
	void SetDimensions(Vec2 dimensions);
	void StretchToIncludePoint(Vec2 point);

	static const AABB2 ZEROTOONE;

public:
	Vec2 m_mins;
	Vec2 m_maxs;
};
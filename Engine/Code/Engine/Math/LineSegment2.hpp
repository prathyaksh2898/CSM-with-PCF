#pragma once
#include "Vec2.hpp"

struct LineSegment2 {
public:
	Vec2 m_start = Vec2::ZERO;
	Vec2 m_end = Vec2::ONE;

public:
	~LineSegment2();
	LineSegment2();
	LineSegment2(const LineSegment2& copyFrom);
	explicit LineSegment2(Vec2 start, Vec2 end);
	explicit LineSegment2(float startX, float startY, float endX, float endY);
	
	void		operator=(const LineSegment2& lineSegmentToAssign);
};
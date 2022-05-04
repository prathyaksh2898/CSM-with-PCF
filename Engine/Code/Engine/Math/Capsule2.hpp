#pragma once
#include "LineSegment2.hpp"

struct Capsule2 {
public:
	LineSegment2 m_bone;
	float m_radius;

public:
	~Capsule2();
	Capsule2();
	Capsule2(const Capsule2& copyFrom);
	explicit Capsule2(LineSegment2 bone, float radius);
	explicit Capsule2(Vec2 start, Vec2 end, float radius);
};
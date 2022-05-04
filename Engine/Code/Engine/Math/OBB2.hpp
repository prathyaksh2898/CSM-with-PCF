#pragma once

#include "Vec2.hpp"

struct OBB2 {
public:
	Vec2 m_center = Vec2::ZERO;
	Vec2 m_iBasisNormal = Vec2::ONE;
	Vec2 m_halfDimensions = Vec2(0.5f, 0.5f);

public:
	~OBB2();
	OBB2();
	OBB2(const OBB2& copyFrom);
	explicit OBB2(Vec2 center, Vec2 iBasisNormal, Vec2 m_halfDimensions);
};
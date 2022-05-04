#pragma once
#include "Engine/Math/Mat44.hpp"


//-----------------------------------------------------------------------------------------------
struct Vec3;


//-----------------------------------------------------------------------------------------------
struct EulerAngles
{
public:
	EulerAngles() = default;
	EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees);
	
	void              GetAsVectors_XFwd_YLeft_ZUp(Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const;
	Vec3              GetVectorXFwd() const;
	Vec3              GetVectorYLeft() const;
	Vec3              GetVectorZUp() const;
	Mat44             GetAsMatrix_XFwd_YLeft_ZUp() const;

	EulerAngles const operator+( const EulerAngles& eulerAnglesToAdd ) const;
	EulerAngles const operator*( float uniformScale ) const;			// Vec3 * float

public:
	float m_yawDegrees   = 0.0f;
	float m_pitchDegrees = 0.0f;
	float m_rollDegrees  = 0.0f;
};
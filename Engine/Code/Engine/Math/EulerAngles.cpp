#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
EulerAngles::EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees)
{
	m_yawDegrees = yawDegrees;
	m_pitchDegrees = pitchDegrees;
	m_rollDegrees = rollDegrees;
}


//-----------------------------------------------------------------------------------------------
void EulerAngles::GetAsVectors_XFwd_YLeft_ZUp(Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const
{
	Mat44 vectorMatrix;
	vectorMatrix = GetAsMatrix_XFwd_YLeft_ZUp();

	out_forwardIBasis.x = vectorMatrix.m_values[vectorMatrix.Ix];
	out_forwardIBasis.y = vectorMatrix.m_values[vectorMatrix.Iy];
	out_forwardIBasis.z = vectorMatrix.m_values[vectorMatrix.Iz];

	out_leftJBasis.x = vectorMatrix.m_values[vectorMatrix.Jx];
	out_leftJBasis.y = vectorMatrix.m_values[vectorMatrix.Jy];
	out_leftJBasis.z = vectorMatrix.m_values[vectorMatrix.Jz];

	out_upKBasis.x = vectorMatrix.m_values[vectorMatrix.Kx];
	out_upKBasis.y = vectorMatrix.m_values[vectorMatrix.Ky];
	out_upKBasis.z = vectorMatrix.m_values[vectorMatrix.Kz];

}


//-----------------------------------------------------------------------------------------------
Vec3 EulerAngles::GetVectorXFwd() const
{
	Vec3 forward;
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );

	forward.x = cy * cp;
	forward.y = sy * cp;
	forward.z = -sp;

	return forward;
}


//-----------------------------------------------------------------------------------------------
Vec3 EulerAngles::GetVectorYLeft() const
{
	Vec3 left;
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	float cr = CosDegrees( m_rollDegrees );
	float sr = SinDegrees( m_rollDegrees );

	left.x = cy * sp * sr - sy * cr;
	left.y = sy * sp * sr + cy * cr;
	left.z = cp * sr;

	return left;
}


//-----------------------------------------------------------------------------------------------
Vec3 EulerAngles::GetVectorZUp() const
{
	Vec3 up;
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	float cr = CosDegrees( m_rollDegrees );
	float sr = SinDegrees( m_rollDegrees );

	up.x = cy * sp * cr + sy * sr;
	up.y = sy * sp * cr - cy * sr;
	up.z = cp * cr;

	return up;
}


//-----------------------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_XFwd_YLeft_ZUp() const
{
	Mat44 matrix;
	
	float cy = CosDegrees( m_yawDegrees );
	float sy = SinDegrees( m_yawDegrees );
	float cp = CosDegrees( m_pitchDegrees );
	float sp = SinDegrees( m_pitchDegrees );
	float cr = CosDegrees( m_rollDegrees );
	float sr = SinDegrees( m_rollDegrees );

	matrix.m_values[ Mat44::Ix ] = cy * cp;
	matrix.m_values[ Mat44::Iy ] = sy * cp;
	matrix.m_values[ Mat44::Iz ] = -sp;

	matrix.m_values[ Mat44::Jx ] = cy * sp * sr - sy * cr;
	matrix.m_values[ Mat44::Jy ] = sy * sp * sr + cy * cr;
	matrix.m_values[ Mat44::Jz ] = cp * sr;

	matrix.m_values[ Mat44::Kx ] = cy * sp * cr + sy * sr;
	matrix.m_values[ Mat44::Ky ] = sy * sp * cr - cy * sr;
	matrix.m_values[ Mat44::Kz ] = cp * cr;

	return matrix;
}


//-----------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator+( const EulerAngles& eulerAnglesToAdd ) const
{
	return EulerAngles( this->m_yawDegrees + eulerAnglesToAdd.m_yawDegrees, this->m_pitchDegrees + eulerAnglesToAdd.m_pitchDegrees, this->m_rollDegrees + eulerAnglesToAdd.m_rollDegrees );
}


//-----------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator*( float uniformScale ) const
{
	return EulerAngles( this->m_yawDegrees * uniformScale, this->m_pitchDegrees * uniformScale, this->m_rollDegrees * uniformScale );
}

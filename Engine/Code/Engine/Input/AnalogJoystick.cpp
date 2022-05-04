#include "AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"

Vec2  AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}

float AnalogJoystick::GetMagnitude() const
{
	return m_correctedPosition.GetLength();
}

float AnalogJoystick::GetOrientationDegrees() const
{
	return m_correctedPosition.GetOrientationDegrees();
}

Vec2  AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}

float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}

float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}

void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2( 0, 0 );
	m_correctedPosition = Vec2( 0, 0 );
}

void AnalogJoystick::SetDeadZoneThreshholds( float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold )
{
	m_innerDeadZoneFraction = normalizedInnerDeadzoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadzoneThreshold;
}

void AnalogJoystick::UpdatePosition( float rawNormalizedX, float rawNormalizedY )
{
	m_rawPosition = Vec2( rawNormalizedX, rawNormalizedY );

	float normalizedPolarRadius = m_rawPosition.GetLength();
	float polarAngle = m_rawPosition.GetOrientationDegrees();

	float correctPolarRadius = RangeMapClamped( normalizedPolarRadius, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f );

	m_correctedPosition = Vec2::MakeFromPolarDegrees( polarAngle, correctPolarRadius );

}
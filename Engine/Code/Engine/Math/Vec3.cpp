#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::ZERO = Vec3( 0.0f, 0.0f, 0.0f );
Vec3 const Vec3::ONE  = Vec3( 1.0f, 1.0f, 1.0f );


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}


//-----------------------------------------------------------------------------------------------
Vec3::Vec3( Vec2 const& point )
	:x( point.x ),
	y( point.y )
{

}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
	return sqrtf( x * x + y * y + z * z );
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
	return sqrtf( x * x + y * y );
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
	return ( x * x + y * y + z * z );
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXYSquared() const
{
	return ( x * x + y * y );
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZRadians() const
{
	return atan2f( y, x );
}


//-----------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees( y, x );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetRotatedAboutZRadians( float deltaRadians ) const
{
	float polarCoordinateRadius = GetLengthXY();
	float polarCoordinateAngle = GetAngleAboutZRadians();
	float newPolarCoordinateAngle = polarCoordinateAngle + deltaRadians;
	float newXCoordinate = polarCoordinateRadius * cosf( newPolarCoordinateAngle );
	float newYCoordinate = polarCoordinateRadius * sinf( newPolarCoordinateAngle );
	Vec3 const rotatedVector( newXCoordinate, newYCoordinate, z );
	return rotatedVector;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetRotatedAboutZDegrees( float deltaDegrees ) const
{
	float polarCoordinateRadius = GetLengthXY();
	float polarCoordinateAngle = GetAngleAboutZDegrees();
	float newPolarCoordinateAngle = polarCoordinateAngle + deltaDegrees;

	float newXCoordinate = polarCoordinateRadius * CosDegrees( newPolarCoordinateAngle );
	float newYCoordinate = polarCoordinateRadius * SinDegrees( newPolarCoordinateAngle );

	Vec3 const rotatedVector( newXCoordinate, newYCoordinate, z );

	return rotatedVector;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetClamped( float maxLength ) const
{
	float presentLength = GetLength();
	if ( presentLength < maxLength || presentLength == 0 )
	{
		Vec3 const newVec3 = Vec3( x, y, z );
		return newVec3;
	}
	Vec3 const newVec3 = Vec3( x, y, z ) * maxLength / presentLength;
	return newVec3;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Vec3::GetNormalized() const
{
	float length = GetLength();

	if ( length == 0 )
	{
		return Vec3::ZERO;
	}

	Vec3 const newVec3 = Vec3( x, y, z ) / length;
	return newVec3;
}


//-----------------------------------------------------------------------------------------------
void Vec3::SetFromText( const char* text )
{
	Strings vec3Strings = SplitStringOnDelimiter( text, ',' );
	GUARANTEE_OR_DIE( vec3Strings.size() == 3, "Vec3 accepts only 3 arguments" );
	x = static_cast< float >( atof( vec3Strings[ 0 ].c_str() ) );
	y = static_cast< float >( atof( vec3Strings[ 1 ].c_str() ) );
	z = static_cast< float >( atof( vec3Strings[ 2 ].c_str() ) );
}


//-----------------------------------------------------------------------------------------------
void Vec3::Normalize()
{
	float length = GetLength();
	if ( length == 0 )
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	else
	{
		float lengthInv = 1 / length;
		x = x * lengthInv;
		y = y * lengthInv;
		z = z * lengthInv;
	}
}


//-----------------------------------------------------------------------------------------------
float Vec3::NormalizeAndGetPreviousLength()
{
	float length = GetLength();

	if ( length == 0.0f )
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	else
	{
		x = x / length;
		y = y / length;
		z = z / length;
	}

	return length;
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	return Vec3( this->x + vecToAdd.x, this->y + vecToAdd.y, this->z + vecToAdd.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( this->x - vecToSubtract.x, this->y - vecToSubtract.y, this->z - vecToSubtract.z );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
	return Vec3( -this->x, -this->y, -this->z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( this->x * uniformScale, this->y * uniformScale, this->z * uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	return Vec3( this->x * vecToMultiply.x, this->y * vecToMultiply.y, this->z * vecToMultiply.z );
}


//-----------------------------------------------------------------------------------------------
bool Vec3::IsAlmostEqual( Vec3 const& value, float epsilon )
{
	if ( x > value.x + epsilon || x < value.x - epsilon )
		return false;

	if ( y > value.y + epsilon || y < value.y - epsilon )
		return false;

	if ( z > value.z + epsilon || z < value.z - epsilon )
		return false;

	return true;
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( this->x / inverseScale, this->y / inverseScale, this->z / inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	this->x += vecToAdd.x;
	this->y += vecToAdd.y;
	this->z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	this->x -= vecToSubtract.x;
	this->y -= vecToSubtract.y;
	this->z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	this->x *= uniformScale;
	this->y *= uniformScale;
	this->z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	this->x /= uniformDivisor;
	this->y /= uniformDivisor;
	this->z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale );
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if ( this->x == compare.x && this->y == compare.y && this->z == compare.z )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if ( this->x != compare.x || this->y != compare.y || this->z != compare.z )
	{
		return true;
	}
	return false;
}



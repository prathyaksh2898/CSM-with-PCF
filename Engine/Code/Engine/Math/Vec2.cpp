#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( Vec3 const& vect ):
	x( vect.x ),
	y( vect.y )
{

}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length) {
	Vec2 const newVec2 = Vec2(CosDegrees(orientationDegrees), SinDegrees(orientationDegrees)) * length;
	return newVec2;
}

//normalize and get previous length
//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length) {
	Vec2 const newVec2 = Vec2(cosf(orientationRadians), sinf(orientationRadians)) * length;
	return newVec2;
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const {
	return sqrtf(x * x + y * y);
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const {
	return (x * x + y * y);
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationDegrees() const {
	return Atan2Degrees(y, x);
}


//-----------------------------------------------------------------------------------------------
float Vec2::GetOrientationRadians() const {
	return atan2f(y, x);
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotated90Degrees() const {
	Vec2 const newVec2 = Vec2(-y, x);
	return newVec2;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedMinus90Degrees() const {
	Vec2 const newVec2 = Vec2(y, -x);
	return newVec2;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedRadians(float deltaRadians) const {
	float polarCoordinatesRadius = GetLength();
	float polarCoordinatesAngle = GetOrientationRadians();
	polarCoordinatesAngle += deltaRadians;
	Vec2 const newVec2 = Vec2(cosf(polarCoordinatesAngle), sinf(polarCoordinatesAngle)) * polarCoordinatesRadius;
	return newVec2;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedDegrees(float deltaDegrees) const {
	float polarCoordinatesRadius = GetLength();
	float polarCoordinatesAngle = GetOrientationDegrees();
	polarCoordinatesAngle += deltaDegrees;
	Vec2 const newVec2 = Vec2(CosDegrees(polarCoordinatesAngle), SinDegrees(polarCoordinatesAngle)) * polarCoordinatesRadius;
	return newVec2;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetClamped(float maxLength) const {
	float presentLength = GetLength();
	if (presentLength < maxLength) {
		Vec2 const newVec2 = Vec2(x, y);
		return newVec2;
	}
	Vec2 const newVec2 = Vec2(x, y) * maxLength / presentLength;
	return newVec2;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetNormalized() const {
	float length = GetLength();
	if (length == 0.0f)
		return Vec2::ZERO;
	Vec2 const newVec2 = Vec2(x/length, y/length);
	return newVec2;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::GetReflected(Vec2 const& impactSurfaceNormal) const {
	Vec2 reflectedVector;
	float magnitudeVn = DotProduct2D(impactSurfaceNormal, *this);
	Vec2 Vn = magnitudeVn * impactSurfaceNormal;
	Vec2 Vt = *this - Vn;
	reflectedVector = Vt - Vn;
	return reflectedVector;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationRadians(float newOrientationRadians){
	float polarCoordinatesRadius = GetLength();
	x = polarCoordinatesRadius * cosf(newOrientationRadians);
	y = polarCoordinatesRadius * sinf(newOrientationRadians);
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetOrientationDegrees(float newOrientationDegrees) {
	float polarCoordinatesRadius = GetLength();
	x = polarCoordinatesRadius * CosDegrees(newOrientationDegrees);
	y = polarCoordinatesRadius * SinDegrees(newOrientationDegrees);
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians(float newOrientationRadians, float newLength) {
	x = newLength * cosf(newOrientationRadians);
	y = newLength * sinf(newOrientationRadians);
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength) {
	x = newLength * CosDegrees(newOrientationDegrees);
	y = newLength * SinDegrees(newOrientationDegrees);
}


//-----------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees() {
	float temp;
	temp = x;
	x = -y;
	y = temp;
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees() {
	float temp;
	temp = -x;
	x = y;
	y = temp;
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateRadians(float deltaRadians) {
	float polarCoordinatesRadius = GetLength();
	float polarCoordinatesAngle = GetOrientationRadians();
	x = polarCoordinatesRadius * cosf(polarCoordinatesAngle + deltaRadians);
	y = polarCoordinatesRadius * sinf(polarCoordinatesAngle +deltaRadians);
}


//-----------------------------------------------------------------------------------------------
void Vec2::RotateDegrees(float deltaDegrees) {
	float polarCoordinatesRadius = GetLength();
	float polarCoordinatesAngle = GetOrientationDegrees();
	x = polarCoordinatesRadius * CosDegrees(polarCoordinatesAngle + deltaDegrees);
	y = polarCoordinatesRadius * SinDegrees(polarCoordinatesAngle + deltaDegrees);
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetLength(float newLength) {
	float polarCoordinatesAngle = GetOrientationRadians();
	x = newLength * cosf(polarCoordinatesAngle);
	y = newLength * sinf(polarCoordinatesAngle);
}


//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength(float maxLength) {
	float presentLength = GetLength();
	if (presentLength < maxLength || presentLength == 0) {
		return;
	}
	x = x * maxLength / presentLength;
	y = y * maxLength / presentLength;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Normalize() {
	float length = GetLength();
	if (length == 0.0f) {
		x = 0.0f;
		y = 0.0f;
		return;
	}
	x /= length;
	y /= length;
}


//-----------------------------------------------------------------------------------------------
void Vec2::Reflect(Vec2 const& impactSurfaceNormal){
	float magnitudeVn = DotProduct2D(impactSurfaceNormal, *this);
	Vec2 Vn = magnitudeVn * impactSurfaceNormal;
	Vec2 Vt = *this - Vn;
	*this = Vt - Vn;
}

//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength() {
	float length = GetLength();
	Normalize();
	return length;
}


//-----------------------------------------------------------------------------------------------
void Vec2::SetFromText(const char* text){
	Strings vec2Strings = SplitStringOnDelimiter(text, ',');
	GUARANTEE_OR_DIE(vec2Strings.size() == 2, "Vec2 accepts only 2 arguments");
	x = static_cast<float>(atof(vec2Strings[0].c_str()));
	y = static_cast<float>(atof(vec2Strings[1].c_str()));
}


//-----------------------------------------------------------------------------------------------
Vec2 const Vec2::ZERO = Vec2(0.f, 0.f);
Vec2 const Vec2::ONE = Vec2(1.f, 1.f);


//-----------------------------------------------------------------------------------------------
bool Vec2::IsAlmostEqual( Vec2 value, float epsilon ) const
{
	if ( x > value.x + epsilon || x < value.x - epsilon )
		return false;

	if ( y > value.y + epsilon || y < value.y - epsilon )
		return false;

	return true;
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( this->x + vecToAdd.x, this->y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( this->x - vecToSubtract.x, this->y - vecToSubtract.y);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -this->x, -this->y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( this->x * uniformScale, this->y * uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2(this->x * vecToMultiply.x, this->y * vecToMultiply.y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2(this->x / inverseScale, this->y / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	this->x += vecToAdd.x;
	this->y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	this->x -= vecToSubtract.x;
	this->y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	this->x *= uniformScale;
	this->y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	this->x /= uniformDivisor;
	this->y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	if (this->x == compare.x && this->y == compare.y) {
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	if (this->x != compare.x || this->y != compare.y) {
		return true;
	}
	return false;
}



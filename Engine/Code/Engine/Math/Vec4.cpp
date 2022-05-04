#include "Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
Vec4::Vec4(const Vec4& copyFrom){
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4(const Vec3& copyFrom){
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = 0.0f;
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4(const Vec2& copyFrom){
	x = copyFrom.x;
	y = copyFrom.y;
	z = 0.0f;
	w = 0.0f;
}


//-----------------------------------------------------------------------------------------------
Vec4::Vec4(float initialX, float initialY, float initialZ, float initialW){
	x = initialX;
	y = initialY;
	z = initialZ;
	w = initialW;
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator+(const Vec4& vecToAdd) const{
	return Vec4(this->x + vecToAdd.x, this->y + vecToAdd.y, this->z + vecToAdd.z, this->w + vecToAdd.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-(const Vec4& vecToSubtract) const{
	return Vec4(this->x - vecToSubtract.x, this->y - vecToSubtract.y, this->z - vecToSubtract.z, this->w - vecToSubtract.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-() const{
	return Vec4(-x, -y, -z, -w);
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator/=(const float uniformDivisor){
	float inverseScale = 1 / uniformDivisor;
	x *= inverseScale;
	y *= inverseScale;
	z *= inverseScale;
	w *= inverseScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator=(const Vec4& copyFrom){
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*(float uniformScale) const{
	return Vec4(this->x * uniformScale, this->y * uniformScale, this->z * uniformScale, this->w * uniformScale);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*(const Vec4& vecToMultiply) const{
	return Vec4(this->x * vecToMultiply.x, this->y * vecToMultiply.y, this->z * vecToMultiply.z, this->w * vecToMultiply.w);
}


//-----------------------------------------------------------------------------------------------
const Vec4 Vec4::operator/(float inverseScale) const{
	return Vec4(this->x / inverseScale, this->y / inverseScale, this->z / inverseScale, this->w / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator*=(const float uniformScale){
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator-=(const Vec4& vecToSubtract){
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}


//-----------------------------------------------------------------------------------------------
void Vec4::operator+=(const Vec4& vecToAdd){
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator!=(const Vec4& compare) const{
	if (x != compare.x ||
		y != compare.y ||
		z != compare.z ||
		w != compare.w)
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec4::operator==(const Vec4& compare) const{
	if (x == compare.x &&
		y == compare.y &&
		z == compare.z &&
		w == compare.w)
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
const Vec4 operator*(float uniformScale, const Vec4& vecToScale){
	return Vec4(vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale, vecToScale.w * uniformScale);
}


//-----------------------------------------------------------------------------------------------
void Vec4::SetFromText(const char* text){
	Strings vec3Strings = SplitStringOnDelimiter(text, ',');
	GUARANTEE_OR_DIE(vec3Strings.size() == 4, "Vec4 accepts only 4 arguments");
	x = static_cast<float>(atof(vec3Strings[0].c_str()));
	y = static_cast<float>(atof(vec3Strings[1].c_str()));
	z = static_cast<float>(atof(vec3Strings[2].c_str()));
	w = static_cast<float>(atof(vec3Strings[3].c_str()));
}
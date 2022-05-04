#include "IntVec2.hpp"
#include "MathUtils.hpp"
#include <math.h>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2() {
	x = 0;
	y = 0;
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator==( const IntVec2& compare ) const
{
	if ( this->x == compare.x && this->y == compare.y )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator+( const IntVec2& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//-----------------------------------------------------------------------------------------------
IntVec2::~IntVec2() {

}


//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2(int valx, int valy) {
	x = valx;
	y = valy;
}


IntVec2::IntVec2(IntVec2 const& copyFrom) {
	x = copyFrom.x;
	y = copyFrom.y;
}

void IntVec2::operator=(const IntVec2& copyFrom) {
	x = copyFrom.x;
	y = copyFrom.y;
}


float IntVec2::GetLength() const {
	float xVal = static_cast<float>(x);
	float yVal = static_cast<float>(y);
	return sqrtf(xVal * xVal + yVal * yVal);
}


int IntVec2::GetLengthSquared() const {
	return x * x + y * y;
}


int IntVec2::GetTaxicabLength() const {
	return abs(x) + abs(y);
}


float IntVec2::GetOrientationRadians() const {
	return atan2f(static_cast<float>(y), static_cast<float>(x));
}


float IntVec2::GetOrientationDegrees() const {
	return Atan2Degrees(static_cast<float>(y), static_cast<float>(x));
}


IntVec2 IntVec2::GetRotated90Degrees() const {
	IntVec2 rotatedVec;
	rotatedVec.x = -y;
	rotatedVec.y = x;
	return rotatedVec;
}


IntVec2 IntVec2::GetRotatedMinus90Degrees() const {
	IntVec2 rotatedVec;
	rotatedVec.x = y;
	rotatedVec.y = -x;
	return rotatedVec;
}

void IntVec2::Rotate90Degrees() {
	int temp;
	temp = -y;
	y = x;
	x = temp;
}


void IntVec2::RotateMinus90Degrees() {
	int temp;
	temp = y;
	y = -x;
	x = temp;
}


void IntVec2::SetFromText(const char* text){
	Strings intVec2Strings = SplitStringOnDelimiter(text, ',');
	GUARANTEE_OR_DIE(intVec2Strings.size() == 2, "IntVec2 accepts only 2 arguments");
	x = atoi(intVec2Strings[0].c_str());
	y = atoi(intVec2Strings[1].c_str());
}

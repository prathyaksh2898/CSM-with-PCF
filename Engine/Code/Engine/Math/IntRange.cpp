#pragma once
#include "IntRange.hpp"


//-------------------------------------------------------------------------------------------------
IntRange::~IntRange() {

}


//-------------------------------------------------------------------------------------------------
IntRange::IntRange() {

}


//-------------------------------------------------------------------------------------------------
IntRange::IntRange(const IntRange& copyFrom) {
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}


//-------------------------------------------------------------------------------------------------
IntRange::IntRange(int min, int max) {
	m_min = min;
	m_max = max;
}


//-------------------------------------------------------------------------------------------------
bool IntRange::IsOnRange(int num) {
	if (num >= m_min && num <= m_max)
		return true;
	return false;
}


//-------------------------------------------------------------------------------------------------
bool IntRange::IsOverlappingWith(IntRange range) {
	if (IsOnRange(range.m_min) || IsOnRange(range.m_max))
		return true;
	
	if (range.IsOnRange(m_min) || range.IsOnRange(m_max))
		return true;
	
	return false;
}


//-------------------------------------------------------------------------------------------------
bool IntRange::operator==(const IntRange& compare) const {
	return (m_min == compare.m_min && m_max == compare.m_max);
}


//-------------------------------------------------------------------------------------------------
bool IntRange::operator!=(const IntRange& compare) const {
	return (m_min != compare.m_min || m_max != compare.m_max);
}


//-------------------------------------------------------------------------------------------------
void IntRange::operator=(const IntRange& floatRangeToAssign) {
	m_min = floatRangeToAssign.m_min;
	m_max = floatRangeToAssign.m_max;
}

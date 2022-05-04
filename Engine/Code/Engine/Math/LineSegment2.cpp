#include "LineSegment2.hpp"


//-------------------------------------------------------------------------------------------------
LineSegment2::~LineSegment2() {

}


//-------------------------------------------------------------------------------------------------
LineSegment2::LineSegment2() {

}


//-------------------------------------------------------------------------------------------------
LineSegment2::LineSegment2(const LineSegment2& copyFrom) {
	m_start = copyFrom.m_start;
	m_end = copyFrom.m_end;
}


//-------------------------------------------------------------------------------------------------
LineSegment2::LineSegment2(Vec2 start, Vec2 end) {
	m_start = start;
	m_end = end;
}


//-------------------------------------------------------------------------------------------------
LineSegment2::LineSegment2(float startX, float startY, float endX, float endY){
	m_start = Vec2(startX, startY);
	m_end = Vec2(endX, endY);
}


//-------------------------------------------------------------------------------------------------
void LineSegment2::operator=(const LineSegment2& lineSegmentToAssign){
	m_start = lineSegmentToAssign.m_start;
	m_end = lineSegmentToAssign.m_end;
}

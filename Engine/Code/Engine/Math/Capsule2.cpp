#include "Capsule2.hpp"


//-------------------------------------------------------------------------------------------------
Capsule2::~Capsule2(){

}


//-------------------------------------------------------------------------------------------------
Capsule2::Capsule2() {

}


//-------------------------------------------------------------------------------------------------
Capsule2::Capsule2(const Capsule2& copyFrom) {
	m_bone = copyFrom.m_bone;
	m_radius = copyFrom.m_radius;
}


//-------------------------------------------------------------------------------------------------
Capsule2::Capsule2(LineSegment2 bone, float radius) {
	m_bone = bone;
	m_radius = radius;
}


//-------------------------------------------------------------------------------------------------
Capsule2::Capsule2(Vec2 start, Vec2 end, float radius){
	m_bone = LineSegment2(start, end);
	m_radius = radius;
}

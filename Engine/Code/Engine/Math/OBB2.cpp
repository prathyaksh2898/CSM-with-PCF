#include "OBB2.hpp"


//-------------------------------------------------------------------------------------------------
OBB2::~OBB2(){

}


//-------------------------------------------------------------------------------------------------
OBB2::OBB2(){

}


//-------------------------------------------------------------------------------------------------
OBB2::OBB2(const OBB2& copyFrom){
	m_center = copyFrom.m_center;
	m_iBasisNormal = copyFrom.m_iBasisNormal;
	m_halfDimensions = copyFrom.m_halfDimensions;
}


//-------------------------------------------------------------------------------------------------
OBB2::OBB2(Vec2 center, Vec2 iBasisNormal, Vec2 halfDimensions){
	m_center = center;
	m_iBasisNormal = iBasisNormal;
	m_halfDimensions = halfDimensions;
}

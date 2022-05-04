#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"


//----------------------------------------------------------------------------------------------------
AABB3::~AABB3()
{

}


//----------------------------------------------------------------------------------------------------
AABB3::AABB3()
{
	m_mins = Vec3::ZERO;
	m_maxs = Vec3( 1.0f, 1.0f, 1.0f );
}


//----------------------------------------------------------------------------------------------------
AABB3::AABB3( Vec3 min, Vec3 max )
{
	m_mins = min;
	m_maxs = max;
}


//----------------------------------------------------------------------------------------------------
AABB3::AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
{
	m_mins = Vec3( minX, minY, minZ );
    m_maxs = Vec3( maxX, maxY, maxZ );
}


//----------------------------------------------------------------------------------------------------
AABB3::AABB3( const AABB3& copyFrom )
{
	m_mins = copyFrom.m_mins;
	m_maxs = copyFrom.m_maxs;
}


//----------------------------------------------------------------------------------------------------
AABB3::AABB3( Vec3 center, float xDims, float yDims, float zDims )
{
	m_mins.x = center.x - ( xDims * 0.5f );
	m_mins.y = center.y - ( yDims * 0.5f );
	m_mins.z = center.z - ( zDims * 0.5f );
	m_maxs.x = center.x + ( xDims * 0.5f );
	m_maxs.y = center.y + ( yDims * 0.5f );
	m_maxs.z = center.z + ( zDims * 0.5f );
}


//----------------------------------------------------------------------------------------------------
void AABB3::operator=( const AABB3& copyFrom )
{
	m_mins = copyFrom.m_mins;
	m_maxs = copyFrom.m_maxs;
}


//----------------------------------------------------------------------------------------------------
const AABB3 AABB3::ZEROTOONE = AABB3( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f );


//------------------------------------------------------------------------------------------------
bool AABB3::IsPointInside( Vec3 point ) const
{
	return ( m_mins.x < point.x ) &&
		   ( m_mins.y < point.y ) &&
		   ( m_mins.z < point.z ) &&
		   ( m_maxs.x > point.x ) &&
		   ( m_maxs.y > point.y ) &&
		   ( m_maxs.z > point.z );
}


//----------------------------------------------------------------------------------------------------
Vec3 AABB3::GetCenter() const
{
	Vec3 center;
	center.x = (m_mins.x + m_maxs.x) * 0.5f;
	center.y = (m_mins.y + m_maxs.y) * 0.5f;
	center.z = (m_mins.z + m_maxs.z) * 0.5f;
	return center;
}


//----------------------------------------------------------------------------------------------------
Vec3 AABB3::GetDimensions() const
{
	float width  = m_maxs.x - m_mins.x;
	float height = m_maxs.y - m_mins.y;
	float depth  = m_maxs.z - m_mins.z;
	return Vec3( width, height, depth );
}


//----------------------------------------------------------------------------------------------------
void AABB3::Translate( Vec3 translation )
{
	m_mins = m_mins + translation;
	m_maxs = m_maxs + translation;
}


//----------------------------------------------------------------------------------------------------
void AABB3::SetCenter( Vec3 center )
{
	Vec3 oldCenter = GetCenter();
	Vec3 changeInCenter = center - GetCenter();
	Translate( changeInCenter );
}


//----------------------------------------------------------------------------------------------------
void AABB3::SetDimensions( Vec3 dimensions )
{
	Vec3 halfDimensions = dimensions * 0.5f;
	Vec3 center = GetCenter();
	m_mins = center - halfDimensions;
	m_maxs = center + halfDimensions;
}


//------------------------------------------------------------------------------------------------
void AABB3::StretchToIncludePoint( Vec3 point )
{
	if ( IsPointInside( point ) )
		return;
	
	if ( point.x < m_mins.x )
		m_mins.x = point.x;
	
	if ( point.x > m_maxs.x )
		m_maxs.x = point.x;
	
	if ( point.y < m_mins.y )
		m_mins.y = point.y;
	
	if ( point.y > m_maxs.y )
		m_maxs.y = point.y;
	
	if ( point.z < m_mins.z )
		m_mins.z = point.z;
	
	if ( point.z > m_maxs.z )
		m_maxs.z = point.z;
}


//------------------------------------------------------------------------------------------------
void AABB3::MakeAABBCube( bool matchBiggestDimensions /*= true */ )
{
	if ( matchBiggestDimensions )
	{
		Vec3 dims   = GetDimensions();
		Vec3 center = GetCenter();

		if ( dims.x > dims.y && dims.x > dims.z )
		{
			m_mins.y = center.y - (dims.x * 0.5f);
			m_mins.z = center.z - (dims.x * 0.5f);
			m_maxs.y = center.y + (dims.x * 0.5f);
			m_maxs.z = center.z + (dims.x * 0.5f);
		}
		else if ( dims.y > dims.x && dims.y > dims.z )
		{
			m_mins.x = center.x - (dims.y * 0.5f);
			m_mins.z = center.z - (dims.y * 0.5f);
			m_maxs.x = center.x + (dims.y * 0.5f);
			m_maxs.z = center.z + (dims.y * 0.5f);
		}
		else if ( dims.z > dims.y && dims.z > dims.x )
		{
			m_mins.x = center.x - (dims.z * 0.5f);
			m_mins.y = center.y - (dims.z * 0.5f);
			m_maxs.x = center.x + (dims.z * 0.5f);
			m_maxs.y = center.y + (dims.z * 0.5f);
		}
	}
}

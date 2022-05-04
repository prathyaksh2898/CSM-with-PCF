#include "AABB2.hpp"
#include "MathUtils.hpp"


//--------------------------------------------------------------------------------------------------
AABB2::AABB2()
{
}


//--------------------------------------------------------------------------------------------------
AABB2::~AABB2()
{
}


//--------------------------------------------------------------------------------------------------
AABB2::AABB2( AABB2 const& copyFrom )
{
	m_mins = copyFrom.m_mins;
	m_maxs = copyFrom.m_maxs;
}


//--------------------------------------------------------------------------------------------------
AABB2::AABB2( Vec2 mins, Vec2 maxs )
{
	m_mins = mins;
	m_maxs = maxs;
}


//--------------------------------------------------------------------------------------------------
AABB2::AABB2( float x1, float y1, float x2, float y2 )
{
	float minX = ( x1 > x2 ? x2 : x1 );
	float maxX = ( x1 > x2 ? x1 : x2 );
	float minY = ( y1 > y2 ? y2 : y1 );
	float maxY = ( y1 > y2 ? y1 : y2 );
	m_mins = Vec2( minX, minY );
	m_maxs = Vec2( maxX, maxY );
}


//-----------------------------------------------------------------------------------------------
void AABB2::operator=( const AABB2& copyFrom )
{
	m_maxs = copyFrom.m_maxs;
	m_mins = copyFrom.m_mins;
}

//--------------------------------------------------------------------------------------------------
bool AABB2::IsPointInside( Vec2 point ) const
{
	return ( m_mins.x < point.x ) &&
		( m_mins.y < point.y ) &&
		( m_maxs.x > point.x ) &&
		( m_maxs.y > point.y );
}


//--------------------------------------------------------------------------------------------------
Vec2 AABB2::GetCenter() const
{
	Vec2 center;
	center.x = ( m_mins.x + m_maxs.x ) * 0.5f;
	center.y = ( m_mins.y + m_maxs.y ) * 0.5f;
	return center;
}


//--------------------------------------------------------------------------------------------------
Vec2 AABB2::GetDimensions() const
{
	float width = m_maxs.x - m_mins.x;
	float height = m_maxs.y - m_mins.y;
	return Vec2( width, height );
}


//--------------------------------------------------------------------------------------------------
Vec2 AABB2::GetNearestPoint( Vec2 point ) const
{
	float x = Clamp( point.x, m_mins.x, m_maxs.x );
	float y = Clamp( point.y, m_mins.y, m_maxs.y );
	return Vec2( x, y );
}


//--------------------------------------------------------------------------------------------------
void AABB2::Translate( Vec2 translation )
{
	m_mins = m_mins + translation;
	m_maxs = m_maxs + translation;
}


//--------------------------------------------------------------------------------------------------
void AABB2::SetCenter( Vec2 newCenter )
{
	Vec2 oldCenter = GetCenter();
	Vec2 changeInCenter = newCenter - GetCenter();
	Translate( changeInCenter );
}


//--------------------------------------------------------------------------------------------------
Vec2 AABB2::GetPointAtUV( Vec2 uv ) const
{
	Vec2 point;
	point.x = Interpolate( m_mins.x, m_maxs.x, uv.x );
	point.y = Interpolate( m_mins.y, m_maxs.y, uv.y );
	return point;
}


//--------------------------------------------------------------------------------------------------
Vec2 AABB2::GetUVForPoint( Vec2 point ) const
{
	Vec2 uv;
	uv.x = GetFractionWithin( point.x, m_mins.x, m_maxs.x );
	uv.y = GetFractionWithin( point.y, m_mins.y, m_maxs.y );
	return uv;
}


//--------------------------------------------------------------------------------------------------
void AABB2::StretchToIncludePoint( Vec2 point )
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
}


//--------------------------------------------------------------------------------------------------
AABB2 const AABB2::ZEROTOONE = AABB2( 0.f, 0.f, 1.f, 1.f );


//--------------------------------------------------------------------------------------------------
void AABB2::SetDimensions( Vec2 dimensions )
{
	Vec2 halfDimensions = dimensions * 0.5f;
	Vec2 center = GetCenter();
	m_mins = center - halfDimensions;
	m_maxs = center + halfDimensions;
}


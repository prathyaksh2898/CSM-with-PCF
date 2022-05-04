#include "FloatRange.hpp"


//-------------------------------------------------------------------------------------------------
FloatRange::~FloatRange()
{

}


//-------------------------------------------------------------------------------------------------
FloatRange::FloatRange()
{

}


//-------------------------------------------------------------------------------------------------
FloatRange::FloatRange( const FloatRange& copyFrom )
{
	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;
}


//-------------------------------------------------------------------------------------------------
FloatRange::FloatRange( float min, float max )
{
	if( min < max )
	{
		m_min = min;
		m_max = max;
	}

	else
	{
		m_max = min;
		m_min = max;
	}
}


//-------------------------------------------------------------------------------------------------
bool FloatRange::IsOnRange( float num ) const
{
	if ( num >= m_min && num <= m_max )
		return true;
	return false;
}


//-------------------------------------------------------------------------------------------------
bool FloatRange::IsOverlappingWith( FloatRange range )
{

	if ( *this == range )
		return true;

	if ( range.m_min > m_min && range.m_min < m_max || range.m_max > m_min && range.m_max < m_max )
		return true;

	if ( range.m_min < m_min && range.m_max > m_min || range.m_max > m_max && range.m_min < m_max )
		return true;

	return false;

}


//-------------------------------------------------------------------------------------------------
bool FloatRange::GetOverlap( FloatRange range, FloatRange& overlappedRange )
{
	if ( !IsOverlappingWith( range ) )
	{
		return false;
	}

	overlappedRange.m_min = m_min >= range.m_min ? m_min : range.m_min;
	overlappedRange.m_max = m_max <= range.m_max ? m_max : range.m_max;
	
	return true;
}


//-------------------------------------------------------------------------------------------------
bool FloatRange::operator==( const FloatRange& compare ) const
{
	return ( m_min == compare.m_min && m_max == compare.m_max );
}


//-------------------------------------------------------------------------------------------------
bool FloatRange::operator!=( const FloatRange& compare ) const
{
	return ( m_min != compare.m_min || m_max != compare.m_max );
}


//-------------------------------------------------------------------------------------------------
void FloatRange::operator=( const FloatRange& floatRangeToAssign )
{
	m_min = floatRangeToAssign.m_min;
	m_max = floatRangeToAssign.m_max;
}

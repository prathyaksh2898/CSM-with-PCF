#include "HeatMaps.hpp"


//-----------------------------------------------------------------------------------------------
TileHeatMap::TileHeatMap( IntVec2 const& dimensions, bool xFirst /*= false*/ ) :
	m_dimensions( dimensions ), m_xFirst( xFirst )
{
	int size = dimensions.x * dimensions.y;
	m_values.resize( static_cast< size_t >( size ) );

	for ( int valueNum = 0; valueNum < size; ++valueNum )
	{
		m_values[ valueNum ] = 0.0f;
	}
}


//-----------------------------------------------------------------------------------------------
TileHeatMap::~TileHeatMap()
{

}


//-----------------------------------------------------------------------------------------------
float TileHeatMap::GetValueAt( IntVec2 const& coordinates ) const
{
	if ( !m_xFirst )
		return m_values[ coordinates.x * m_dimensions.y + coordinates.y ];

	else
		return m_values[ coordinates.x + coordinates.y * m_dimensions.x ];
}


//-----------------------------------------------------------------------------------------------
float TileHeatMap::GetValueAt( int index ) const
{
	return m_values[ index ];
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetValueAt( IntVec2 const& coordinates, float value )
{
	if ( !m_xFirst )
		m_values[ coordinates.x * m_dimensions.y + coordinates.y ] = value;

	else
		m_values[ coordinates.x + coordinates.y * m_dimensions.x ] = value;
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::AddValueAt( IntVec2 const& coordinates, float value )
{
	if ( !m_xFirst )
		m_values[ coordinates.x * m_dimensions.y + coordinates.y ] += value;

	else
		m_values[ coordinates.x + coordinates.y * m_dimensions.x ] += value;
}


//-----------------------------------------------------------------------------------------------
void TileHeatMap::SetAllValues( float value )
{
	int size = m_dimensions.x * m_dimensions.y;

	for ( int valueNum = 0; valueNum < size; ++valueNum )
	{
		m_values[ valueNum ] = value;
	}
}

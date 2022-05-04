#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <vector>

class TileHeatMap {
public:
	explicit TileHeatMap( IntVec2 const& dimesions, bool xFirst = false );
	~TileHeatMap();
	
	float GetValueAt(IntVec2 const& coordinates) const;
	float GetValueAt( int index ) const;
	void SetValueAt(IntVec2 const& coordinates, float value);
	void AddValueAt(IntVec2 const& coordinates, float value);

	void SetAllValues(float value);

protected:
	std::vector<float> m_values;
	IntVec2 m_dimensions;
	bool m_xFirst = false;
};
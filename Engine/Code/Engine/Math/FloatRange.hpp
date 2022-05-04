#pragma once

struct FloatRange
{
public: 
	float m_min = 0.f;
	float m_max = 0.f;

public:
	~FloatRange(); 											
	FloatRange();											
	FloatRange(const FloatRange& copyFrom);					
	explicit FloatRange(float min, float max);		

	bool IsOnRange(float num) const;
	bool IsOverlappingWith(FloatRange range);

	bool GetOverlap( FloatRange range, FloatRange& overlappedRange );

	bool		operator==(const FloatRange& compare) const;		
	bool		operator!=(const FloatRange& compare) const;		
	
	// Operators (self-mutating / non-const)
	void		operator=(const FloatRange& floatRangeToAssign);	
};

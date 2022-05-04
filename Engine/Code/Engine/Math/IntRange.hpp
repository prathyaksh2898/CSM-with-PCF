#pragma once
#pragma once

struct IntRange
{
public:
	int m_min = 0;
	int m_max = 0;

public:
	~IntRange();
	IntRange();
	IntRange(const IntRange& copyFrom);
	explicit IntRange(int min, int max);

	bool IsOnRange(int num);
	bool IsOverlappingWith(IntRange range);

	bool		operator==(const IntRange& compare) const;
	bool		operator!=(const IntRange& compare) const;

	// Operators (self-mutating / non-const)
	void		operator=(const IntRange& floatRangeToAssign);
};

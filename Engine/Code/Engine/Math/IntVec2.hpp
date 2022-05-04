#pragma once

struct IntVec2
{
public:
	IntVec2();
	~IntVec2();
	IntVec2( int valx, int valy );
	IntVec2( IntVec2 const& copyFrom );

	void operator=( const IntVec2& copyFrom );

	bool		operator==( const IntVec2& compare ) const;

	const IntVec2 operator+( const IntVec2& vecToAdd ) const;		// vec2 + vec2
	const IntVec2 operator-( const IntVec2& vecToSubtract ) const;	// vec2 - vec2

	float GetLength() const;
	int GetLengthSquared() const;
	int GetTaxicabLength() const;
	float GetOrientationRadians() const;
	float GetOrientationDegrees() const;
	IntVec2 GetRotated90Degrees() const;
	IntVec2 GetRotatedMinus90Degrees() const;

	void Rotate90Degrees();
	void RotateMinus90Degrees();

	void SetFromText( const char* text );

public:
	int x = 0;
	int y = 0;
};
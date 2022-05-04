#pragma once


//-----------------------------------------------------------------------------------------------
struct Vec2;
struct Vec3;


//-----------------------------------------------------------------------------------------------
struct Vec4
{
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	// Construction/Destruction
	~Vec4() {}												
	Vec4() {}												
	Vec4(const Vec4& copyFrom);							
	Vec4(const Vec2& copyFrom);							
	Vec4(const Vec3& copyFrom);							
	explicit Vec4(float initialX, float initialY, float initialZ, float initialW);		

	// Operators (const)
	bool		operator==(const Vec4& compare) const;		
	bool		operator!=(const Vec4& compare) const;		
	const Vec4	operator+(const Vec4& vecToAdd) const;		
	const Vec4	operator-(const Vec4& vecToSubtract) const;	
	const Vec4	operator-() const;								
	const Vec4	operator*(float uniformScale) const;			
	const Vec4	operator*(const Vec4& vecToMultiply) const;	
	const Vec4	operator/(float inverseScale) const;			

	// Operators (self-mutating / non-const)
	void		operator+=(const Vec4& vecToAdd);				
	void		operator-=(const Vec4& vecToSubtract);		
	void		operator*=(const float uniformScale);		
	void		operator/=(const float uniformDivisor);		
	void		operator=(const Vec4& copyFrom);			

	// Standalone "friend" functions that are conceptually, but not actually, part of Vec4::
	friend const Vec4 operator*(float uniformScale, const Vec4& vecToScale);	

	void SetFromText(const char* text);
};





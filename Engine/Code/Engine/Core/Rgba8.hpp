#pragma once

//-----------------------------------------------------------------------------------------------
struct Rgba8
{
public:
	unsigned char r = 0xFF;
	unsigned char g = 0xFF;
	unsigned char b = 0xFF;
	unsigned char a = 0xFF;

	static Rgba8 const WHITE;
	static Rgba8 const BLUE;
	static Rgba8 const RED;
	static Rgba8 const GREEN;
	static Rgba8 const YELLOW;
	static Rgba8 const BLACK;
	static Rgba8 const ORANGE;
	
public:
	// Construction/Destruction
	~Rgba8() {}												// destructor (do nothing)
	Rgba8() {}												// default constructor (do nothing)
	explicit Rgba8(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xFF);
	
	void GetAsFloats(float* clearColorAsFloats) const;
	void SetFromText(const char* text);

	bool		operator==( const Rgba8& compare ) const;

	static Rgba8 LerpColor(Rgba8 startColor, Rgba8 endColor, float lerpValue);
};



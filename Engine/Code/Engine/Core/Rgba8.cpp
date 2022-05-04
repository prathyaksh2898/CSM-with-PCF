#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <stdlib.h>


//------------------------------------------------------------------------------------------------
const unsigned char FULL = 255;


//------------------------------------------------------------------------------------------------
Rgba8::Rgba8( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha )
	: r( red )
	, g( green )
	, b( blue )
	, a( alpha )
{
}


//------------------------------------------------------------------------------------------------
void Rgba8::GetAsFloats( float* clearColorAsFloats ) const
{
	clearColorAsFloats[ 0 ] = static_cast< float > ( r ) / 255.0f;
	clearColorAsFloats[ 1 ] = static_cast< float > ( g ) / 255.0f;
	clearColorAsFloats[ 2 ] = static_cast< float > ( b ) / 255.0f;
	clearColorAsFloats[ 3 ] = static_cast< float > ( a ) / 255.0f;
}


//------------------------------------------------------------------------------------------------
void Rgba8::SetFromText( const char* text )
{
	Strings rgba8Strings = SplitStringOnDelimiter( text, ',' );
	int size = static_cast< int >( rgba8Strings.size() );
	GUARANTEE_OR_DIE( size == 3 || size == 4, "IntVec2 accepts only 2 arguments" );
	r = static_cast< unsigned char >( atoi( rgba8Strings[ 0 ].c_str() ) );
	g = static_cast< unsigned char >( atoi( rgba8Strings[ 1 ].c_str() ) );
	b = static_cast< unsigned char >( atoi( rgba8Strings[ 2 ].c_str() ) );
	a = 255;

	if ( size == 4 )
	{
		a = static_cast< unsigned char >( atoi( rgba8Strings[ 3 ].c_str() ) );
	}
}


//------------------------------------------------------------------------------------------------
bool Rgba8::operator==( const Rgba8& compare ) const
{
	if ( a == compare.a &&
		 b == compare.b &&
		 g == compare.g &&
		 r == compare.r )
		return true;

	return false;
}


//------------------------------------------------------------------------------------------------
Rgba8 Rgba8::LerpColor( Rgba8 startColor, Rgba8 endColor, float lerpValue )
{
	Rgba8 lerpedColor;

	lerpedColor.r = static_cast< unsigned char >( Interpolate( static_cast< float >( startColor.r ), static_cast< float >( endColor.r ), lerpValue ) );
	lerpedColor.g = static_cast< unsigned char >( Interpolate( static_cast< float >( startColor.g ), static_cast< float >( endColor.g ), lerpValue ) );
	lerpedColor.b = static_cast< unsigned char >( Interpolate( static_cast< float >( startColor.b ), static_cast< float >( endColor.b ), lerpValue ) );
	lerpedColor.a = static_cast< unsigned char >( Interpolate( static_cast< float >( startColor.a ), static_cast< float >( endColor.a ), lerpValue ) );

	return lerpedColor;
}


//------------------------------------------------------------------------------------------------
Rgba8 const Rgba8::WHITE = Rgba8( 255, 255, 255, 255 );
Rgba8 const Rgba8::BLACK = Rgba8( 0, 0, 0, 255 );

Rgba8 const Rgba8::ORANGE = Rgba8( 255, 165, 0 );
Rgba8 const Rgba8::RED = Rgba8( 255, 0, 0, 255 );
Rgba8 const Rgba8::BLUE = Rgba8( 0, 0, 255, 255 );
Rgba8 const Rgba8::GREEN = Rgba8( 0, 255, 0, 255 );
Rgba8 const Rgba8::YELLOW = Rgba8( 255, 255, 0, 255 );


#include "BitmapFont.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/VertexData/VertexUtils.hpp"

#include <vector>

//------------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( char const* fontFilePathNameWithNoExtension, Texture const& fontTexture ) :
	m_fontFilePathNameWithNoExtension( fontFilePathNameWithNoExtension ),
	m_fontGlyphsSpriteSheet( fontTexture, IntVec2( 16, 16 ) )
{

}


//------------------------------------------------------------------------------------------------
Texture const& BitmapFont::GetTexture() const
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}


//------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect )
{
	int numChars = static_cast< int >( text.length() );
	for ( int charNum = 0; charNum < numChars; ++charNum )
	{
		Vec2 bottomleft = Vec2::ZERO;
		Vec2 topRight = Vec2::ZERO;

		unsigned char character = text[ charNum ];
		float glyphAspect = GetGlyphAspect( character );

		bottomleft.x = textMins.x + charNum * glyphAspect * cellAspect * cellHeight;
		bottomleft.y = textMins.y;

		topRight.x = textMins.x + ( charNum + 1 ) * glyphAspect * cellAspect * cellHeight;
		topRight.y = textMins.y + cellHeight;

		AABB2 bounds( bottomleft, topRight );

		Vec2 textBottomLeft = Vec2::ZERO;
		Vec2 textTopRight = Vec2::ZERO;
		m_fontGlyphsSpriteSheet.GetSpriteUVs( textBottomLeft, textTopRight, character );

		AddVertsForAABB2D( vertexArray, bounds, tint, AABB2( textBottomLeft, textTopRight ) );
	}
}


//------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight, std::string const& text, Rgba8 const& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/, Vec2 const& alignment /*= Vec2(.5f, .5f)*/, TextBoxMode mode /*= TextBoxMode::SHRINK*/, int maxGlyphsToDraw /*= 99999999*/ )
{
	Strings linesOfText = SplitStringOnDelimiter( text, '\n' );
	float maxPureLineWidth = 0.f;
	int numLines = static_cast< int >( linesOfText.size() );
	float scaleValue = 1.f;

	for ( int lineNum = 0; lineNum < numLines; lineNum++ )
	{
		float pureLineWidth = GetPureTextWidth( linesOfText[ lineNum ] );
		if ( maxPureLineWidth < pureLineWidth )
		{
			maxPureLineWidth = pureLineWidth;
		}
	}

	float imaginaryBoxYSize = static_cast< float >( numLines ) * cellHeight;
	float imaginaryBoxXSize = static_cast< float >( maxPureLineWidth ) * cellAspect * cellHeight;

	if ( mode == TextBoxMode::SHRINK )
	{

		float scaleX = 1.f;
		if ( imaginaryBoxXSize > box.m_maxs.x - box.m_mins.x )
		{
			scaleX = ( box.m_maxs.x - box.m_mins.x ) / imaginaryBoxXSize;
		}

		float scaleY = 1.f;
		if ( imaginaryBoxYSize > box.m_maxs.y - box.m_mins.y )
		{
			scaleY = ( box.m_maxs.y - box.m_mins.y ) / imaginaryBoxYSize;
		}

		scaleValue = scaleX;
		if ( scaleY < scaleX )
		{
			scaleValue = scaleY;
		}
	}

	Vec2 imaginaryBoxBottomLeft = Vec2::ZERO;

	imaginaryBoxXSize = imaginaryBoxXSize * scaleValue;
	imaginaryBoxYSize = imaginaryBoxYSize * scaleValue;

	imaginaryBoxBottomLeft.x = box.m_mins.x + alignment.x * ( box.m_maxs.x - box.m_mins.x - imaginaryBoxXSize );
	imaginaryBoxBottomLeft.y = box.m_mins.y + alignment.y * ( box.m_maxs.y - box.m_mins.y - imaginaryBoxYSize );

	cellHeight = cellHeight * scaleValue;


	int glyphsDrawn = 0;

	for ( int lineNum = 0; lineNum < numLines; lineNum++ )
	{
		float lineWidth = GetPureTextWidth( linesOfText[ lineNum ] ) * cellAspect * cellHeight;

		Vec2 textMins;

		textMins.x = imaginaryBoxBottomLeft.x + alignment.x * ( imaginaryBoxXSize - lineWidth );
		textMins.y = imaginaryBoxBottomLeft.y + ( numLines - lineNum - 1 ) * cellHeight;

		int glyphsToDraw = static_cast< int >( linesOfText[ lineNum ].length() );
		glyphsDrawn += glyphsToDraw;

		if ( glyphsDrawn > maxGlyphsToDraw )
		{
			glyphsToDraw = glyphsToDraw - ( glyphsDrawn - maxGlyphsToDraw );
			std::string partText = linesOfText[ lineNum ].substr( 0, glyphsToDraw );
			AddVertsForText2D( vertexArray, textMins, cellHeight, partText, tint, cellAspect );
			break;
		}

		AddVertsForText2D( vertexArray, textMins, cellHeight, linesOfText[ lineNum ], tint, cellAspect );
	}
}


//------------------------------------------------------------------------------------------------
std::string BitmapFont::GetFontPathWithoutExtension() const
{
	return m_fontFilePathNameWithNoExtension;
}


//------------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	UNUSED( glyphUnicode );
	return 1.0f;
}


//------------------------------------------------------------------------------------------------
float BitmapFont::GetPureTextWidth( std::string const& text ) const
{
	int numChars = static_cast< int >( text.length() );
	float widthofText = 0.0f;

	for ( int charNum = 0; charNum < numChars; charNum++ )
	{
		unsigned char character = text[ charNum ];
		widthofText += GetGlyphAspect( character );
	}
	return widthofText;
}


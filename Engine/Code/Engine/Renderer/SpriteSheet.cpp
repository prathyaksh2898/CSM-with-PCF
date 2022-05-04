#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Texture.hpp"
#include "SpriteSheet.hpp"


//-------------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet( Texture const& texture, IntVec2 const& simpleGridLayout )
	:m_texture( texture ),
	m_layout( simpleGridLayout )
{

	float nudgeX = 1.0f / static_cast< float >( 100.f * texture.m_dimensions.x );
	float nudgeY = 1.0f / static_cast< float >( 100.f * texture.m_dimensions.y );

	for ( int yIndex = simpleGridLayout.y; yIndex > 0; yIndex-- )
	{
		for ( int xIndex = 0; xIndex < simpleGridLayout.x; xIndex++ )
		{
			Vec2 uvAtMins;
			Vec2 uvAtMaxs;

			uvAtMins.x = static_cast< float >( xIndex ) / static_cast< float >( simpleGridLayout.x ) + nudgeX;
			uvAtMins.y = static_cast< float >( yIndex - 1 ) / static_cast< float >( simpleGridLayout.y ) + nudgeY;

			uvAtMaxs.x = static_cast< float >( xIndex + 1 ) / static_cast< float >( simpleGridLayout.x ) - nudgeX;
			uvAtMaxs.y = static_cast< float >( yIndex ) / static_cast< float >( simpleGridLayout.y ) - nudgeY;

			SpriteDefinition newSprite = SpriteDefinition( *this, static_cast< int >( m_spriteDefs.size() ), uvAtMins, uvAtMaxs );

			m_spriteDefs.push_back( newSprite );
		}
	}
}


//-------------------------------------------------------------------------------------------------
Texture const& SpriteSheet::GetTexture() const
{
	return m_texture;
}


//-------------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{
	return static_cast< int >( m_spriteDefs.size() );
}


//-------------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	return m_spriteDefs[ spriteIndex ];

}


//-------------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteSheet::GetSpriteDefinition( IntVec2 spriteCoords ) const
{
	int index = spriteCoords.y * m_layout.x + spriteCoords.x;
	return m_spriteDefs[ index ];
}


//-------------------------------------------------------------------------------------------------
void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
	m_spriteDefs[ spriteIndex ].GetUvs( out_uvAtMins, out_uvAtMaxs );
}


//-------------------------------------------------------------------------------------------------
void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords ) const
{
	int index = spriteCoords.y * m_layout.x + spriteCoords.x;
	m_spriteDefs[ index ].GetUvs( out_uvAtMins, out_uvAtMaxs );
}

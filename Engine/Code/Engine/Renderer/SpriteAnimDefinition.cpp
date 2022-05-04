#include "SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"


//------------------------------------------------------------------------------------------------
SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/ ) :
	m_spriteSheet( sheet ),
	m_startSpriteIndex( startSpriteIndex ),
	m_endSpriteIndex( endSpriteIndex ),
	m_durationSeconds( durationSeconds ),
	m_playbackType( playbackType )
{

}


//------------------------------------------------------------------------------------------------
const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int numIndex = m_endSpriteIndex - m_startSpriteIndex + 1;
	float secondsPerIndex = m_durationSeconds / numIndex;
	int index = static_cast< int >( seconds / secondsPerIndex );

	switch ( m_playbackType )
	{
		case SpriteAnimPlaybackType::ONCE:
		{
			if ( index < numIndex )
				return m_spriteSheet.GetSpriteDefinition( m_startSpriteIndex + index );
			return m_spriteSheet.GetSpriteDefinition( m_endSpriteIndex );
		}

		case SpriteAnimPlaybackType::LOOP:
		{
			int returnIndex = index % numIndex;
			return m_spriteSheet.GetSpriteDefinition( m_startSpriteIndex + returnIndex );
		}

		case SpriteAnimPlaybackType::PINGPONG:
		{
			int returnIndex = index % ( 2 * ( numIndex - 1 ) );
			if ( returnIndex >= numIndex )
			{
				returnIndex = numIndex - ( ( returnIndex - numIndex ) + 2 );
			}
			return m_spriteSheet.GetSpriteDefinition( m_startSpriteIndex + returnIndex );
		}
	}
	return m_spriteSheet.GetSpriteDefinition( m_startSpriteIndex );
}

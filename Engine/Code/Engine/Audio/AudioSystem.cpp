#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Game/EngineBuildPreferences.hpp"

//-----------------------------------------------------------------------------------------------
AudioSystem* g_theAudio = nullptr;


//-----------------------------------------------------------------------------------------------
#if !defined( ENGINE_DISABLE_AUDIO )

#if defined( _WIN64 )

#pragma comment( lib, "ThirdParty/fmod/fmod64_vc.lib" )

#else

#pragma comment( lib, "ThirdParty/fmod/fmod_vc.lib" )

#endif


//-----------------------------------------------------------------------------------------------
static FMOD_VECTOR ToFMODVec3( Vec3 const& v )
{
	FMOD_VECTOR fv;
	fv.x = ( v.x );
	fv.y = ( v.y );
	fv.z = ( v.z );

	return fv;
}


//-----------------------------------------------------------------------------------------------
AudioSystem::AudioSystem( AudioConfig const& config )
	:m_config( config )
	, m_fmodSystem( nullptr )
{
}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
}


//------------------------------------------------------------------------------------------------
void AudioSystem::Startup()
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->init( 512, FMOD_INIT_3D_RIGHTHANDED, nullptr );
	ValidateResult( result );
}


//------------------------------------------------------------------------------------------------
void AudioSystem::Shutdown()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath )
{
	std::map< std::string, SoundID >::iterator found = m_registeredSoundIDs.find( soundFilePath );
	if ( found != m_registeredSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_3D, nullptr, &newSound );
		if ( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registeredSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::StartSound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if ( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if ( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, true, &channelAssignedToSound );
	if ( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		playbackMode |= FMOD_2D;

		float frequency;
		channelAssignedToSound->setMode( playbackMode );
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
		channelAssignedToSound->setPaused( isPaused );
	}

	return ( SoundPlaybackID ) channelAssignedToSound;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if ( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to stop sound on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if ( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if ( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if ( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = ( FMOD::Channel* ) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if ( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if ( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", ( int ) result ) );
	}
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlayMusic( char const* bgname, float volume, bool loop )
{
	if ( m_activeBgMusic != MISSING_SOUND_ID )
	{
		StopMusic();
	}

	std::string musicPath = bgname;

	m_activeBgMusic = StartSound( CreateOrGetSound( musicPath ), loop, volume );

	return m_activeBgMusic;
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::SetMusicSpeed( float speed )
{
	if ( m_activeBgMusic == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	SetSoundPlaybackSpeed( m_activeBgMusic, speed );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::StopMusic()
{
	if ( m_activeBgMusic == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	StopSound( m_activeBgMusic );
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( SoundID soundID, float volume, bool loop /*= false */ )
{
	size_t numSounds = m_registeredSounds.size();
	if ( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if ( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, true, &channelAssignedToSound );
	if ( channelAssignedToSound )
	{
		int loopCount = loop ? -1 : 0;
		unsigned int playbackMode = loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		playbackMode |= FMOD_2D;

		float frequency;
		channelAssignedToSound->setMode( playbackMode );
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * 1.0f );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( 0.0f );
		channelAssignedToSound->setLoopCount( loopCount );
		channelAssignedToSound->setPaused( false );
	}

	return ( SoundPlaybackID ) channelAssignedToSound;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( char const* sfxName, float volume, bool loop /*= false */ )
{
	SoundID id = CreateOrGetSound( sfxName );
	return PlaySound( id, volume, loop );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::UpdateListeners( int count, AudioListenerInfo const* listeners )
{
	m_fmodSystem->set3DNumListeners( count );

	for ( int listenerNum = 0; listenerNum < count; listenerNum++ )
	{
		FMOD_VECTOR position = ToFMODVec3( listeners[ listenerNum ].m_position );
		FMOD_VECTOR velocity = ToFMODVec3( listeners[ listenerNum ].m_position );
		FMOD_VECTOR forward = ToFMODVec3( listeners[ listenerNum ].m_forward );
		FMOD_VECTOR up = ToFMODVec3( listeners[ listenerNum ].m_up );
		m_fmodSystem->set3DListenerAttributes( listenerNum, &position, &velocity, &forward, &up );
	}
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySoundAt( SoundID soundID, Vec3 const& pos, float volume /*= 1.0f*/, bool loop /*= false */ )
{
	size_t numSounds = m_registeredSounds.size();
	if ( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if ( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, true, &channelAssignedToSound );
	if ( channelAssignedToSound )
	{
		FMOD_VECTOR fpos = ToFMODVec3( pos );
		FMOD_VECTOR fvel = ToFMODVec3( Vec3::ZERO );

		int loopCount = loop ? -1 : 0;
		unsigned int playbackMode = loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		playbackMode |= FMOD_3D;

		float frequency;
		channelAssignedToSound->setMode( playbackMode );
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * 1.0f );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( 0.0f );
		channelAssignedToSound->setLoopCount( loopCount );
		channelAssignedToSound->set3DAttributes( &fpos, &fvel );
		channelAssignedToSound->setPaused( false );
	}

	return ( SoundPlaybackID ) channelAssignedToSound;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySoundAt( char const* sfxName, Vec3 const& pos, float volume /*= 1.0f*/, bool loop /*= false */ )
{
	SoundID id = CreateOrGetSound( sfxName );
	return PlaySoundAt( id, pos, volume, loop );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::UpdateSoundPosition( SoundPlaybackID id, Vec3 const& pos )
{
	UNUSED( id );
	UNUSED( pos );
}

#endif 

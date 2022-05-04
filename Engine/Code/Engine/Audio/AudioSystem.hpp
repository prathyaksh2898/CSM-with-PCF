#pragma once
#include "Engine/Math/Vec3.hpp"

#include "ThirdParty/fmod/fmod.hpp"

#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef   size_t SoundID;
typedef   size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = ( size_t ) ( -1 ); // for bad SoundIDs and SoundPlaybackIDs


//-----------------------------------------------------------------------------------------------
struct AudioConfig
{

};


//-----------------------------------------------------------------------------------------------
class AudioSystem;
struct Vec3;


//-----------------------------------------------------------------------------------------------
struct AudioListenerInfo
{
	Vec3 m_position;
	Vec3 m_forward;
	Vec3 m_up;
	Vec3 m_velocity;
};


//-----------------------------------------------------------------------------------------------
class AudioSystem
{
public:
	AudioSystem( AudioConfig const& config );
	virtual ~AudioSystem();

public:
	void						Startup();
	void						Shutdown();
	virtual void				BeginFrame();
	virtual void				EndFrame();

	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath );
	virtual SoundPlaybackID		StartSound( SoundID soundID, bool isLooped = false, float volume = 1.f, float balance = 0.0f, float speed = 1.0f, bool isPaused = false );
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)

	virtual void				ValidateResult( FMOD_RESULT result );

	SoundPlaybackID             PlayMusic( char const* bgname, float volume, bool loop );
	void                        SetMusicSpeed( float speed );
	void                        StopMusic();

	SoundPlaybackID             PlaySound( SoundID id, float volume, bool loop = false );
	SoundPlaybackID             PlaySound( char const* sfxName, float volume, bool loop = false );

	void                        UpdateListeners( int count, AudioListenerInfo const* listeners );
	SoundPlaybackID             PlaySoundAt( SoundID soundID, Vec3 const& pos, float volume = 1.0f, bool loop = false );
	SoundPlaybackID             PlaySoundAt( char const* sfxName, Vec3 const& pos, float volume = 1.0f, bool loop = false );
	void                        UpdateSoundPosition( SoundPlaybackID id, Vec3 const& pos );

protected:
	AudioConfig							m_config;
	FMOD::System* m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;

	SoundPlaybackID                     m_activeBgMusic;
};


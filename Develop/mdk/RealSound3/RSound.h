#ifndef _RSOUND
#define _RSOUND

#include "RSample.h"
#include "RSoundTimer.h"
#include "fmod.hpp"



/// State of sound
enum
{
	RSD_SOUNDSTATE_STOP			= 0,				///< Stopped
	RSD_SOUNDSTATE_PLAY_FADEOUT,					///< Playing with fade-out
	RSD_SOUNDSTATE_PLAY_FADEIN,						///< Playing with fade-in
	RSD_SOUNDSTATE_PLAY,							///< Playing
	RSD_SOUNDSTATE_DELETE							///< Will be deleted
};





/// RVolumeFader
struct RVolumeFader
{
	TIMEVAL		m_tStartTime;
	float		m_fVolume;

	void Start( TIMEVAL tTime, float fVolume)
	{
		m_tStartTime = tTime;
		m_fVolume = fVolume;
	}
};





/// class : RSound
class RSound
{
	friend class RealSound3;


	/// Member variables
protected:
	RSample*			m_pSample;					///< Pointer of sample
	FMOD::Channel*		m_pChannel;					///< Pointer of channel
	unsigned char		m_nState;					///< State of sound
	RVolumeFader*		m_pVolumeFader;				///< Volume fader
	


	/// Member functions
public:
	// Constructor/destructor
	RSound();
	virtual ~RSound();

	// Create/destroy
	bool _create( FMOD::System* pSystem, RSample* pSample);
	bool _destroy();

	// Play and Stop
	void Play();
	void Stop();


	// Update this sound
protected:
	void _update( RealSound3* pSystem);
	void _update_volume();
	float _update_rolloff( float fDistance);


	/// Get/Set functions
public:
	// Get sample pointer
	RSample* GetSample() const;

	// Get sound pointer
	FMOD::Sound* GetSound() const;

	// Get channel pointer
	FMOD::Channel* GetChannel() const;

	// Get file name
	const char* GetFileName() const;

	// Get type
	int GetType() const;

	// Get loop
	bool GetLoop() const;

	// Get/Set volume
	void SetVolume( float fVolume);
	float GetVolume() const;

	// Get priority
	int GetPriority() const;

	// Get/Set Distance
	void SetDistance( float fMinDist, float fMaxDist);
	void GetDistance( float* fMinDist, float* fMaxDist) const;

	// Get unique sound
	bool GetUnique() const;

	// Get sound position and velocity
	virtual bool GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel);

	// Get virtual sound
	bool IsVirtual() const;

	// Is will delete this channel
	bool IsDelete() const;

	// Is playing
	bool IsPlaying() const;

	// Get playing time
	unsigned int GetPlayingTime() const;

	// Get/Set playing position
	void SetPlayingPosition( unsigned int pos);
	unsigned int GetPlayingPosition() const;


	/// Event handler
protected:
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnPlay();
	virtual void OnStop();
	virtual void OnUpdate();
};


#endif

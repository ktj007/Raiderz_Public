#include "stdafx.h"
#include "RSound.h"
#include "RSoundError.h"
#include "RealSound3.h"
#include <math.h>


// Sound fade time
#define SOUND_FADETIME				2000.0


// RSound
RSound::RSound()
{
	m_pSample = NULL;
	m_pChannel = NULL;
	m_nState = RSD_SOUNDSTATE_STOP;
	m_pVolumeFader = NULL;
}


// ~RSound
RSound::~RSound()
{
	if ( m_nState != RSD_SOUNDSTATE_DELETE)		_destroy();
}


// _create
bool RSound::_create( FMOD::System* pSystem, RSample* pSample)
{
	if ( pSystem == NULL  ||  pSample == NULL)
		return false;


	// Set sound
	m_pSample = pSample;
	FMOD::Sound* _sound = pSample->GetSound();


	// Get type
	int nType = pSample->GetType();


	// Get channel
	FMOD_RESULT result;
	result = pSystem->playSound( FMOD_CHANNEL_FREE, _sound, true, &m_pChannel);
	if ( _ERROR( result)  ||  m_pChannel == NULL)
	{
		_ERRORMSG( result);
		return false;
	}


	// Set volume
	float fVolume = pSample->GetVolume();
	if ( nType == RSD_SOUNDTYPE_BGM  ||  pSample->GetLoop() == true)
	{
		m_pVolumeFader = new RVolumeFader;
		m_pVolumeFader->Start( GetCurTime(), fVolume);

		fVolume = 0.0f;
	}
	m_pChannel->setVolume( fVolume);


	// Set 3D attribute
	if ( nType == RSD_SOUNDTYPE_EFFECT)
	{
		m_pChannel->setPriority( pSample->GetPriority());
		m_pChannel->set3DMinMaxDistance( pSample->m_fMinDist, pSample->m_fMaxDist);

		FMOD_VECTOR _pos, _vel;
		if ( GetPosition( &_pos, &_vel) == true)
		{
			_vel.x = 0.0f;
			_vel.y = 0.0f;
			_vel.z = 0.0f;
			m_pChannel->set3DAttributes( &_pos, &_vel);
		}
	}


	// Set user data
	m_pChannel->setUserData( static_cast< void*>( this));


	// Increase reference count
	m_pSample->_inc_reference();


	// Call event handler
	OnCreate();

	return true;
}


// _destroy
bool RSound::_destroy()
{
	if ( m_nState == RSD_SOUNDSTATE_DELETE)		return false;


	// Set state
	m_nState = RSD_SOUNDSTATE_DELETE;


	// Reset channel
	if ( m_pChannel != NULL)
	{
		// Delete user data
		m_pChannel->setUserData( NULL);


		// Stop channel
		bool bPlaying;
		m_pChannel->isPlaying( &bPlaying);
		if ( bPlaying == true)		m_pChannel->stop();
		m_pChannel = NULL;
	}


	// Delete volume fader
	if ( m_pVolumeFader != NULL)
	{
		delete m_pVolumeFader;
		m_pVolumeFader = NULL;
	}


	// Decrease reference count
	if ( m_pSample != NULL)			m_pSample->_dec_reference();


	// Call event handler
	OnDestroy();

	return true;
}


// Play
void RSound::Play()
{
	if ( m_nState != RSD_SOUNDSTATE_STOP  ||  m_pChannel == NULL)
		return;


	// Set state
	if ( m_pSample->GetType() == RSD_SOUNDTYPE_BGM  ||  m_pSample->GetLoop() == true)
		m_nState = RSD_SOUNDSTATE_PLAY_FADEIN;
	else
		m_nState = RSD_SOUNDSTATE_PLAY;


	// Set last access time
	m_pSample->m_tLastAccessTime = GetCurTime( TIMEUNIT_MIN);


	// Play
	m_pChannel->setPaused( false);


	// Call event handler
	OnPlay();
}


// Stop
void RSound::Stop()
{
	if ( m_nState == RSD_SOUNDSTATE_DELETE  ||  m_pChannel == NULL)
		return;


	switch ( m_nState)
	{
	case RSD_SOUNDSTATE_STOP :
		{
			_destroy();
			break;
		}

	case RSD_SOUNDSTATE_PLAY_FADEIN :
	case RSD_SOUNDSTATE_PLAY :
		{
			if ( m_pVolumeFader == NULL)	m_pVolumeFader = new RVolumeFader;
			m_pVolumeFader->Start( GetCurTime(), GetVolume());

			m_nState = RSD_SOUNDSTATE_PLAY_FADEOUT;

			// Call event handler
			OnStop();

			break;
		}
	}
}


// Update
void RSound::_update( RealSound3* pSystem)
{
	if ( pSystem == NULL  ||  m_nState == RSD_SOUNDSTATE_DELETE  ||  m_pChannel == NULL)
		return;


	// Update sound
	if ( GetType() == RSD_SOUNDTYPE_EFFECT)
	{
		// Set position and velocity
		FMOD_VECTOR _pos, _vel;
		if ( GetPosition( &_pos, &_vel) == true)
		{
			// Get listener
			FMOD_VECTOR _listener;
			pSystem->GetSystem()->get3DListenerAttributes( 0, &_listener, NULL, NULL, NULL);


			// Get pan distance
			float fPanDist = pSystem->GetPanningDistance();


			// Set position and velocity
			m_pChannel->set3DAttributes( &_pos, &_vel);


			// Get min/max distance
			float fMinDist, fMaxDist;
			m_pChannel->get3DMinMaxDistance( &fMinDist, &fMaxDist);
			fMinDist = fMaxDist * 0.2f;
			fMaxDist = fMaxDist * 0.8f;


			// Get distance
			FMOD_VECTOR _diff;
			_diff.x = abs( _pos.x - _listener.x);
			_diff.y = abs( _pos.y - _listener.y);
			_diff.z = abs( _pos.z - _listener.z);
			float fDistance = sqrt( _diff.x * _diff.x + _diff.y * _diff.y + _diff.z * _diff.z);


			// Get pan level
			float fPanLevel;
			if ( fDistance < fMinDist)				fPanLevel = 0.0f;
			else if ( fDistance > fMaxDist)			fPanLevel = 1.0f;
			else
			{
				float fRatio = ( fDistance - fMinDist) / ( fMaxDist - fMinDist);
//				fPanLevel = fRatio;					// Linear
				fPanLevel = sqrt( fRatio);			// Log
			}
			m_pChannel->set3DPanLevel( fPanLevel);
		}
	}


	// Call event handler
	OnUpdate();
}


// _update_volume
void RSound::_update_volume()
{
	if ( m_nState == RSD_SOUNDSTATE_DELETE  ||  m_pChannel == NULL  ||  m_pVolumeFader == NULL)
		return;


	// Update volume fade
	switch ( m_nState)
	{
	case RSD_SOUNDSTATE_PLAY_FADEIN :
		{
			TIMEVAL tElapsed = GetCurTime() - m_pVolumeFader->m_tStartTime;
			if ( tElapsed > SOUND_FADETIME)
			{
				m_pChannel->setVolume( m_pVolumeFader->m_fVolume);
				m_nState = RSD_SOUNDSTATE_PLAY_FADEIN;

				delete m_pVolumeFader;
				m_pVolumeFader = NULL;
			}
			else
			{
				float fRatio = (float)( tElapsed / SOUND_FADETIME);
				float fVolume = m_pVolumeFader->m_fVolume * pow( fRatio, 2);
				m_pChannel->setVolume( fVolume);
			}
			break;
		}

	case RSD_SOUNDSTATE_PLAY_FADEOUT :
		{
			TIMEVAL tElapsed = GetCurTime() - m_pVolumeFader->m_tStartTime;
			if ( tElapsed > SOUND_FADETIME)
			{
				m_pChannel->setVolume( 0.0);
				m_nState = RSD_SOUNDSTATE_STOP;

				_destroy();
			}
			else
			{
				float fRatio = (float)( 1.0 - tElapsed / SOUND_FADETIME);
				float fVolume = m_pVolumeFader->m_fVolume * pow( fRatio, 2);
				m_pChannel->setVolume( fVolume);
			}
			break;
		}
	}
}


// _update_rolloff
float RSound::_update_rolloff( float fDistance)
{
	if ( m_nState == RSD_SOUNDSTATE_DELETE  ||  m_pChannel == NULL)
		return 1.0f;


	float fMinDist, fMaxDist;
	m_pChannel->get3DMinMaxDistance( &fMinDist, &fMaxDist);


	// Get roll-off and pan level
	float fRollOff;
	if ( fDistance <= fMinDist)			fRollOff = 1.0f;
	else if ( fDistance > fMaxDist)		fRollOff = 0.0f;
	else
	{
		float fRatio = 1.0f - (fDistance - fMinDist) / (fMaxDist - fMinDist);

		FMOD_MODE _mode;
		m_pChannel->getMode( &_mode);
		if ( _mode & FMOD_3D_LINEARROLLOFF)		fRollOff = fRatio;
		else									fRollOff = pow( fRatio, 2);
	}

	return fRollOff;
}


// GetSample
RSample* RSound::GetSample() const
{
	return m_pSample;
}


// GetSound
FMOD::Sound* RSound::GetSound() const
{
	if ( m_pSample == NULL)		return NULL;
	return m_pSample->GetSound();
}


// GetChannel
FMOD::Channel* RSound::GetChannel() const
{
	return m_pChannel;
}


// GetFileName
const char* RSound::GetFileName() const
{
	if ( m_pSample == NULL)		return NULL;
	return m_pSample->GetFileName();
}


// GetType
int RSound::GetType() const
{
	if ( m_pSample == NULL)		return RSD_SOUNDTYPE_NONE;
	return m_pSample->GetType();
}


// GetLoop
bool RSound::GetLoop() const
{
	if ( m_pSample == NULL)		return false;
	return m_pSample->GetLoop();
}


// SetVolume
void RSound::SetVolume( float fVolume)
{
	if ( m_pChannel != NULL)
		m_pChannel->setVolume( fVolume);
}


// GetVolume
float RSound::GetVolume() const
{
	float fVolume = 0.0f;
	if ( m_pChannel != NULL)
		m_pChannel->getVolume( &fVolume);

	return fVolume;
}


// GetPriority
int RSound::GetPriority() const
{
	if ( m_pSample == NULL)		return 128;
	return m_pSample->GetPriority();
}


// SetDistance
void RSound::SetDistance( float fMinDist, float fMaxDist)
{
	if ( m_pChannel != NULL)
		m_pChannel->set3DMinMaxDistance( fMinDist, fMaxDist);
}


// GetDistance
void RSound::GetDistance( float* fMinDist, float* fMaxDist) const
{
	if ( m_pChannel != NULL)
		m_pChannel->get3DMinMaxDistance( fMinDist, fMaxDist);
}


// GetUnique
bool RSound::GetUnique() const
{
	if ( m_pSample == NULL)		return false;
	return m_pSample->GetUnique();
}


// GetPosition
bool RSound::GetPosition( FMOD_VECTOR* pPos, FMOD_VECTOR* pVel)
{
	pPos->x = 0.0f;
	pPos->y = 0.0f;
	pPos->z = 0.0f;
	
	pVel->x = 0.0f;
	pVel->y = 0.0f;
	pVel->z = 0.0f;

	return true;
}


// IsVirtual
bool RSound::IsVirtual() const
{
	bool bVirtual = false;
	if ( m_pChannel != NULL)
		m_pChannel->isVirtual( &bVirtual);

	return bVirtual;
}


// IsDelete
bool RSound::IsDelete() const
{
	return ( m_nState == RSD_SOUNDSTATE_DELETE  ?  true : false);
}


// IsPlaying
bool RSound::IsPlaying() const
{
	bool bPlaying = false;
	if ( m_pChannel != NULL)
		m_pChannel->isPlaying( &bPlaying);

	return false;
}


// GetPlayingTime
unsigned int RSound::GetPlayingTime() const
{
	unsigned int nLength = 0;

	if ( m_pChannel != NULL  &&  m_pSample != NULL)
	{
		bool bPlaying = false;
		m_pChannel->isPlaying( &bPlaying);

		if ( bPlaying == true)
		{
			FMOD::Sound* pSound = m_pSample->GetSound();
			if ( pSound != NULL)
				pSound->getLength( &nLength, FMOD_TIMEUNIT_MS);
		}
	}

	return nLength;
}


// SetPlayingPosition
void RSound::SetPlayingPosition( unsigned int pos)
{
	if ( m_pChannel != NULL)
		m_pChannel->setPosition( pos, FMOD_TIMEUNIT_MS);
}


// GetPlayingPosition
unsigned int RSound::GetPlayingPosition() const
{
	unsigned int nPosition = 0;
	if ( m_pChannel != NULL)
		m_pChannel->getPosition( &nPosition, FMOD_TIMEUNIT_MS);

	return nPosition;
}


// OnCreate
void RSound::OnCreate()
{
}


// OnDestroy
void RSound::OnDestroy()
{
}


// OnPlay
void RSound::OnPlay()
{
}


// OnStop
void RSound::OnStop()
{
}


// OnUpdate
void RSound::OnUpdate()
{
}

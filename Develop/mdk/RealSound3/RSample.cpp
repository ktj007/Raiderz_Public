#include "stdafx.h"
#include "RSample.h"
#include "RSoundError.h"



// RSample
RSample::RSample()
{
	m_pSound = NULL;
	m_nType = RSD_SOUNDTYPE_NONE;
	m_bLoop = false;
	m_fVolume = 1.0f;
	m_nPriority = 128;
	m_fMinDist = 100.0f;
	m_fMaxDist = 1000.0f;
	m_bUnique = false;
	m_nReference = 0;
	m_tLastAccessTime = 0.0;
}


// RSample
RSample::RSample( const RSampleInfo& _info)
{
	m_pSound = NULL;
	m_strFileName = _info.strFileName;
	m_nType = _info.nType;
	m_bLoop = _info.bLoop;
	m_fVolume = _info.fVolume;
	m_nPriority = _info.nPriority;
	m_fMinDist = _info.fMinDist;
	m_fMaxDist = _info.fMaxDist;
	m_bUnique = _info.bUnique;
	m_nReference = 0;
	m_tLastAccessTime = 0.0;
}


// ~RSample
RSample::~RSample()
{
	_release_sound();
}


// _create_sound
bool RSample::_create_sound( FMOD::System* pSystem, int nMode)
{
	if ( pSystem == NULL)		return false;
	if ( m_pSound != NULL)		return true;


	// Call event handler
	OnPreCreateSound( &nMode);


	// Create sound
	FMOD_RESULT result;
	switch ( m_nType)
	{
	case RSD_SOUNDTYPE_GENERIC :
		{
			if ( nMode & FMOD_OPENMEMORY)
			{
				if ( GetDataPtr() == NULL)
					OnLoadData();

				const char* pData = GetDataPtr();
				if ( pData != NULL)
				{
					FMOD_CREATESOUNDEXINFO ex;
					memset( &ex, 0, sizeof( FMOD_CREATESOUNDEXINFO));
					ex.cbsize = sizeof( FMOD_CREATESOUNDEXINFO);
					ex.length = GetDataSize();

					result = pSystem->createSound( pData, nMode, &ex, &m_pSound);
				}
				else
				{
					result = FMOD_ERR_INVALID_PARAM;
				}

				if ( GetDataPtr() != NULL)
					OnReleaseData();
			}
			else
			{
				result = pSystem->createSound( m_strFileName.c_str(), nMode, NULL, &m_pSound);
			}

			break;
		}


	case RSD_SOUNDTYPE_BGM :
		{
			if ( nMode & FMOD_OPENMEMORY)
			{
				if ( GetDataPtr() == NULL)
					OnLoadData();

				const char* pData = GetDataPtr();
				if ( pData != NULL)
				{
					FMOD_CREATESOUNDEXINFO ex;
					memset( &ex, 0, sizeof( FMOD_CREATESOUNDEXINFO));
					ex.cbsize = sizeof( FMOD_CREATESOUNDEXINFO);
					ex.length = GetDataSize();

					result = pSystem->createStream( pData, nMode, &ex, &m_pSound);
				}
				else
				{
					result = FMOD_ERR_INVALID_PARAM;
				}
			}
			else
			{
				result = pSystem->createStream( m_strFileName.c_str(), nMode, NULL, &m_pSound);
			}

			break;
		}


	case RSD_SOUNDTYPE_EFFECT :
		{
			if ( nMode & FMOD_OPENMEMORY)
			{
				if ( GetDataPtr() == NULL)
					OnLoadData();

				const char* pData = GetDataPtr();
				if ( pData != NULL)
				{
					FMOD_CREATESOUNDEXINFO ex;
					memset( &ex, 0, sizeof( FMOD_CREATESOUNDEXINFO));
					ex.cbsize = sizeof( FMOD_CREATESOUNDEXINFO);
					ex.length = GetDataSize();

					result = pSystem->createSound( pData, nMode, &ex, &m_pSound);
				}
				else
				{
					result = FMOD_ERR_INVALID_PARAM;
				}

				if ( GetDataPtr() != NULL)
					OnReleaseData();
			}
			else
			{
				result = pSystem->createSound( m_strFileName.c_str(), nMode, NULL, &m_pSound);
			}

			break;
		}
	}


	// Check error
	if ( _ERROR( result))
	{
		_ERRORMSG( result, m_strFileName.c_str());
		return false;
	}


	// Call event handler
	OnCreateSound();

	return true;
}


// _release_sound
bool RSample::_release_sound()
{
	// Release sound
	if ( m_pSound != NULL)
	{
		m_pSound->release();
		m_pSound = NULL;

		// Call event handler
		OnReleaseSound();
	}


	// Release sound data
	if ( GetDataPtr() != NULL)
	{
		// Call event handler
		OnReleaseData();
	}

	return true;
}


// _inc_reference
void RSample::_inc_reference()
{
	m_nReference++;
}


// _dec_reference
void RSample::_dec_reference()
{
	m_nReference--;
}


// _update_sample
void RSample::_update_sample()
{
	if ( m_pSound == NULL  ||  m_nReference > 0)
		return;

	if ( ( GetCurTime( TIMEUNIT_MIN) - m_tLastAccessTime) > 5.0 /*5 min*/)
		_release_sound();
}


// GetSound
FMOD::Sound* RSample::GetSound() const
{
	return m_pSound;
}


// GetDataPtr
const char* RSample::GetDataPtr()
{
	return NULL;
}


// GetDataSize
const size_t RSample::GetDataSize()
{
	return 0;
}


// GetFileName
const char* RSample::GetFileName() const
{
	return m_strFileName.c_str();
}


// GetType
int RSample::GetType() const
{
	return m_nType;
}


// GetLoop
bool RSample::GetLoop() const
{
	return m_bLoop;
}


// GetVolume
float RSample::GetVolume() const
{
	return m_fVolume;
}


// GetPriority
int RSample::GetPriority() const
{
	return m_nPriority;
}


// GetMinDist
float RSample::GetMinDist() const
{
	return m_fMinDist;
}


// GetMaxDist
float RSample::GetMaxDist() const
{
	return m_fMaxDist;
}


// GetUnique
bool RSample::GetUnique() const
{
	return m_bUnique;
}


// GetReference
int RSample::GetReference() const
{
	return m_nReference;
}


// GetLastAccessTime
TIMEVAL RSample::GetLastAccessTime() const
{
	return m_tLastAccessTime;
}

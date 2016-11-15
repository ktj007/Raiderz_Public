#include "stdafx.h"
#include "RealSound3.h"
#include "RSoundError.h"
#include <math.h>


// Maximum sound reference count
#define MAX_REFERENCE				10


// Name of the channel groups
static const char* STR_CHANNELGROUP[] = { "_MASTER_", "_GENERIC_", "_BGM_", "_EFFECT_" };


// Name of the effect channel groups
static const char* STR_EFFECTCHNLGRP[] = { "_GENERIC_", "_EMPHASIZE_" };


// Reverb preset
static FMOD_REVERB_PROPERTIES REVERB_PRESET[] =
{
	FMOD_PRESET_OFF,
	FMOD_PRESET_GENERIC,
	FMOD_PRESET_PADDEDCELL,
	FMOD_PRESET_ROOM,
	FMOD_PRESET_BATHROOM,
	FMOD_PRESET_LIVINGROOM,
	FMOD_PRESET_STONEROOM,
	FMOD_PRESET_AUDITORIUM,
	FMOD_PRESET_CONCERTHALL,
	FMOD_PRESET_CAVE,
	FMOD_PRESET_ARENA,
	FMOD_PRESET_HANGAR,
	FMOD_PRESET_CARPETTEDHALLWAY,
	FMOD_PRESET_HALLWAY,
	FMOD_PRESET_STONECORRIDOR,
	FMOD_PRESET_ALLEY,
	FMOD_PRESET_FOREST,
	FMOD_PRESET_CITY,
	FMOD_PRESET_MOUNTAINS,
	FMOD_PRESET_QUARRY,
	FMOD_PRESET_PLAIN,
	FMOD_PRESET_PARKINGLOT,
	FMOD_PRESET_SEWERPIPE,
	FMOD_PRESET_UNDERWATER
};


// State of fade in/out
enum
{
	FADE_NONE		= 0,
	FADE_IN,
	FADE_OUT
};








// RChannelGrpVolFader
RChannelGrpVolFader::RChannelGrpVolFader()
{
	m_pChannelGroup = NULL;
	m_nState = FADE_NONE;
	m_tStartTime = 0.0;
	m_tFadeTime = 2000.0f;
	m_fStartVolume = 0.0f;
	m_fEndVolume = 0.0f;
}


// Create
void RChannelGrpVolFader::Create( FMOD::ChannelGroup* pChannelGroup, int nFadeTime)
{
	m_pChannelGroup = pChannelGroup;
	m_tFadeTime = (TIMEVAL)nFadeTime;
}


// Destroy
void RChannelGrpVolFader::Destroy()
{
	m_pChannelGroup = NULL;
	m_nState = FADE_NONE;
	m_tStartTime = 0.0;
	m_tFadeTime = 2000.0f;
	m_fStartVolume = 0.0f;
	m_fEndVolume = 0.0f;
}


// FadeIn
void RChannelGrpVolFader::FadeIn( float fEndVolume)
{
	if ( m_nState == FADE_IN)		return;

	float fStartVolume = 0.0f;
	m_pChannelGroup->getVolume( &fStartVolume);

	if ( fEndVolume <= fStartVolume)
		return;

	m_nState = FADE_IN;
	m_tStartTime = GetCurTime();
	m_fStartVolume = fStartVolume;
	m_fEndVolume = fEndVolume;
}


// FadeOut
void RChannelGrpVolFader::FadeOut( float fEndVolume)
{
	if ( m_nState == FADE_OUT)		return;

	float fStartVolume = 0.0f;
	m_pChannelGroup->getVolume( &fStartVolume);

	if ( fEndVolume >= fStartVolume)
		return;

	m_nState = FADE_OUT;
	m_tStartTime = GetCurTime();
	m_fStartVolume = fStartVolume;
	m_fEndVolume = fEndVolume;
}


// Update
void RChannelGrpVolFader::Update()
{
	if ( m_pChannelGroup == NULL  ||  m_nState == FADE_NONE)
		return;


	TIMEVAL tElapsed = GetCurTime() - m_tStartTime;
	if ( tElapsed > m_tFadeTime)
	{
		m_pChannelGroup->setVolume( m_fEndVolume);
		m_nState = FADE_NONE;

		return;
	}


	float fRatio;
	if ( m_nState == FADE_IN)		fRatio = (float)pow( tElapsed / m_tFadeTime, 2);
	else							fRatio = 1.0f - (float)pow( (m_tFadeTime - tElapsed) / m_tFadeTime, 2);

	float fVolume = m_fStartVolume + ( m_fEndVolume - m_fStartVolume) * fRatio;
	m_pChannelGroup->setVolume( fVolume);
}











// RealSound3
RealSound3::RealSound3()
{
	m_bCreate = false;
	m_pSystem = NULL;
	m_bHardwareAccelerated = true;
	m_fPanningDistance = 1000.0f;
	m_nRollOffModel = RSD_ROLLOFF_LOG;
	m_pCurrentBGM = NULL;
	m_bSilence = false;

	for ( int i = 0;  i < RSD_CHANNELGROUP_MAX;  i++)
	{
		m_pChannelGroups[ i] = NULL;
		m_fVolume[ i] = 1.0f;
		m_bMute[ i] = false;
	}

	for ( int i = 0;  i < RSD_EFFECTCHNLGRP_MAX;  i++)
		m_pEffectChnlGrp[ i] = NULL;

	m_SystemInfo.nMaxChannels = 100;
	m_SystemInfo.nSoftwareChannels = 10;
	m_SystemInfo.nSpeakerMode = RSD_SPEAKERMODE_STEREO;
	m_SystemInfo.nSampleRate = 44100;

	m_Reverb.bEnable = false;
	m_Reverb.nAmbient = RSD_REVERB_OFF;

	m_Compressor.bEnable = false;
	m_Compressor.pDSP = NULL;
	m_Compressor.fThreshold = -4.0f;
	m_Compressor.fAttack = 10.0f;
	m_Compressor.fRelease = 300.0f;
	m_Compressor.fGainMakeUp = 0.0f;

	m_Thread.hThread = NULL;
	m_Thread.hEventDestroy = NULL;
	m_Thread.bExit = true;

	m_ShutdownInfo.bShutdown = false;
}


// ~RealSound3
RealSound3::~RealSound3()
{
	Destroy();
}


// Create
bool RealSound3::Create( int nMaxChannels, int nSoftwareChannels, int nSpeakerMode, int nSampleRate)
{
	if ( m_bCreate == true)				return false;


	_MESSAGE( "Open RealSound engine.");


	// Create sound system
	FMOD_RESULT result = FMOD::System_Create( &m_pSystem);
	if ( _ERROR( result))				goto rsound_error_create;


	// Check version
	unsigned int _version;
	result = m_pSystem->getVersion( &_version);
	if ( _ERROR( result))				goto rsound_error_create;
	else if ( _version < FMOD_VERSION)
	{
		printf( "[FMOD]-[ERROR] You are using an old version of FMOD %08x. This program requires %08x\n", _version, FMOD_VERSION);
		Destroy();
		return false;
	}


	// Check driver counts
	int _drivers;
	result = m_pSystem->getNumDrivers( &_drivers);
	if ( _ERROR( result))				goto rsound_error_create;
	else if ( _drivers > 0)
	{
		result = m_pSystem->setOutput( FMOD_OUTPUTTYPE_DSOUND);
		if ( _ERROR( result))			goto rsound_error_create;

		FMOD_CAPS _caps;
		FMOD_SPEAKERMODE _speaker;
		result = m_pSystem->getDriverCaps( 0, &_caps, 0, 0, &_speaker); 
		if ( _ERROR( result))			goto rsound_error_create;

		_MESSAGE( "Driver capabilities : 0x%08X", _caps);


		// Set speaker mode
		switch ( nSpeakerMode)
		{
		case RSD_SPEAKERMODE_STEREO :
			_speaker = FMOD_SPEAKERMODE_STEREO;
			break;

		case RSD_SPEAKERMODE_QUAD :
			_speaker = FMOD_SPEAKERMODE_QUAD;
			break;

		case RSD_SPEAKERMODE_5POINT1 :
			_speaker = FMOD_SPEAKERMODE_5POINT1;
			break;

		case RSD_SPEAKERMODE_7POINT1 :
			_speaker = FMOD_SPEAKERMODE_7POINT1;
			break;
		}
		result = m_pSystem->setSpeakerMode( _speaker);
		if ( _ERROR( result))			goto rsound_error_create;


		if ( _caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			_MESSAGE( "Bad latency.");

			result = m_pSystem->setDSPBufferSize( 1024, 10);
			if ( _ERROR( result))		goto rsound_error_create;
		}

		if ( _caps & FMOD_CAPS_REVERB_LIMITED)
			_MESSAGE( "Device supports some form of limited hardware reverb.");


		char _name[ 256];
		result = m_pSystem->getDriverInfo( 0, _name, 256, 0);
		if ( _ERROR( result))			goto rsound_error_create;
		else if ( strstr( _name, "SigmaTel"))			// Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it.
		{
			result = m_pSystem->setSoftwareFormat( nSampleRate, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			if ( _ERROR( result))		goto rsound_error_create;
		}
		else
		{
			// Sampling rate = 48000 -> 44100
			result = m_pSystem->setSoftwareFormat( nSampleRate, FMOD_SOUND_FORMAT_PCM16, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			if ( _ERROR( result))		goto rsound_error_create;
		}
	}
	else
	{
		result = m_pSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND);
		if ( _ERROR( result))			goto rsound_error_create;
	}


	// Set software channels
	result = m_pSystem->setSoftwareChannels( nSoftwareChannels);
	if ( _ERROR( result))				goto rsound_error_create;


	// Set hardware channels
//	result = m_pSystem->setHardwareChannels( ... );
//	if ( _ERROR( result))				goto rsound_error_create;


	// Initialize sound system
	result = m_pSystem->init( nMaxChannels, FMOD_INIT_3D_RIGHTHANDED, 0);
	if ( result == FMOD_ERR_OUTPUT_CREATEBUFFER)		// Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo...
	{
		result = m_pSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO);
		if ( _ERROR( result))			goto rsound_error_create;

		result = m_pSystem->init( nMaxChannels, FMOD_INIT_NORMAL, 0);
		if ( _ERROR( result))			goto rsound_error_create;
	}
	else if ( _ERROR( result))			goto rsound_error_create;


	// Set roll-off callback
	m_pSystem->set3DRolloffCallback( RealSound3::_callback_rolloff);


	// Create channel groups
	for ( int i = 0;  i < RSD_CHANNELGROUP_MAX;  i++)
	{
		if ( i == RSD_CHANNELGROUP_MASTER)
		{
			result = m_pSystem->getMasterChannelGroup( &m_pChannelGroups[ RSD_CHANNELGROUP_MASTER]);
			if ( _ERROR( result))		goto rsound_error_create;
		}
		else
		{
			result = m_pSystem->createChannelGroup( STR_CHANNELGROUP[ i], &m_pChannelGroups[ i]);
			if ( _ERROR( result))		goto rsound_error_create;

			result = m_pChannelGroups[ RSD_CHANNELGROUP_MASTER]->addGroup( m_pChannelGroups[ i]);
			if ( _ERROR( result))		goto rsound_error_create;
		}

		m_pChannelGroups[ i]->setVolume( m_fVolume[ i]);
		m_pChannelGroups[ i]->setMute( m_bMute[ i]);
	}


	// Create effect channel groups
	for ( int i = 0;  i < RSD_EFFECTCHNLGRP_MAX;  i++)
	{
		result = m_pSystem->createChannelGroup( STR_EFFECTCHNLGRP[ i], &m_pEffectChnlGrp[ i]);
		if ( _ERROR( result))			goto rsound_error_create;

		result = m_pChannelGroups[ RSD_CHANNELGROUP_EFFECT]->addGroup( m_pEffectChnlGrp[ i]);
		if ( _ERROR( result))			goto rsound_error_create;

		m_pEffectChnlGrp[ i]->setVolume( 1.0f);
	}


	// Set channel group volume fader
	m_MasterChnlGrpVolFader.Create( m_pChannelGroups[ RSD_CHANNELGROUP_MASTER], 2000);
	m_EffectChnlGrpVolFader.Create( m_pEffectChnlGrp[ RSD_EFFECTCHNLGRP_GENERIC], 700);


	// Initialize critical section
	InitializeCriticalSection( &m_Thread.csLock);


	// Reset scan list iterators
	m_itrScanSample = m_vSamples.begin();
	m_itrScanSound = m_vSounds.begin();


	// Create thread
	m_Thread.bExit = false;
	m_Thread.hThread = CreateThread( NULL, 0, _thread, this, 0, NULL);
	if ( m_Thread.hThread == NULL)
	{
		_MESSAGE( "- [ERROR] Cannot create thread.");
		goto rsound_error_create;
	}


	// Output system info
	int _SoftwareChannels = 0;
	m_pSystem->getSoftwareChannels( &_SoftwareChannels);
	int _HardwareChannels = 0;
	int _Hardware2DChannels = 0;
	int _Hardware3DChannels = 0;
	m_pSystem->getHardwareChannels( &_Hardware2DChannels, &_Hardware3DChannels, &_HardwareChannels);
	_MESSAGE( "Software channels : %d channels,  Hardware channels : %d(2D) + %d(3D) = %d channels", _SoftwareChannels, _Hardware2DChannels, _Hardware3DChannels, _HardwareChannels);


	// Set system info
	m_bCreate = true;
	m_SystemInfo.nMaxChannels = nMaxChannels;
	m_SystemInfo.nSoftwareChannels = nSoftwareChannels;
	m_SystemInfo.nSpeakerMode = nSpeakerMode;
	m_SystemInfo.nSampleRate = nSampleRate;


	// Initialize variables
	m_pCurrentBGM = NULL;


	// Call event handler
	OnCreate();

	return true;



rsound_error_create:

	_ERRORMSG( result);

	Destroy();

	return false;
}


// Destroy
bool RealSound3::Destroy()
{
	if ( m_bCreate == false)		return false;


	_MESSAGE( "Close RealSound engine.");


	// Call event handler
	OnDestroy();


	// Delete thread
	if ( m_Thread.hThread != NULL)
	{
		m_Thread.bExit = true;

		if ( WaitForSingleObject( m_Thread.hEventDestroy, 5000) == WAIT_TIMEOUT)
			TerminateThread( m_Thread.hThread, 0);

		CloseHandle( m_Thread.hThread);
		m_Thread.hThread = NULL;

		CloseHandle( m_Thread.hEventDestroy);
		m_Thread.hEventDestroy = NULL;
	}


	// Remove all samples
	RemoveAllSamples();


	// Delete critical section
	DeleteCriticalSection( &m_Thread.csLock);


	// Clear reverb
	if ( m_Reverb.vLocal.empty() == false)
	{
		for ( vector< FMOD::Reverb*>::iterator itr = m_Reverb.vLocal.begin();  itr != m_Reverb.vLocal.end();  itr++)
			(*itr)->release();

		m_Reverb.vLocal.clear();
	}
	m_Reverb.bEnable = false;


	// Clear compressor
	if ( m_Compressor.pDSP != NULL)
	{
		m_Compressor.pDSP->remove();
		m_Compressor.pDSP->release();
		m_Compressor.pDSP = NULL;
	}
	m_Compressor.bEnable = false;


	// Release channel group volume fader
	m_MasterChnlGrpVolFader.Destroy();
	m_EffectChnlGrpVolFader.Destroy();


	// Release effect channel groups
	for ( int i = 0;  i < RSD_EFFECTCHNLGRP_MAX;  i++)
	{
		if ( m_pEffectChnlGrp[ i] == NULL)	continue;

		m_pEffectChnlGrp[ i]->release();
		m_pEffectChnlGrp[ i] = NULL;
	}


	// Release channel groups
	for ( int i = 0;  i < RSD_CHANNELGROUP_MAX;  i++)
	{
		if ( m_pChannelGroups[ i] == NULL)	continue;

		m_pChannelGroups[ i]->release();
		m_pChannelGroups[ i] = NULL;
	}


	// Destroy sound system
	FMOD_RESULT result = m_pSystem->close();
	if ( _ERROR( result))
	{
		_ERRORMSG( result);
		return false;
	}

	result = m_pSystem->release();
	if ( _ERROR( result))
	{
		_ERRORMSG( result);
		return false;
	}


	// Clear variables
	m_pSystem = NULL;
	m_bCreate = false;

	return true;
}


// AddSample
bool RealSound3::AddSample( const char* szSampleName, RSample* pSample)
{
	if ( m_bCreate == false)		return false;


	// Enter critical section
	Lock();


	// Add sample
	map< string, RSample*>::iterator itr = m_vSamples.find( szSampleName);
	if ( itr != m_vSamples.end())
	{
		Unlock();
		return false;
	}

	m_vSamples.insert( map< string, RSample*>::value_type( szSampleName, pSample));


	// Leave critical section
	Unlock();

	return true;
}


// RemoveSample
bool RealSound3::RemoveSample( const char* szSampleName)
{
	if ( m_bCreate == false)		return false;


	// Enter critical section
	Lock();


	// Remove sample
	map< string, RSample*>::iterator itr = m_vSamples.find( szSampleName);
	if ( itr == m_vSamples.end())
	{
		Unlock();
		return false;
	}

	delete (*itr).second;
	m_vSamples.erase( itr);


	// Reset scan iterator
	m_itrScanSample = m_vSamples.begin();


	// Leave critical section
	Unlock();

	return true;
}


// GetSample
RSample* RealSound3::GetSample( const char* szSampleName)
{
	if ( m_bCreate == false)		return NULL;


	// Enter critical section
	Lock();


	map< string, RSample*>::iterator itr = m_vSamples.find( szSampleName);
	if ( itr == m_vSamples.end())
	{
		Unlock();
		return NULL;
	}


	// Leave critical section
	Unlock();

	return (*itr).second;
}


// IsExistSample
bool RealSound3::IsExistSample( const char* szSampleName)
{
	if ( m_bCreate == false)		return false;


	// Enter critical section
	Lock();


	map< string, RSample*>::iterator itr = m_vSamples.find( szSampleName);
	if ( itr == m_vSamples.end())
	{
		Unlock();
		return false;
	}


	// Leave critical section
	Unlock();

	return true;
}


// ClearSamples
void RealSound3::RemoveAllSamples()
{
	if ( m_bCreate == false)		return;


	// Enter critical section
	Lock();


	// Reset current BGM
	m_pCurrentBGM = NULL;


	// Remove all sounds
	if ( m_vSounds.empty() == false)
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  itr++)
		{
			RSound* pSound = (*itr).second;
			if ( pSound->IsDelete() == false)		pSound->_destroy();

			delete pSound;
		}

		m_vSounds.clear();


		// Reset scan iterator
		m_itrScanSound = m_vSounds.begin();
	}


	// Remove all samples
	if ( m_ShutdownInfo.bShutdown == true)
	{
		for ( map< string, RSample*>::iterator itr = m_vSamples.begin();  itr != m_vSamples.end();  itr++)
		{
			RSample* pSample = (*itr).second;
			pSample->_release_sound();
		}
	}

	else if ( m_vSamples.empty() == false)
	{
		for ( map< string, RSample*>::iterator itr = m_vSamples.begin();  itr != m_vSamples.end();  itr++)
		{
			RSample* pSample = (*itr).second;
			pSample->_release_sound();

			delete pSample;
		}

		m_vSamples.clear();


		// Reset scan iterator
		m_itrScanSample = m_vSamples.begin();
	}


	// Leave critical section
	Unlock();
}


// Play
RSound* RealSound3::Play( const char* szSampleName, const char* szID, DWORD dwData)
{
	RSound* pSound = NULL;

	if ( m_bCreate == false  ||  szSampleName == NULL  ||  szSampleName[ 0] == 0)
		return NULL;


#ifdef _DEBUG
	// Debug info
//	_MESSAGE( "Play sound : %s", szSampleName);
#endif


	// Enter critical section
	Lock();


	// Get sample
	map< string, RSample*>::iterator itr = m_vSamples.find( szSampleName);
	if ( itr == m_vSamples.end())		goto rsound_error_play;
	RSample* pSample = (*itr).second;


	// Check maximum reference
	if ( pSample->GetReference() >= MAX_REFERENCE)
		goto rsound_error_play;


	// Check unique sound
	if ( pSample->GetUnique() == true  &&  pSample->GetReference() > 0)
		goto rsound_error_play;


	// Get type
	int nType = pSample->GetType();
	if ( nType == RSD_SOUNDTYPE_NONE)	goto rsound_error_play;


	// Create a sound
	if ( pSample->GetSound() == NULL)
	{
		// Get mode
		FMOD_MODE _mode = FMOD_DEFAULT;
		switch ( nType)
		{
		case RSD_SOUNDTYPE_GENERIC :
			_mode |= FMOD_2D | FMOD_CREATESAMPLE;
			_mode |= ( m_bHardwareAccelerated == true)		?  FMOD_HARDWARE : FMOD_SOFTWARE;
			_mode |= ( pSample->GetLoop() == true)			?  FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
			_mode |= ( pSample->GetUnique() == true)		?  FMOD_UNIQUE : FMOD_DEFAULT;
			break;

		case RSD_SOUNDTYPE_BGM :
			_mode |= FMOD_2D | FMOD_CREATESTREAM;
			_mode |= ( m_bHardwareAccelerated == true)		?  FMOD_HARDWARE : FMOD_SOFTWARE;
			_mode |= ( pSample->GetLoop() == true)			?  FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
			break;

		case RSD_SOUNDTYPE_EFFECT :
			_mode |= FMOD_3D | FMOD_SOFTWARE | FMOD_CREATESAMPLE | FMOD_3D_IGNOREGEOMETRY | FMOD_VIRTUAL_PLAYFROMSTART;
			_mode |= ( m_nRollOffModel == RSD_ROLLOFF_LOG)  ?  FMOD_3D_LOGROLLOFF : FMOD_3D_LINEARROLLOFF;
			_mode |= ( pSample->GetLoop() == true)			?  FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
			_mode |= ( pSample->GetUnique() == true)		?  FMOD_UNIQUE : FMOD_DEFAULT;
			break;

		default:
			goto rsound_error_play;
		}


		// Create a sound
		if ( pSample->_create_sound( m_pSystem, _mode) == false)
			goto rsound_error_play;
	}


	// Create a sound object
	pSound = overrideNewSound( pSample, dwData);
	if ( pSound == NULL  ||  pSound->_create( m_pSystem, pSample) == false)
		goto rsound_error_play;


	// Add to channel group
	FMOD::Channel* _channel = pSound->GetChannel();
	if ( _channel == NULL)				goto rsound_error_play;

	if ( nType == RSD_SOUNDTYPE_EFFECT)
	{
		int _chnelgrop = ( dwData & PLAY_EMPHASIZE)  ?  RSD_EFFECTCHNLGRP_EMPHASIZE  :  RSD_EFFECTCHNLGRP_GENERIC;
		_channel->setChannelGroup( m_pEffectChnlGrp[ _chnelgrop]);


		// Set important
		if ( dwData & PLAY_IMPORTANT)
			_channel->setPriority( 0);
	}
	else
		_channel->setChannelGroup( m_pChannelGroups[ nType]);


	// Set callback
	_channel->setCallback( RealSound3::_callback_channel);


	// Add sound to list
	m_vSounds.push_back( pair< string, RSound*>( (szID == NULL) ? "__no_id__" : szID, pSound));


	// Set BGM
	if ( nType == RSD_SOUNDTYPE_BGM)
	{
		if ( m_pCurrentBGM != NULL)
			m_pCurrentBGM->Stop();

		m_pCurrentBGM = pSound;
	}


	// Now playing
	pSound->_update( this);
	pSound->Play();


	// Leave critical section
	Unlock();


	return pSound;



rsound_error_play:

	// Delete sound
	if ( pSound != NULL)
		delete pSound;


	// Leave critical section
	Unlock();


	return pSound;
}


// Stop
bool RealSound3::Stop( const char* szID, bool bMute)
{
	if ( m_bCreate == false  ||  szID == NULL  ||  szID[ 0] == 0)
		return false;


	// Enter critical section
	Lock();


	// Stop sound
	string strID = szID;
	if ( bMute == false)
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  itr++)
		{
			if ( (*itr).first != strID)		continue;

			RSound* pSound = (*itr).second;
			if ( pSound != NULL)
				pSound->Stop();
		}
	}
	else
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  )
		{
			if ( (*itr).first != strID)
				itr++;

			else
			{
				RSound* pSound = (*itr).second;
				if ( pSound != NULL)
				{
					if ( pSound->IsDelete() == false)
						pSound->_destroy();

					if ( pSound == m_pCurrentBGM)
						m_pCurrentBGM = NULL;
				}
			}
		}
	}


	// Leave critical section
	Unlock();

	return true;
}


// StopAll
void RealSound3::StopAll( bool bMute)
{
	if ( m_bCreate == false)		return;


	// Enter critical section
	Lock();


	// Reset current BGM
	m_pCurrentBGM = NULL;


	// Stop all sound
	if ( bMute == false)
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  itr++)
			(*itr).second->Stop();
	}
	else
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  itr++)
		{
			RSound* pSound = (*itr).second;
			if ( pSound->IsDelete() == false)		pSound->_destroy();

			delete pSound;
		}

		m_vSounds.clear();


		// Reset scan iterator
		m_itrScanSound = m_vSounds.begin();
	}


	// Leave critical section
	Unlock();
}


// ReleaseAllSound
void RealSound3::ReleaseAllSound()
{
	if ( m_bCreate == false)		return;


	// Enter critical section
	Lock();


	// Reset current BGM
	m_pCurrentBGM = NULL;


	// Remove all sounds
	if ( m_vSounds.empty() == false)
	{
		for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();  itr++)
		{
			RSound* pSound = (*itr).second;
			if ( pSound->IsDelete() == false)		pSound->_destroy();

			delete pSound;
		}

		m_vSounds.clear();


		// Reset scan iterator
		m_itrScanSound = m_vSounds.begin();
	}


	// Release all sounds
	if ( m_vSamples.empty() == false)
	{
		for ( map< string, RSample*>::iterator itr = m_vSamples.begin();  itr != m_vSamples.end();  itr++)
		{
			RSample* pSample = (*itr).second;
			pSample->_release_sound();	
		}
	}


	// Leave critical section
	Unlock();
}


// SetListener
void RealSound3::SetListener( const float* _pos, const float* _vel, const float* _forward, const float* _up)
{
	if ( m_bCreate == false)		return;

	FMOD_RESULT result = m_pSystem->set3DListenerAttributes( 0, (FMOD_VECTOR*)_pos, (FMOD_VECTOR*)_vel, (FMOD_VECTOR*)_forward, (FMOD_VECTOR*)_up);
	if ( _ERROR( result))			_ERRORMSG( result);
}


// Update
void RealSound3::Update()
{
	if ( m_bCreate == false)		return;


	// Enter critical section
	Lock();


	// Process sounds
	for ( list< pair< string, RSound*> >::iterator itr = m_vSounds.begin();  itr != m_vSounds.end();   itr++)
	{
		RSound* pSound = (*itr).second;
		pSound->_update( this);
	}


	// Call event handler
	OnUpdate();


	// Update system
	m_pSystem->update();


	// Leave critical section
	Unlock();
}


// _system_shutdown
void RealSound3::_system_shutdown()
{
	// Backup system info
	m_ShutdownInfo.bShutdown = true;
	m_ShutdownInfo.bReverb = m_Reverb.bEnable;
	m_ShutdownInfo.bCompressor = m_Compressor.bEnable;


	// Destroy system
	Destroy();
}


// _system_restore
void RealSound3::_system_restore()
{
	// System restart
	Create( m_SystemInfo.nMaxChannels, m_SystemInfo.nSoftwareChannels, m_SystemInfo.nSpeakerMode, m_SystemInfo.nSampleRate);
	EnableReverb( m_ShutdownInfo.bReverb);
	EnableCompressor( m_ShutdownInfo.bCompressor);


	m_ShutdownInfo.bShutdown = false;
}


// GetSystem
FMOD::System* RealSound3::GetSystem() const
{
	return m_pSystem;
}


// GetSampleList
const map< string, RSample*>* RealSound3::GetSampleList()
{
	return &m_vSamples;
}


// GetSoundList
const list< pair< string, RSound*> >* RealSound3::GetSoundList()
{
	return &m_vSounds;
}


// GetChannelGroup
FMOD::ChannelGroup* RealSound3::GetChannelGroup( int nGroup) const
{
	return m_pChannelGroups[ nGroup];
}


// GetSoundsPlaying
size_t RealSound3::GetSoundsPlaying() const
{
	return m_vSounds.size();
}


// SetVolume
void RealSound3::SetVolume( int nChannelGroup, float fVolume)
{
	if ( m_bCreate == false  ||  m_fVolume[ nChannelGroup] == fVolume)	return;
	m_fVolume[ nChannelGroup] = fVolume;

	if ( m_pChannelGroups[ nChannelGroup] != NULL  &&  m_bSilence == false)
		m_pChannelGroups[ nChannelGroup]->setVolume( fVolume);
}


// GetVolume
float RealSound3::GetVolume( int nChannelGroup) const
{
	return m_fVolume[ nChannelGroup];
}


// SetSilence
void RealSound3::SetSilence( bool bSilence)
{
	if ( m_bCreate == false  ||  m_bSilence == bSilence)
		return;

	m_bSilence = bSilence;

	if ( bSilence == true)			m_MasterChnlGrpVolFader.FadeOut( 0.0f);
	else							m_MasterChnlGrpVolFader.FadeIn( m_fVolume[ RSD_CHANNELGROUP_MASTER]);
}


// GetSilence
bool RealSound3::GetSilence() const
{
	return m_bSilence;
}


// SetMute
void RealSound3::SetMute( int nChannelGroup, bool bMute)
{
	if ( m_bCreate == false  ||  m_bMute[ nChannelGroup] == bMute)		return;
	m_bMute[ nChannelGroup] = bMute;

	if ( m_pChannelGroups[ nChannelGroup] != NULL)
		m_pChannelGroups[ nChannelGroup]->setMute( bMute);
}


// GetMute
bool RealSound3::GetMute( int nChannelGroup) const
{
	return m_bMute[ nChannelGroup];
}


// SetHardwareAccelerate
void RealSound3::SetHardwareAccelerate( bool bAccelerate)
{
	if ( m_bCreate == false  ||  m_bHardwareAccelerated == bAccelerate)
		return;

	ReleaseAllSound();

	m_bHardwareAccelerated = bAccelerate;
}


// IsHardwareAccelerate
bool RealSound3::IsHardwareAccelerate() const
{
	return m_bHardwareAccelerated;
}


// SetPanningDistance
void RealSound3::SetPanningDistance( float fDistance)
{
	m_fPanningDistance = fDistance;
}


// GetPanningDistance
float RealSound3::GetPanningDistance() const
{
	return m_fPanningDistance;
}


// SetRollOffModel
void RealSound3::SetRollOffModel( int nModel)
{
	if ( m_bCreate == false  ||  m_nRollOffModel == nModel)
		return;

	ReleaseAllSound();

	m_nRollOffModel = nModel;
}


// GetRollOffModel
int RealSound3::GetRollOffModel() const
{
	return m_nRollOffModel;
}


// EnableReverb
void RealSound3::EnableReverb( bool bEnable)
{
	if ( m_bCreate == false  ||  m_Reverb.bEnable == bEnable)
		return;

	m_Reverb.bEnable = bEnable;

	if ( bEnable == true)
		m_pSystem->setReverbAmbientProperties( &REVERB_PRESET[ m_Reverb.nAmbient]);
	else
		m_pSystem->setReverbAmbientProperties( &REVERB_PRESET[ RSD_REVERB_OFF]);
}


// IsEnabledReverb
bool RealSound3::IsEnabledReverb() const
{
	return m_Reverb.bEnable;
}


// SetReverb
void RealSound3::SetReverb( int nReverb)
{
	if ( m_bCreate == false)		return;

	m_Reverb.nAmbient = nReverb;

	if ( m_Reverb.bEnable == true)
		m_pSystem->setReverbAmbientProperties( &REVERB_PRESET[ nReverb]);
}


// GetReverb
int RealSound3::GetReverb() const
{
	return m_Reverb.nAmbient;
}


// AddLocalReverb
void RealSound3::AddLocalReverb( int nReverb, const float* pPos, float fMinDist, float fMaxDist)
{
	if ( m_bCreate == false)		return;

	FMOD::Reverb* pReverb;
	FMOD_RESULT result = m_pSystem->createReverb( &pReverb);
	if ( _ERROR( result))			return;

	m_Reverb.vLocal.push_back( pReverb);

	FMOD_REVERB_PROPERTIES prop = REVERB_PRESET[ nReverb];
	pReverb->setProperties( &prop);
	pReverb->set3DAttributes( (FMOD_VECTOR*)pPos, fMinDist, fMaxDist);
}


// ClearLocalReverb
void RealSound3::ClearLocalReverb()
{
	if ( m_bCreate == false  ||  m_Reverb.vLocal.empty() == true)
		return;

	for ( vector< FMOD::Reverb*>::iterator itr = m_Reverb.vLocal.begin();  itr != m_Reverb.vLocal.end();  itr++)
		(*itr)->release();

	m_Reverb.vLocal.clear();
}


// EnableCompressor
void RealSound3::EnableCompressor( bool bEnable)
{
	if ( m_bCreate == false  ||  m_Compressor.bEnable == bEnable)
		return;

	if ( bEnable == true)
	{
		FMOD_RESULT result = m_pSystem->createDSPByType( FMOD_DSP_TYPE_COMPRESSOR, &m_Compressor.pDSP);
		if ( _ERROR( result) == false)
		{
			result = m_pChannelGroups[ RSD_CHANNELGROUP_EFFECT]->addDSP( m_Compressor.pDSP, 0);
			if ( _ERROR( result) == false)
			{
				SetCompressor( m_Compressor.fThreshold, m_Compressor.fAttack, m_Compressor.fRelease, m_Compressor.fGainMakeUp);

				m_Compressor.bEnable = true;
			}
		}
		else
			_ERRORMSG( result);
	}
	else
	{
		m_Compressor.pDSP->remove();
		m_Compressor.pDSP->release();
		m_Compressor.pDSP = NULL;

		m_Compressor.bEnable = false;
	}
}


// IsEnabledCompressor
bool RealSound3::IsEnabledCompressor() const
{
	return m_Compressor.bEnable;
}


// SetCompressor
void RealSound3::SetCompressor( float fThreshold, float fAttack, float fRelease, float fGainMakeup)
{
	if ( m_bCreate == false)		return;

	m_Compressor.fThreshold = fThreshold;
	m_Compressor.fAttack = fAttack;
	m_Compressor.fRelease = fRelease;
	m_Compressor.fGainMakeUp = fGainMakeup;

	if ( m_Compressor.pDSP != NULL)
	{
		m_Compressor.pDSP->setParameter( FMOD_DSP_COMPRESSOR_THRESHOLD, fThreshold);
		m_Compressor.pDSP->setParameter( FMOD_DSP_COMPRESSOR_ATTACK, fAttack);
		m_Compressor.pDSP->setParameter( FMOD_DSP_COMPRESSOR_RELEASE, fRelease);
		m_Compressor.pDSP->setParameter( FMOD_DSP_COMPRESSOR_GAINMAKEUP, fGainMakeup);
	}
}


// GetCompressor
void RealSound3::GetCompressor( float* pThreshold, float* pAttack, float* pRelease, float* pGainMakeUp)
{
	if ( pThreshold != NULL)	*pThreshold = m_Compressor.fThreshold;
	if ( pAttack != NULL)		*pAttack = m_Compressor.fAttack;
	if ( pRelease != NULL)		*pRelease = m_Compressor.fRelease;
	if ( pGainMakeUp != NULL)	*pGainMakeUp = m_Compressor.fGainMakeUp;
}


// SetPitch
void RealSound3::SetPitch( float fPitch)
{
	if ( m_bCreate == false)		return;

	if ( m_pChannelGroups[ RSD_CHANNELGROUP_GENERIC] != NULL)
		m_pChannelGroups[ RSD_CHANNELGROUP_GENERIC]->setPitch( fPitch);

	if ( m_pChannelGroups[ RSD_CHANNELGROUP_EFFECT] != NULL)
		m_pChannelGroups[ RSD_CHANNELGROUP_EFFECT]->setPitch( fPitch);
}


// GetPitch
float RealSound3::GetPitch() const
{
	float fPitch = 1.0f;
	if ( m_pChannelGroups[ RSD_CHANNELGROUP_GENERIC] != NULL)
		m_pChannelGroups[ RSD_CHANNELGROUP_GENERIC]->getPitch( &fPitch);

	return fPitch;
}


// SetSpeakerMode
void RealSound3::SetSpeakerMode( int nSpeakerMode)
{
	if ( m_bCreate == false  ||  m_SystemInfo.nSpeakerMode == nSpeakerMode)
		return;


	// Shutdown system
	_system_shutdown();


	// Set speaker mode
	m_SystemInfo.nSpeakerMode = nSpeakerMode;


	// Restore system
	_system_restore();
}


// GetSpeakerMode
int RealSound3::GetSpeakerMode() const
{
	return m_SystemInfo.nSpeakerMode;
}


// SetSpeakerPosition
void RealSound3::SetSpeakerPosition( int nSpeaker, float x, float y)
{
	if ( m_bCreate == false)		return;

	m_pSystem->set3DSpeakerPosition( (FMOD_SPEAKER)nSpeaker, x, y, true);
}


// GetSpeakerPosition
void RealSound3::GetSpeakerPosition( int nSpeaker, float* x, float *y)
{
	if ( m_bCreate == false)		return;

	m_pSystem->get3DSpeakerPosition( (FMOD_SPEAKER)nSpeaker, x, y, NULL);
}


// GetCPUUsage
void RealSound3::GetCPUUsage( float *pTotal, float *pDSP, float *pStream, float *pGeometry, float *pUpdate)
{
	if ( m_bCreate == false)		return;

	m_pSystem->getCPUUsage( pDSP, pStream, pGeometry, pUpdate, pTotal);
}


// _thread
unsigned long WINAPI RealSound3::_thread( LPVOID lpParameter)
{
	RealSound3* pRealSound = (RealSound3*)lpParameter;

	// Call event handler
	pRealSound->_on_create_thread();


	// Run thread
	bool bSampleUpdate = false;
	while ( pRealSound->_is_exit_thread() == false)
	{
		// Update timer
		UpdateTimer();


		// Update samples
		if ( bSampleUpdate == true)
		{
			bSampleUpdate = false;

			pRealSound->_on_update_samples();
		}

		// Update volumes
		else
		{
			bSampleUpdate = true;

			pRealSound->_on_update_volumes();
		}


		// Sleep to relax CPU usage
		Sleep( 1);
	}


	// Call event handler
	pRealSound->_on_destroy_thread();

	return 0;
}


// _is_exit_thread
bool RealSound3::_is_exit_thread() const
{
	return m_Thread.bExit;
}


// _on_create_thread
void RealSound3::_on_create_thread()
{
	m_Thread.hEventDestroy = CreateEvent( NULL, FALSE, FALSE, NULL);
}


// _on_destroy_thread
void RealSound3::_on_destroy_thread()
{
	SetEvent( m_Thread.hEventDestroy);
}


// _on_update_samples
void RealSound3::_on_update_samples()
{
	if ( m_bCreate == false)		return;


	// Enter critical section
	Lock();


	// Update samples
	if ( m_vSamples.empty() == false)
	{
		if ( m_itrScanSample == m_vSamples.end())
			m_itrScanSample = m_vSamples.begin();

		if ( m_itrScanSample != m_vSamples.end())
		{
			RSample* pSample = (*m_itrScanSample).second;
			if ( pSample != NULL)
			{
				pSample->_update_sample();

				m_itrScanSample++;
			}
		}
	}


	// Leave critical section
	Unlock();
}


// _on_update_volumes
void RealSound3::_on_update_volumes()
{
	if ( m_bCreate == false)		return;


	// Enter critical section
	Lock();


	// Emphasized effect sound
	int nChannels = 0;
	m_pEffectChnlGrp[ RSD_EFFECTCHNLGRP_EMPHASIZE]->getNumChannels( &nChannels);
	if ( nChannels > 0)		m_EffectChnlGrpVolFader.FadeOut( 0.4f);
	else					m_EffectChnlGrpVolFader.FadeIn( 1.0f);


	// Update master channel group volume fader
	m_MasterChnlGrpVolFader.Update();

	// Update effect channel group volume fader
	m_EffectChnlGrpVolFader.Update();


	// Update sounds
	if ( m_vSounds.empty() == false)
	{
		if ( m_itrScanSound == m_vSounds.end())
			m_itrScanSound = m_vSounds.begin();

		if ( m_itrScanSound != m_vSounds.end())
		{
			RSound* pSound = (*m_itrScanSound).second;
			if ( pSound != NULL)
			{
				if ( pSound->IsDelete() == true)
				{
					// Clear BGM
					if ( pSound == m_pCurrentBGM)
						m_pCurrentBGM = NULL;


					// Delete sound
					delete pSound;


					// Set next scan iterator
					m_itrScanSound = m_vSounds.erase( m_itrScanSound);
				}
				else
				{
					// Update sound volume
					pSound->_update_volume();

					// Set next scan iterator
					m_itrScanSound++;
				}
			}
		}
	}
		

	// Leave critical section
	Unlock();
}


// Lock
void RealSound3::Lock()
{
	if ( m_bCreate == false)		return;


	// Enter critical section
	EnterCriticalSection( &m_Thread.csLock);
}


// Unlock
void RealSound3::Unlock()
{
	if ( m_bCreate == false)		return;


	// Leave critical section
	LeaveCriticalSection( &m_Thread.csLock);
}


// CallbackChannel
FMOD_RESULT	F_CALLBACK RealSound3::_callback_channel( FMOD_CHANNEL* pChannel, FMOD_CHANNEL_CALLBACKTYPE nType, void* pCommandData1, void* pCommandData2)
{
	if ( nType == FMOD_CHANNEL_CALLBACKTYPE_END)
	{
		FMOD::Channel* _channel = (FMOD::Channel*)pChannel;
		if ( _channel != NULL)
		{
			void* userdata = NULL;
			_channel->getUserData( &userdata);

			if ( userdata != NULL)
			{
				// Destroy sound
				RSound* pSound = (RSound*)userdata;
				if ( pSound != NULL)
				{
					if ( pSound->IsDelete() == false)
						pSound->_destroy();
				}
			}
		}
	}

	return FMOD_OK;
}


// CallbackRollOff
float F_CALLBACK RealSound3::_callback_rolloff( FMOD_CHANNEL *pChannel, float fDistance)
{
	FMOD::Channel* _channel = (FMOD::Channel*)pChannel;
	if ( _channel == NULL)			return 0.0f;

	void* userdata = NULL;
	_channel->getUserData( &userdata);
	if ( userdata == NULL)			return 0.0f;

	RSound* pSound = (RSound*)userdata;
	if ( pSound == NULL)			return 0.0f;

	return pSound->_update_rolloff( fDistance);
}


// OnCreate
void RealSound3::OnCreate()
{
	// Set the distance units(meters)
	m_pSystem->set3DSettings( 100.0f, 100.0f, 1.0f);
}


// OnDestroy
void RealSound3::OnDestroy()
{
}


// OnPreUpdate
void RealSound3::OnPreUpdate()
{
}


// OnUpdate
void RealSound3::OnUpdate()
{
}


#include "stdafx.h"
#include "RealSound.h"
#include "RSoundError.h"
#include <math.h>


// Maximum sound reference count
#define MAX_REFERENCE				10

// Name of the channel groups
static const char* STR_CHANNELGROUP[] = { "_MASTER_", "_2D_", "_3D_", "_BGM_" };



RealSound::RealSound()
{
	m_Timer.Update();

	m_pSystem = NULL;
	m_bHardwareAccelerate = true;
	m_bDoppler = false;
	m_bSilence = false;
	m_pCompressor = NULL;
	m_tLastUpdateReleaseSound = m_Timer.GetCurTime();

	m_Reverb.pPresets = NULL;
	m_Reverb.nAmbient = 0;

	m_HRTF.bEnable = true;
	m_HRTF.fMinAngle = 1.3f;
	m_HRTF.fMaxAngle = 2.3f;
	m_HRTF.fScale = 0.9f;

	m_Thread.hThread = NULL;
	m_Thread.hEventDestroy = NULL;
	m_Thread.bExit = true;
}


RealSound::~RealSound()
{
	Destroy();
}


bool RealSound::Create( const SoundSystemCreateInfo* info)
{
	if ( m_pSystem != NULL)
		return false;


	_MESSAGE( "Open RealSound engine.");


	// Create sound system
	FMOD_RESULT result = FMOD::System_Create( &m_pSystem);
	if ( _ERROR( result))
				goto rsound_error_create;


	// Check version
	unsigned int _version;
	result = m_pSystem->getVersion( &_version);
	if ( _ERROR( result))
		goto rsound_error_create;
	
	if ( _version < FMOD_VERSION)
	{
		_MESSAGE( "- [ERROR] You are using an old version of FMOD %08x. This program requires %08x\n", _version, FMOD_VERSION);

		Destroy();
		return false;
	}


	// Check driver counts
	int _drivers;
	result = m_pSystem->getNumDrivers( &_drivers);
	if ( _ERROR( result))
		goto rsound_error_create;

	if ( _drivers > 0)
	{
		result = m_pSystem->setOutput( FMOD_OUTPUTTYPE_DSOUND);
		if ( _ERROR( result))
			goto rsound_error_create;

		FMOD_CAPS _caps;
		FMOD_SPEAKERMODE _speaker;
		result = m_pSystem->getDriverCaps( 0, &_caps, 0, 0, &_speaker); 
		if ( _ERROR( result))
			goto rsound_error_create;

		_MESSAGE( "Driver capabilities : 0x%08X", _caps);


		// Set speaker mode
		switch ( info ? info->SpeakerMode : RSD_SPEAKERMODE_STEREO)
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
		if ( _ERROR( result))
			goto rsound_error_create;


		if ( _caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			_MESSAGE( "Bad latency.");

			result = m_pSystem->setDSPBufferSize( 1024, 10);
			if ( _ERROR( result))
				goto rsound_error_create;
		}

		if ( _caps & FMOD_CAPS_REVERB_LIMITED)
			_MESSAGE( "Device supports some form of limited hardware reverb.");


		char _name[ 256];
		result = m_pSystem->getDriverInfo( 0, _name, 256, 0);
		if ( _ERROR( result))
			goto rsound_error_create;
		else if ( strstr( _name, "SigmaTel"))			// Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it.
		{
			result = m_pSystem->setSoftwareFormat( info ? info->SampleRate : 44100, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			if ( _ERROR( result))
				goto rsound_error_create;
		}
		else
		{
			// Sampling rate = 48000 -> 44100
			result = m_pSystem->setSoftwareFormat( info ? info->SampleRate : 44100, FMOD_SOUND_FORMAT_PCM16, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			if ( _ERROR( result))
				goto rsound_error_create;
		}
	}
	else
	{
		result = m_pSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND);
		if ( _ERROR( result))
			goto rsound_error_create;
	}


	// Set software(virtual) channels
	result = m_pSystem->setSoftwareChannels( info ? info->VirtualChannels : 20);
	if ( _ERROR( result))
		goto rsound_error_create;


	// Initialize sound system
	result = m_pSystem->init( info ? info->MaxChannels : 100, FMOD_INIT_3D_RIGHTHANDED, 0);
	if ( result == FMOD_ERR_OUTPUT_CREATEBUFFER)		// Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo...
	{
		result = m_pSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO);
		if ( _ERROR( result))
			goto rsound_error_create;

		result = m_pSystem->init( info ? info->MaxChannels : 100, FMOD_INIT_3D_RIGHTHANDED, 0);
		if ( _ERROR( result))
			goto rsound_error_create;
	}
	else if ( _ERROR( result))
		goto rsound_error_create;


	// Set the distance units(meters)
	result = m_pSystem->set3DSettings( info ? info->DopplerScale : 2.0f, info ? info->DistanceFactor : 100.0f, info ? info->RolloffScale : 1.0f);
	if ( _ERROR( result))
		goto rsound_error_create;

	// Set roll-off callback
	result = m_pSystem->set3DRolloffCallback( RealSound::_internal_callback_rolloff);
	if ( _ERROR( result))
		goto rsound_error_create;


	// Create channel groups
	for ( int i = 0;  i < RSD_CHANNELGROUP_MAX;  i++)
	{
		if ( i == RSD_CHANNELGROUP_MASTER)
		{
			result = m_pSystem->getMasterChannelGroup( &m_pChannelGroups[ RSD_CHANNELGROUP_MASTER]);
			if ( _ERROR( result))
				goto rsound_error_create;
		}
		else
		{
			result = m_pSystem->createChannelGroup( STR_CHANNELGROUP[ i], &m_pChannelGroups[ i]);
			if ( _ERROR( result))
				goto rsound_error_create;

			result = m_pChannelGroups[ RSD_CHANNELGROUP_MASTER]->addGroup( m_pChannelGroups[ i]);
			if ( _ERROR( result))
				goto rsound_error_create;
		}

		m_pChannelGroups[ i]->getVolume( &m_fChannelGroupsVolume[ i]);
	}


	// Add compressor
	if ( m_pSystem->createDSPByType( FMOD_DSP_TYPE_COMPRESSOR, &m_pCompressor) == FMOD_OK)
	{
		m_pCompressor->setBypass( true);
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_THRESHOLD, -4.0f);
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_ATTACK, 10.0f);
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_RELEASE, 300.0f);
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_GAINMAKEUP, 0.0f);
		m_pChannelGroups[ RSD_CHANNELGROUP_3D]->addDSP( m_pCompressor, NULL);
	}


	// Create reverb presets
	m_Reverb.pPresets = new FMOD_REVERB_PROPERTIES[ RSD_REVERB_MAX];
	FMOD_REVERB_PROPERTIES _reverbs[] =
	{
		FMOD_PRESET_OFF,			FMOD_PRESET_GENERIC,		FMOD_PRESET_PADDEDCELL,			FMOD_PRESET_ROOM,			FMOD_PRESET_BATHROOM,
		FMOD_PRESET_LIVINGROOM,		FMOD_PRESET_STONEROOM,		FMOD_PRESET_AUDITORIUM,			FMOD_PRESET_CONCERTHALL,	FMOD_PRESET_CAVE,
		FMOD_PRESET_ARENA,			FMOD_PRESET_HANGAR,			FMOD_PRESET_CARPETTEDHALLWAY,	FMOD_PRESET_HALLWAY,		FMOD_PRESET_STONECORRIDOR,
		FMOD_PRESET_ALLEY,			FMOD_PRESET_FOREST,			FMOD_PRESET_CITY,				FMOD_PRESET_MOUNTAINS,		FMOD_PRESET_QUARRY,
		FMOD_PRESET_PLAIN,			FMOD_PRESET_PARKINGLOT,		FMOD_PRESET_SEWERPIPE,			FMOD_PRESET_UNDERWATER
	};
	memcpy( m_Reverb.pPresets, _reverbs, sizeof( FMOD_REVERB_PROPERTIES) * RSD_REVERB_MAX);
	m_Reverb.nAmbient = 0;
	m_Reverb.vLocal.clear();


	// Create thread
	InitializeCriticalSection( &m_Thread.csLock);
	m_Thread.bExit = false;
	m_Thread.hThread = CreateThread( NULL, 0, _internal_thread, this, 0, NULL);


	// Output system info
	int _SoftwareChannels = 0;
	m_pSystem->getSoftwareChannels( &_SoftwareChannels);
	int _HardwareChannels = 0;
	int _Hardware2DChannels = 0;
	int _Hardware3DChannels = 0;
	m_pSystem->getHardwareChannels( &_Hardware2DChannels, &_Hardware3DChannels, &_HardwareChannels);
	_MESSAGE( "Software channels : %d channels,  Hardware channels : %d(2D) + %d(3D) = %d channels", _SoftwareChannels, _Hardware2DChannels, _Hardware3DChannels, _HardwareChannels);


	// Set variant
	m_pSystem->setUserData( static_cast< void*>( this));
	m_bHardwareAccelerate = true;
	m_bDoppler = false;
	m_bSilence = false;
	m_HRTF.bEnable = true;
	m_HRTF.fMinAngle = 1.3f;
	m_HRTF.fMaxAngle = 2.3f;
	m_HRTF.fScale = 0.9f;


	// Call event handler
	OnCreatedSystem( m_pSystem);

	return true;


rsound_error_create:

	_ERRORMSG( result);

	Destroy();

	return false;
}


bool RealSound::Destroy()
{
	if ( m_pSystem == NULL)
		return false;


	_MESSAGE( "Close RealSound engine.");


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
	DeleteCriticalSection( &m_Thread.csLock);


	// Remove all samples
	RemoveAllSamples();


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
	
	m_pSystem = NULL;


	// Clear channel groups
	for ( int i = 0;  i < RSD_CHANNELGROUP_MAX;  i++)
		m_pChannelGroups[ i] = NULL;


	// Clear reverb presets
	if ( m_Reverb.pPresets != NULL)
		delete [] m_Reverb.pPresets;


	// Call event handler
	OnDestroyedSystem();


	return true;
}


bool RealSound::AddSample( const char* name, const char* filename, int type, float volume, bool loop, bool unique, int priority, int min_dist, int max_dist, bool ignorefod)
{
	if ( m_pSystem == NULL  ||  name == NULL  ||  filename == NULL  ||  type <= RSD_SOUNDTYPE_NONE  ||  type >= RSD_SOUNDTYPE_MAX)
		return false;

	map< string, Sample*>::iterator itr = m_vSamples.find( name);
	if ( itr != m_vSamples.end())
		return false;

	Sample* pSample = new Sample();
	pSample->pSound = NULL;
	pSample->strName = name;
	pSample->strFileName = filename;
	pSample->nType = type;
	pSample->nVolume = (unsigned char)max( 0, min( 100, (int)( volume * 100.0f)));
	pSample->bLoop = loop;
	pSample->bUnique = unique;
	pSample->nPriority = max( 0, min( 255, priority));
	pSample->nMinDist = max( 0, min_dist);
	pSample->nMaxDist = max( 0, max_dist);
	pSample->bIgnoreFOD = ignorefod;
	pSample->nReferenceCount = 0;
	pSample->tLastAccessTime = 0.0;
	pSample->pData = NULL;
	pSample->nDataSize = 0;
	m_vSamples.insert( map< string, Sample*>::value_type( name, pSample));

	return true;
}


bool RealSound::RemoveSample( const char* szSampleName)
{
	if ( m_pSystem == NULL  ||  szSampleName == NULL)
		return false;

	string strName( szSampleName);
	map< string, Sample*>::iterator itr = m_vSamples.find( strName);
	if ( itr == m_vSamples.end())
		return false;

	Sample* pSample = (*itr).second;
	for ( int group = (RSD_CHANNELGROUP_MASTER + 1);  group < RSD_CHANNELGROUP_MAX;  group++)
	{
		int channels = 0;
		m_pChannelGroups[ group]->getNumChannels( &channels);
		for ( int channel = channels - 1;  channel >= 0;  channel--)
		{
			FMOD::Channel* pChannel = NULL;
			if ( m_pChannelGroups[ group]->getChannel( channel, &pChannel) != FMOD_OK)
				continue;

			FMOD::Sound* pSound = NULL;
			if ( pChannel->getCurrentSound( &pSound) != FMOD_OK)
				continue;

			void* userdata = NULL;
			if ( pSound->getUserData( &userdata) != FMOD_OK)
				continue;

			if ( userdata == pSample)
				pChannel->stop();
		}
	}

	if ( pSample->pSound != NULL)
		pSample->pSound->release();

	if ( pSample->pData != NULL)
		delete pSample->pData;

	delete pSample;

	m_vSamples.erase( itr);

	return true;
}


void RealSound::RemoveAllSamples()
{
	StopAll( true);

	if ( m_vSamples.empty() == false)
	{
		for ( map< string, Sample*>::iterator itr = m_vSamples.begin();  itr != m_vSamples.end();  itr++)
		{
			Sample* pSample = (*itr).second;
			
			if ( pSample->pSound != NULL)
				pSample->pSound->release();

			if ( pSample->pData != NULL)
				delete pSample->pData;

			delete pSample;
		}

		m_vSamples.clear();
	}
}


bool RealSound::IsExistSample( const char* szSampleName)
{
	map< string, Sample*>::iterator itr = m_vSamples.find( string( szSampleName));
	if ( itr != m_vSamples.end())
		return true;

	return false;
}


inline bool _normalize( FMOD_VECTOR* vec)
{
	float scale = sqrt( vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	if ( scale > 0.0001f)
	{
		scale = 1.0f / scale;
		vec->x *= scale;
		vec->y *= scale;
		vec->z *= scale;
		return true;
	}

	return false;
}


inline float _dot_product( const FMOD_VECTOR& vec1, const FMOD_VECTOR& vec2)
{
	return ( vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}


int RealSound::Play( const char* szSampleName, const char* szID, SoundAdapter* pAdapter)
{
	Sample* pSample = NULL;
	FMOD::Channel* pChannel = NULL;


	if ( m_pSystem == NULL)
		return RSD_CHANNELID_NONE;


	// Find sound sample
	map< string, Sample*>::iterator itr = m_vSamples.find( string( szSampleName));
	if ( itr == m_vSamples.end())
	{
		_MESSAGE( "[REALSOUND3] - [ERROR] Cannot find sample (%s)\n", szSampleName);
		goto rsound_error_create;
	}
	pSample = (*itr).second;


	// Check maximum or unique reference
	if ( pSample->nReferenceCount > MAX_REFERENCE  ||  ( pSample->bUnique == true  &&  pSample->nReferenceCount > 0))
		goto rsound_error_create;


	// Create a sound
	FMOD_RESULT result;
	if ( pSample->pSound == NULL)
	{
		// Get mode
		FMOD_MODE mode = FMOD_DEFAULT;
		switch ( pSample->nType)
		{
		case RSD_SOUNDTYPE_2D :
			mode |= FMOD_2D | FMOD_CREATESAMPLE;
 			mode |= ( m_bHardwareAccelerate == true)	?  FMOD_HARDWARE : FMOD_SOFTWARE;
 			mode |= ( pSample->bLoop == true)			?  FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
 			mode |= ( pSample->bUnique == true)			?  FMOD_UNIQUE : FMOD_DEFAULT;
			break;

		case RSD_SOUNDTYPE_3D :
			mode |= FMOD_3D | FMOD_CREATESAMPLE | FMOD_SOFTWARE | FMOD_3D_WORLDRELATIVE | FMOD_3D_IGNOREGEOMETRY | FMOD_VIRTUAL_PLAYFROMSTART;
			mode |= ( pSample->bLoop == true)			?  FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
			mode |= ( pSample->bUnique == true)			?  FMOD_UNIQUE : FMOD_DEFAULT;
			break;

		case RSD_SOUNDTYPE_BGM :
			mode |= FMOD_2D | FMOD_CREATESTREAM;
 			mode |= ( m_bHardwareAccelerate == true)	?  FMOD_HARDWARE : FMOD_SOFTWARE;
			mode |= ( pSample->bLoop == true)			?  FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
			break;
		}


		// Create a sound
		result = OnCreateSound( pSample, mode);
		if ( _ERROR( result)  ||  pSample->pSound == NULL)
		{
			_ERRORMSG( result, szSampleName);
			goto rsound_error_create;
		}

		pSample->pSound->setUserData( static_cast< void*>( pSample));
	}


	// Create sound channel
	result = m_pSystem->playSound( FMOD_CHANNEL_FREE, pSample->pSound, true, &pChannel);
	if ( _ERROR( result)  ||  pChannel == NULL)
	{
		_ERRORMSG( result, pSample->strFileName.c_str());
		goto rsound_error_create;
	}


	// Add to channel group
	switch ( pSample->nType)
	{
	case RSD_SOUNDTYPE_2D :
		pChannel->setChannelGroup( m_pChannelGroups[ RSD_CHANNELGROUP_2D]);
		break;

	case RSD_SOUNDTYPE_3D :
		pChannel->setChannelGroup( m_pChannelGroups[RSD_CHANNELGROUP_3D]);
		break;

	case RSD_SOUNDTYPE_BGM :
		pChannel->setChannelGroup( m_pChannelGroups[ RSD_CHANNELGROUP_BGM]);
		break;
	}


	// Get sound adapter
	if ( pAdapter == NULL)
		pAdapter = new SoundAdapter();
	pAdapter->setID( szID);


	// Set common attribute
	pChannel->setUserData( static_cast< void*>( pAdapter));
	pChannel->setCallback( RealSound::_internal_callback_channel);
	pChannel->setVolume( ( pSample->bLoop  ||  pSample->nType == RSD_SOUNDTYPE_BGM)  ?  0.0f : (float)pSample->nVolume * 0.01f);


	// Set 3D attribute
	if ( pSample->nType == RSD_SOUNDTYPE_3D)
	{
		pChannel->setPriority( (int)pSample->nPriority);
		pChannel->set3DMinMaxDistance( (float)pSample->nMinDist, (float)pSample->nMaxDist);
		pChannel->setDelay( FMOD_DELAYTYPE_END_MS, 1000, 0);

		// Attach low-pass filter
		if ( pSample->bIgnoreFOD == false)
		{
			FMOD::DSP* _head = NULL;
			pChannel->getDSPHead( &_head);

			FMOD::DSP* _lowpass = NULL;
			if ( m_pSystem->createDSPByType( FMOD_DSP_TYPE_LOWPASS_SIMPLE, &_lowpass) == FMOD_OK)
				pChannel->addDSP( _lowpass, NULL);
		}


		// Set sound position and distance
		FMOD_VECTOR _pos_l;
		m_pSystem->get3DListenerAttributes( 0, &_pos_l, NULL, NULL, NULL);

		FMOD_VECTOR _pos_s = { 0.0f, 0.0f, 0.0f };
		pAdapter->getPosition( &_pos_s);
		pChannel->set3DAttributes( &_pos_s, NULL);

		float fDistance = sqrt( pow( _pos_s.x - _pos_l.x, 2) + pow( _pos_s.y - _pos_l.y, 2) + pow( _pos_s.z - _pos_l.z, 2));
		pAdapter->setDistance( fDistance);

		_internal_update_channel( pChannel, pAdapter, 0.0);
	}


	// Call event
	OnCreateChannel( pChannel);


	// Set variant
	pSample->nReferenceCount++;
	pSample->tLastAccessTime = m_Timer.GetCurTime();


	// Remind BGM sound
	if ( pSample->nType == RSD_SOUNDTYPE_BGM)
	{
		int nNumChannels = 0;
		m_pChannelGroups[ RSD_CHANNELGROUP_BGM]->getNumChannels( &nNumChannels);
		for ( int i = 0;  i < nNumChannels;  i++)
		{
			FMOD::Channel* _channel;
			if ( m_pChannelGroups[ RSD_CHANNELGROUP_BGM]->getChannel( i, &_channel) != FMOD_OK)
				continue;

			if ( _channel == pChannel)
				continue;

			SoundAdapter* pAdapter;
			if ( _channel->getUserData( (void**)&pAdapter) != FMOD_OK)
				continue;

			pAdapter->setState( SoundAdapter::FADEOUT);
		}
	}


	// Get sound id
	int nIndex = RSD_CHANNELID_NONE;
	pChannel->getIndex( &nIndex);


	// Play sound
	pChannel->setPaused( false);


	// Set fade in
	if ( pSample->bLoop  ||  pSample->nType == RSD_SOUNDTYPE_BGM)
		pAdapter->setState( SoundAdapter::FADEIN);


	// Call event handler
	OnPlayChannel( pChannel);

	return nIndex;


rsound_error_create:
	
	if ( pAdapter != NULL)
		delete pAdapter;

	if ( pChannel != NULL)
	{
		bool bPlaying = false;
		pChannel->isPlaying( &bPlaying);

		if ( bPlaying == true)
			pChannel->stop();
	}

	return RSD_CHANNELID_NONE;
}


int RealSound::Stop( const char* szID, bool bMute)
{
	int nDeleted = 0;

	if ( m_pSystem == NULL)
		return nDeleted;

	string strID( szID);
	for ( int group = (RSD_CHANNELGROUP_MASTER + 1);  group < RSD_CHANNELGROUP_MAX;  group++)
	{
		int channels = 0;
		m_pChannelGroups[ group]->getNumChannels( &channels);
		for ( int channel = channels - 1;  channel >= 0;  channel--)
		{
			FMOD::Channel* pChannel = NULL;
			if ( m_pChannelGroups[ group]->getChannel( channel, &pChannel) != FMOD_OK)
				continue;

			void* userdata = NULL;
			if ( pChannel->getUserData( &userdata) != FMOD_OK)
				continue;

			SoundAdapter* pAdapter = (SoundAdapter*)userdata;
			if ( pAdapter->getID() == strID)
			{
				if ( bMute == true)
				{
					pChannel->stop();
				}
				else if ( pChannel->getUserData( &userdata) == FMOD_OK)
				{
					SoundAdapter* pAdapter = (SoundAdapter*)userdata;
					pAdapter->setState( SoundAdapter::FADEOUT);
				}

				nDeleted++;
			}
		}
	}

	return nDeleted;
}


void RealSound::StopAll( bool bMute)
{
	if ( m_pSystem == NULL)
		return;

	for ( int group = (RSD_CHANNELGROUP_MASTER + 1);  group < RSD_CHANNELGROUP_MAX;  group++)
	{
		int channels = 0;
		m_pChannelGroups[ group]->getNumChannels( &channels);
		for ( int channel = channels - 1;  channel >= 0;  channel--)
		{
			FMOD::Channel* pChannel = NULL;
			if ( m_pChannelGroups[ group]->getChannel( channel, &pChannel) != FMOD_OK)
				continue;

			FMOD::Sound* pSound = NULL;
			if ( pChannel->getCurrentSound( &pSound) != FMOD_OK)
				continue;

			if ( bMute == true)
			{
				pChannel->stop();
			}
			else
			{
				void* userdata = NULL;
				if ( pChannel->getUserData( &userdata) == FMOD_OK)
				{
					SoundAdapter* pAdapter = (SoundAdapter*)userdata;
					pAdapter->setState( SoundAdapter::FADEOUT);
				}
			}
		}
	}
}


size_t RealSound::GetSoundsPlaying() const
{
	int channels = 0;
	if ( m_pSystem != NULL)
		m_pSystem->getChannelsPlaying( &channels);

	return channels;
}


unsigned int RealSound::GetChannelPlayingTime( int nID)
{
	if ( m_pSystem == NULL)
		return 0;

	FMOD::Channel* pChannel;
	if ( m_pSystem->getChannel( nID, &pChannel) != FMOD_OK)
		return 0;

	FMOD::Sound* pSound;
	if ( pChannel->getCurrentSound( &pSound) != FMOD_OK)
		return 0;

	unsigned int nLength = 0;
	pSound->getLength( &nLength, FMOD_TIMEUNIT_MS);

	return nLength;
}


unsigned int RealSound::GetChannelPosition( int nID)
{
	if ( m_pSystem == NULL)
		return 0;

	FMOD::Channel* pChannel;
	if ( m_pSystem->getChannel( nID, &pChannel) == FMOD_OK)
	{
		unsigned int nPos = 0;
		pChannel->getPosition( &nPos, FMOD_TIMEUNIT_MS);

		return nPos;
	}

	return 0;
}


void RealSound::SetChannelPosition( int nID, unsigned int nPos)
{
	if ( m_pSystem == NULL)
		return;

	FMOD::Channel* pChannel;
	if ( m_pSystem->getChannel( nID, &pChannel) == FMOD_OK)
		pChannel->setPosition( nPos, FMOD_TIMEUNIT_MS);
}


bool RealSound::GetChannelVirtual( int nID)
{
	if ( m_pSystem == NULL)
		return false;

	FMOD::Channel* pChannel;
	if ( m_pSystem->getChannel( nID, &pChannel) != FMOD_OK)
		return false;
	
	bool bVirtual;
	pChannel->isVirtual( &bVirtual);

	return bVirtual;
}


float RealSound::GetChannelGroupVolume( int nGroup) const
{
	float fVolume = 1.0f;
	if ( m_pChannelGroups[ nGroup] != NULL)
		m_pChannelGroups[ nGroup]->getVolume( &fVolume);

	return fVolume;
}


void RealSound::SetChannelGroupVolume( int nGroup, float fVolume)
{
	if ( m_pChannelGroups[ nGroup] != NULL)
	{
		m_pChannelGroups[ nGroup]->setVolume( fVolume);
		m_pChannelGroups[ nGroup]->getVolume( &m_fChannelGroupsVolume[ nGroup]);
	}
}


bool RealSound::GetChannelGroupMute( int nGroup) const
{
	bool bMute = false;
	if ( m_pChannelGroups[ nGroup] != NULL)
		m_pChannelGroups[ nGroup]->getMute( &bMute);

	return bMute;
}


void RealSound::SetChannelGroupMute( int nGroup, bool bMute)
{
	if ( m_pChannelGroups[ nGroup] != NULL)
		m_pChannelGroups[ nGroup]->setMute( bMute);
}


void RealSound::SetListener( const float* _pos, const float* _forward, const float* _up)
{
	if ( m_pSystem == NULL)
		return;

	FMOD_VECTOR _vel = { 0.0f, 0.0f, 0.0f };
	if ( m_bDoppler == true)
	{
		FMOD_VECTOR _prev_pos;
		m_pSystem->get3DListenerAttributes( 0, &_prev_pos, NULL, NULL, NULL);

		float fDistanceFactor;
		m_pSystem->get3DSettings( NULL, &fDistanceFactor, NULL);

		TIMEVAL fElapsed = m_Timer.GetElapsedTime();
		float fMeterPerSec = ( fElapsed < 0.0001)  ?  0.0f : 1000.0f / ( (float)fElapsed * fDistanceFactor);
		_vel.x = ( _pos[ 0] - _prev_pos.x) * fMeterPerSec;
		_vel.y = ( _pos[ 1] - _prev_pos.y) * fMeterPerSec;
		_vel.z = ( _pos[ 2] - _prev_pos.z) * fMeterPerSec;
	}

	m_pSystem->set3DListenerAttributes( 0, (FMOD_VECTOR*)_pos, &_vel, (FMOD_VECTOR*)_forward, (FMOD_VECTOR*)_up);
}


void RealSound::SetReverb( int nReverb)
{
	if ( m_pSystem != NULL  &&  m_Reverb.nAmbient != nReverb)
	{
		m_pSystem->setReverbAmbientProperties( &m_Reverb.pPresets[ nReverb]);
		m_Reverb.nAmbient = nReverb;
	}
}


void RealSound::GetReverbProp( int nReverb, FMOD_REVERB_PROPERTIES* prop)
{
	if ( m_pSystem != NULL)
		memcpy( prop, &m_Reverb.pPresets[ nReverb], sizeof( FMOD_REVERB_PROPERTIES));
}


void RealSound::SetReverbProp( int nReverb, const FMOD_REVERB_PROPERTIES* prop)
{
	if ( m_pSystem != NULL)
		memcpy( &m_Reverb.pPresets[ nReverb], prop, sizeof( FMOD_REVERB_PROPERTIES));
}


void RealSound::AddLocalReverb( int nReverb, const float* pPos, float fMinDist, float fMaxDist)
{
	if ( m_pSystem == NULL)
		return;

	FMOD::Reverb* pReverb;
	if ( m_pSystem->createReverb( &pReverb) != FMOD_OK)
		return;

	m_Reverb.vLocal.push_back( pReverb);

	pReverb->setProperties( &m_Reverb.pPresets[ nReverb]);
	pReverb->set3DAttributes( (FMOD_VECTOR*)pPos, fMinDist, fMaxDist);
}


void RealSound::ClearLocalReverb()
{
	if ( m_pSystem == NULL)
		return;

	if ( m_Reverb.vLocal.empty() == false)
	{
		for ( vector< FMOD::Reverb*>::iterator itr = m_Reverb.vLocal.begin();  itr != m_Reverb.vLocal.end();  itr++)
			(*itr)->release();

		m_Reverb.vLocal.clear();
	}
}


bool RealSound::GetCompressor() const
{
	if ( m_pCompressor == NULL)
		return false;

	bool bByPass = true;
	m_pCompressor->getBypass( &bByPass);

	return (!bByPass);
}


void RealSound::SetCompressor( bool bEnable)
{
	if ( m_pCompressor != NULL)
		m_pCompressor->setBypass( !bEnable);
}


void RealSound::GetCompressorProp( float* threshold, float* attack, float* release, float* gainmakeup)
{
	if ( m_pCompressor != NULL)
	{
		if ( threshold != NULL)
			m_pCompressor->getParameter( FMOD_DSP_COMPRESSOR_THRESHOLD, threshold, NULL, 0);
		if ( threshold != NULL)
			m_pCompressor->getParameter( FMOD_DSP_COMPRESSOR_ATTACK, attack, NULL, 0);
		if ( threshold != NULL)
			m_pCompressor->getParameter( FMOD_DSP_COMPRESSOR_RELEASE, release, NULL, 0);
		if ( threshold != NULL)
			m_pCompressor->getParameter( FMOD_DSP_COMPRESSOR_GAINMAKEUP, gainmakeup, NULL, 0);
	}
}


void RealSound::SetCompressorProp( float threshold, float attack, float release, float gainmakeup)
{
	if ( m_pCompressor != NULL)
	{
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_THRESHOLD, threshold);
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_ATTACK, attack);
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_RELEASE, release);
		m_pCompressor->setParameter( FMOD_DSP_COMPRESSOR_GAINMAKEUP, gainmakeup);
	}
}


void RealSound::GetHRTF( bool* bEnable, float* fMinDegree, float* fMaxDegree, float* fScale)
{
	if ( bEnable != NULL)
		*bEnable = m_HRTF.bEnable;

	if ( fMinDegree != NULL)
		*fMinDegree = m_HRTF.fMinAngle * 180.0f / 3.141f;

	if ( fMaxDegree != NULL)
		*fMaxDegree = m_HRTF.fMaxAngle * 180.0f / 3.141f;

	if ( fScale != NULL)
		*fScale = m_HRTF.fScale;
}


void RealSound::SetHRTF( bool bEnable, float fMinDegree, float fMaxDegree, float fScale)
{
	m_HRTF.bEnable = bEnable;
	m_HRTF.fMinAngle = fMinDegree * 3.141f / 180.0f;
	m_HRTF.fMaxAngle = fMaxDegree * 3.141f / 180.0f;
	m_HRTF.fScale = fScale;
}


float RealSound::GetPitch() const
{
	float fPitch = 1.0f;
	if ( m_pChannelGroups[ RSD_CHANNELGROUP_2D] != NULL)
		m_pChannelGroups[ RSD_CHANNELGROUP_2D]->getPitch( &fPitch);

	return fPitch;
}


void RealSound::SetPitch( float fPitch)
{
	if ( m_pChannelGroups[ RSD_CHANNELGROUP_2D] != NULL)
		m_pChannelGroups[ RSD_CHANNELGROUP_2D]->setPitch( fPitch);

	if ( m_pChannelGroups[ RSD_CHANNELGROUP_3D] != NULL)
		m_pChannelGroups[ RSD_CHANNELGROUP_3D]->setPitch( fPitch);
}


int RealSound::GetSpeakerMode() const
{
	return 0;
}


void RealSound::SetSpeakerMode( int nSpeakerMode)
{
}


void RealSound::GetSpeakerPosition( int nSpeaker, float* x, float *y)
{
	if ( m_pSystem != NULL)
		m_pSystem->get3DSpeakerPosition( (FMOD_SPEAKER)nSpeaker, x, y, NULL);
}


void RealSound::SetSpeakerPosition( int nSpeaker, float x, float y)
{
	if ( m_pSystem != NULL)
		m_pSystem->set3DSpeakerPosition( (FMOD_SPEAKER)nSpeaker, x, y, true);
}


void RealSound::GetCPUUsage( float *pTotal, float *pDSP, float *pStream, float *pGeometry, float *pUpdate)
{
	if ( m_pSystem != NULL)
		m_pSystem->getCPUUsage( pDSP, pStream, pGeometry, pUpdate, pTotal);
}


void RealSound::Update()
{
	if ( m_pSystem == NULL)
		return;

	OnPreUpdate( m_pSystem);

	for ( int group = (RSD_CHANNELGROUP_MASTER + 1);  group < RSD_CHANNELGROUP_MAX;  group++)
	{
		int channels = 0;
		m_pChannelGroups[ group]->getNumChannels( &channels);
		for ( int channel = ( channels - 1);  channel >= 0;  channel--)
		{
			FMOD::Channel* pChannel = NULL;
			if ( m_pChannelGroups[ group]->getChannel( channel, &pChannel) != FMOD_OK)
				continue;

			void* userdata = NULL;
			if ( pChannel->getUserData( &userdata) != FMOD_OK)
				continue;

			SoundAdapter* pSoundAdapter = (SoundAdapter*)userdata;
			if ( pSoundAdapter->getState() == SoundAdapter::DESTROY)
			{
				pChannel->stop();
				continue;
			}

			FMOD_MODE mode;
			if ( pChannel->getMode( &mode) == FMOD_OK  &&  ( mode & FMOD_3D))
				_internal_update_channel( pChannel, pSoundAdapter, m_Timer.GetElapsedTime());
		}
	}

	OnUpdate( m_pSystem);

	m_pSystem->update();
}


FMOD_RESULT RealSound::OnCreateSound( Sample* pSample, FMOD_MODE mode)
{
	FMOD_RESULT result;

	switch ( pSample->nType)
	{
	case RSD_SOUNDTYPE_2D :
	case RSD_SOUNDTYPE_3D :
		result = m_pSystem->createSound( pSample->strFileName.c_str(), mode, NULL, &pSample->pSound);
		break;

	case RSD_SOUNDTYPE_BGM :
		result = m_pSystem->createStream( pSample->strFileName.c_str(), mode, NULL, &pSample->pSound);
		break;

	default:
		result = m_pSystem->createSound( pSample->strFileName.c_str(), mode, NULL, &pSample->pSound);
		break;
	}

	return result;
}


FMOD_RESULT	F_CALLBACK RealSound::_internal_callback_channel( FMOD_CHANNEL* pChannel, FMOD_CHANNEL_CALLBACKTYPE nType, void* pCommandData1, void* pCommandData2)
{
	if ( nType == FMOD_CHANNEL_CALLBACKTYPE_END)
	{
		FMOD::Channel* _channel = (FMOD::Channel*)pChannel;

		FMOD::System* pSystem = NULL;
		_channel->getSystemObject( &pSystem);

		void* userdata = NULL;
		if ( pSystem->getUserData( &userdata) == FMOD_OK)
		{
			RealSound* pRealSound = (RealSound*)userdata;
			pRealSound->_internal_stop_channel( _channel);
		}
	}

	return FMOD_OK;
}


float F_CALLBACK RealSound::_internal_callback_rolloff( FMOD_CHANNEL *pChannel, float fDistance)
{
	FMOD::Channel* _channel = (FMOD::Channel*)pChannel;

	void* userdata = NULL;
	if ( _channel->getUserData( &userdata) == FMOD_OK)
	{
		SoundAdapter* pSoundAdapter = (SoundAdapter*)userdata;
		pSoundAdapter->setDistance( fDistance);
	}

	FMOD::System* pSystem = NULL;
	_channel->getSystemObject( &pSystem);
	if ( pSystem->getUserData( &userdata) == FMOD_OK)
	{
		RealSound* pRealSound = (RealSound*)userdata;
		float fRolloff = pRealSound->_internal_rolloff_channel( _channel, fDistance);
		return fRolloff;
	}

	return 1.0f;
}


unsigned long WINAPI RealSound::_internal_thread( LPVOID lpParameter)
{
	RealSound* pRealSound = (RealSound*)lpParameter;

	pRealSound->_internal_thread_create();

	// Run thread
	bool bSampleUpdate = false;
	while ( pRealSound->_internal_is_thread_exit() == false)
	{
		pRealSound->_internal_update_channelgroup_volume();
		pRealSound->_internal_update_channel_volume();
		pRealSound->_internal_update_release_sound();

		Sleep( 1);
	}


	pRealSound->_internal_thread_destroy();
	return 0;
}


void RealSound::_internal_update_channel( FMOD::Channel* pChannel, SoundAdapter* pAdapter, TIMEVAL fElapsed)
{
	// Update position
	FMOD_VECTOR vPos = { 0.0f, 0.0f, 0.0f };
	if ( pAdapter->getPosition( &vPos) == true)
	{
		FMOD_VECTOR vVel = { 0.0f, 0.0f, 0.0f };
		if ( m_bDoppler == true)
		{
			FMOD_VECTOR vPrevPos = { 0.0f, 0.0f, 0.0f };
			pChannel->get3DAttributes( &vPrevPos, NULL);

			float fDistanceFactor;
			m_pSystem->get3DSettings( NULL, &fDistanceFactor, NULL);

			float fMeterPerSec = ( fElapsed < 0.0001)  ?  0.0f : 1000.0f / ( (float)fElapsed * fDistanceFactor);
			vVel.x = ( vPos.x - vPrevPos.x) * fMeterPerSec;
			vVel.y = ( vPos.y - vPrevPos.y) * fMeterPerSec;
			vVel.z = ( vPos.z - vPrevPos.z) * fMeterPerSec;
		}

		pChannel->set3DAttributes( &vPos, &vVel);
	}

	// Get distance ratio
	float fMinDist, fMaxDist;
	pChannel->get3DMinMaxDistance( &fMinDist, &fMaxDist);
	float fDistRatio = pAdapter->getDistance() / fMaxDist;

	// Get sample info
	FMOD::Sound* pSound;
	pChannel->getCurrentSound( &pSound);

	void* userdata;
	pSound->getUserData( &userdata);
	RealSound::Sample* pSample = (RealSound::Sample*)userdata;

	// Set loud 3D setting
	if ( pSample->bIgnoreFOD == false)
	{
		// Set pan-level
		float fPanLevel = 1.0f - exp( -12.0f * pow( fDistRatio, 2.0f));
		pChannel->set3DPanLevel( fPanLevel);

		// Set occlusion
		float fDirect = ( 1.0f - exp( -5.0f * pow( fDistRatio, 2.0f))) * 0.7f;
		float fReverb = ( 1.0f - exp( -1.5f * pow( fDistRatio, 2.0f))) * 0.35f + 0.75f;
		pChannel->set3DOcclusion( fDirect, fReverb);

		// Set low-pass filter
		FMOD::DSP* pHead = NULL;
		pChannel->getDSPHead( &pHead);
		if ( pHead != NULL)
		{
			int nInputs = 0;
			pHead->getNumInputs( &nInputs);
			for ( int i = 0;  i < nInputs;  i++)
			{
				FMOD::DSP* pDsp = NULL;
				if ( pHead->getInput( i, &pDsp, NULL) != FMOD_OK)
					continue;

				FMOD_DSP_TYPE nType;
				if ( pDsp->getType( &nType) != FMOD_OK  ||  nType != FMOD_DSP_TYPE_LOWPASS_SIMPLE)
					continue;


				// HRTF
				float fHRTF = 1.0f;
				if ( m_HRTF.bEnable == true)
				{
					pChannel->get3DAttributes( &vPos, NULL);

					FMOD_VECTOR _pos, _forward;
					m_pSystem->get3DListenerAttributes( 0, &_pos, NULL, &_forward, NULL);

					float _angle = 0.0f;
					FMOD_VECTOR _dir;
					_dir.x = vPos.x - _pos.x;
					_dir.y = vPos.y - _pos.y;
					_dir.z = vPos.z - _pos.z;
					if ( _normalize( &_dir) == true)
						_angle = acosf( _dot_product( _forward, _dir));

					fHRTF = 1.0f - m_HRTF.fScale * min( 1.0f, max( 0.0f, ( _angle - m_HRTF.fMinAngle) / ( m_HRTF.fMaxAngle - m_HRTF.fMinAngle)));
				}

				float fLowPass = 21000.0f * exp( -6.0f * pow( fDistRatio, 2.0f)) * fHRTF + 10.0f;
				pDsp->setParameter( FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, fLowPass);
				break;
			}
		}
	}

	// Set loudless 3D setting
	else
	{
		pChannel->set3DPanLevel( 0.0f);

		float fDirect = 1.0f - exp( -5.0f * pow( fDistRatio, 1.5f));
		pChannel->set3DOcclusion( fDirect, 1.0f);
	}
}


void RealSound::_internal_update_channelgroup_volume()
{
	m_Timer.Update();

	float fElapsed = (float)m_Timer.GetElapsedTime() * 0.0004f;
	for ( int i = 0;  i < RSD_CHANNELGROUP_MAX;  i++)
	{
		float fVolume;
		m_pChannelGroups[ i]->getVolume( &fVolume);

		if ( m_bSilence == true  &&  fVolume > 0.0f)
		{
			fVolume = pow( max( sqrt( fVolume) - fElapsed, 0.0f), 2);
			m_pChannelGroups[ i]->setVolume( fVolume);
		}
		else if ( m_bSilence == false  &&  fVolume < m_fChannelGroupsVolume[ i])
		{
			fVolume = pow( min( sqrt( fVolume) + fElapsed, m_fChannelGroupsVolume[ i]), 2);
			m_pChannelGroups[ i]->setVolume( fVolume);
		}
	}
}


void RealSound::_internal_update_channel_volume()
{
	// Set sound volume
	float fElapsed = (float)m_Timer.GetElapsedTime() * 0.0004f;
	for ( int group = (RSD_CHANNELGROUP_MASTER + 1);  group < RSD_CHANNELGROUP_MAX;  group++)
	{
		int channels = 0;
		m_pChannelGroups[ group]->getNumChannels( &channels);
		for ( int channel = channels - 1;  channel >= 0;  channel--)
		{
			FMOD::Channel* pChannel = NULL;
			if ( m_pChannelGroups[ group]->getChannel( channel, &pChannel) != FMOD_OK)
				continue;

			void* userdata = NULL;
			if ( pChannel->getUserData( &userdata) != FMOD_OK)
				continue;

			SoundAdapter* pAdapter = (SoundAdapter*)userdata;
			switch ( pAdapter->getState())
			{
			case SoundAdapter::FADEIN :
				{
					FMOD::Sound* pSound;
					pChannel->getCurrentSound( &pSound);

					pSound->getUserData( &userdata);
					Sample* pSample = (Sample*)userdata;

					float fVolume, fMaxVolume;
					pChannel->getVolume( &fVolume);

					fVolume = pow( sqrt( fVolume) + fElapsed, 2);
					fMaxVolume = (float)pSample->nVolume * 0.01f;
					if ( fVolume >= fMaxVolume)
					{
						pAdapter->setState( SoundAdapter::NORMAL);
						fVolume = fMaxVolume;
					}

					pChannel->setVolume( fVolume);

					break;
				}

			case SoundAdapter::FADEOUT :
				{
					float fVolume;
					pChannel->getVolume( &fVolume);

					float fRatio = sqrt( fVolume) - fElapsed;
					fVolume = pow( max( 0.0f, fRatio), 2);
					pChannel->setVolume( fVolume);

					if ( fRatio < 0.0f)
						pAdapter->setState( SoundAdapter::DESTROY);
					break;
				}
			}
		}
	}
}


void RealSound::_internal_update_release_sound()
{
	TIMEVAL curr = m_Timer.GetCurTime();

	// Lazy update
	if ( (curr - m_tLastUpdateReleaseSound) < 10000)
		return;

	for ( map< string, Sample*>::iterator itr = m_vSamples.begin();  itr != m_vSamples.end();  itr++)
	{
		Sample* pSample = (*itr).second;
		if ( pSample->nReferenceCount > 0  ||  pSample->pSound == NULL)
			continue;

		if ( (curr - pSample->tLastAccessTime) < 3600000 /* 5min */)
			continue;

		pSample->pSound->release();
		pSample->pSound = NULL;

		if ( pSample->pData != NULL)
		{
			delete pSample->pData;
			pSample->pData = NULL;
			pSample->nDataSize = 0;
		}
	}

	m_tLastUpdateReleaseSound = curr;
}


void RealSound::_internal_stop_channel( FMOD::Channel* pChannel)
{
	void* userdata = NULL;
	if ( pChannel->getUserData( &userdata) == FMOD_OK)
	{
		SoundAdapter* pSoundAdapter = (SoundAdapter*)userdata;
		delete pSoundAdapter;
	}

	FMOD::Sound* pSound = NULL;
	if ( pChannel->getCurrentSound( &pSound) == FMOD_OK)
	{
		if ( pSound->getUserData( &userdata) == FMOD_OK)
		{
			Sample* pSample = (Sample*)userdata;
			pSample->nReferenceCount--;
			pSample->tLastAccessTime = m_Timer.GetCurTime();
		}
	}

	OnReleaseChannel( pChannel);
}


float RealSound::_internal_rolloff_channel( FMOD::Channel* pChannel, float fDistance)
{
	return 1.0f;
}

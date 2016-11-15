
#pragma once

#include "fmod.hpp"
#include "RSoundTimer.h"
#include <string>
#include <map>
#include <vector>
using namespace std;


#define RSD_CHANNELID_NONE		-1



// Type of sound
enum
{
	RSD_SOUNDTYPE_NONE			= 0,				///< Unknown type
	RSD_SOUNDTYPE_2D,								///< 2D type
	RSD_SOUNDTYPE_3D,								///< 3D type
	RSD_SOUNDTYPE_BGM,								///< BGM type
	RSD_SOUNDTYPE_MAX
};


// Type of channel groups
enum
{
	RSD_CHANNELGROUP_MASTER		= 0,				///< Master sound channel group
	RSD_CHANNELGROUP_2D,							///< 2D sound channel group
	RSD_CHANNELGROUP_3D,							///< 3D sound channel group
	RSD_CHANNELGROUP_BGM,							///< BGM sound channel group
	RSD_CHANNELGROUP_MAX
};


// Type of reverb preset
enum
{
	RSD_REVERB_OFF				= 0,				///< Off
	RSD_REVERB_GENERIC,								///< Generic
	RSD_REVERB_PADDEDCELL,							///< Padded cell
	RSD_REVERB_ROOM,								///< Room
	RSD_REVERB_BATHROOM,							///< Bath room
	RSD_REVERB_LIVINGROOM,							///< Living room
	RSD_REVERB_STONEROOM,							///< Stone room
	RSD_REVERB_AUDITORIUM,							///< Auditorium
	RSD_REVERB_CONCERTHALL,							///< Concert hall
	RSD_REVERB_CAVE,								///< Cave
	RSD_REVERB_ARENA,								///< Arena
	RSD_REVERB_HANGAR,								///< Hangar
	RSD_REVERB_CARPETTEDHALLWAY,					///< Carpetted hallway
	RSD_REVERB_HALLWAY,								///< Hallway
	RSD_REVERB_STONECORRIDOR,						///< Stone corridor
	RSD_REVERB_ALLEY,								///< Alley
	RSD_REVERB_FOREST,								///< Forest
	RSD_REVERB_CITY,								///< City
	RSD_REVERB_MOUNTAINS,							///< Mountain
	RSD_REVERB_QUARRY,								///< Quarry
	RSD_REVERB_PLAIN,								///< Plain
	RSD_REVERB_PARKINGLOT,							///< Parking lot
	RSD_REVERB_SEWERPIPE,							///< Sewer pipe
	RSD_REVERB_UNDERWATER,							///< Under water
	RSD_REVERB_MAX
};


// Type of speaker mode
enum
{
	RSD_SPEAKERMODE_AUTO		= 0,				///< Auto
	RSD_SPEAKERMODE_STEREO,							///< 2 speaker setup
	RSD_SPEAKERMODE_QUAD,							///< 4 speaker setup
	RSD_SPEAKERMODE_5POINT1,						///< 5.1 speaker setup
	RSD_SPEAKERMODE_7POINT1,						///< 7.1 speaker setup
	RSD_SPEAKERMODE_MAX
};


// RealSound create info
struct SoundSystemCreateInfo
{
	int		SampleRate;
	int		SpeakerMode;
	int		MaxChannels;
	int		VirtualChannels;
	float	DopplerScale;
	float	DistanceFactor;
	float	RolloffScale;

	SoundSystemCreateInfo() : SampleRate( 44100), SpeakerMode( RSD_SPEAKERMODE_STEREO), MaxChannels( 100), VirtualChannels( 20),
		DopplerScale( 2.0f), DistanceFactor( 100.0f), RolloffScale( 1.0f)		{}
};


// Sound adapter
class SoundAdapter
{
	string			m_strID;
	unsigned char	m_nState;
	float			m_fDistance;

public:
	enum { NORMAL, FADEIN, FADEOUT, DESTROY };

	SoundAdapter() : m_nState( NORMAL), m_fDistance( 0.0f)		{}
	virtual ~SoundAdapter()										{}

	const string& setID( const char* szID)						{ return ( m_strID = szID);				}
	const string& getID()										{ return m_strID;						}

	int setState( int state)									{ return ( m_nState = ( m_nState != SoundAdapter::DESTROY) ? state : SoundAdapter::DESTROY);	}
	int getState() const										{ return m_nState;						}

	float setDistance( float distance)							{ return ( m_fDistance = distance);		}
	float getDistance() const									{ return m_fDistance;					}

	virtual bool getPosition( FMOD_VECTOR* pPos)				{ return false;							}
};






class RealSound
{
public:
	struct Sample
	{
		FMOD::Sound*	pSound;
		string			strName;
		string			strFileName;
		unsigned char	nType;
		unsigned char	nVolume;
		bool			bLoop;
		bool			bUnique;
		unsigned char	nPriority;
		unsigned short	nMinDist, nMaxDist;
		bool			bIgnoreFOD;
		TIMEVAL			tLastAccessTime;
		unsigned char	nReferenceCount;
		char*			pData;
		size_t			nDataSize;
	};
	typedef map< string, Sample*>	SampleMap;


protected:
	FMOD::System*				m_pSystem;
	FMOD::ChannelGroup*			m_pChannelGroups[ RSD_CHANNELGROUP_MAX];
	float						m_fChannelGroupsVolume[ RSD_CHANNELGROUP_MAX];
	SampleMap					m_vSamples;
	RSoundTimer					m_Timer;
	bool						m_bHardwareAccelerate;
	bool						m_bDoppler;
	bool						m_bSilence;
	FMOD::DSP*					m_pCompressor;
	TIMEVAL						m_tLastUpdateReleaseSound;

	// Reverb
	struct _REVERB_DATA
	{
		FMOD_REVERB_PROPERTIES*	pPresets;
		int						nAmbient;
		vector< FMOD::Reverb*>	vLocal;
	} m_Reverb;

	// HRTF
	struct _HRTF_DATA
	{
		bool					bEnable;
		float					fMinAngle;
		float					fMaxAngle;
		float					fScale;
	} m_HRTF;

	// Thread
	struct
	{
		HANDLE					hThread;
		HANDLE					hEventDestroy;
		bool					bExit;
		CRITICAL_SECTION		csLock;
	} m_Thread;



	// Method
public:
	RealSound();
	virtual ~RealSound();

	bool				Create( const SoundSystemCreateInfo* info =NULL);
	bool				Destroy();

	bool				AddSample( const char* name, const char* filename, int type, float volume =1.0f, bool loop =false, bool unique =false, int priority =128, int min_dist =100, int max_dist =1000, bool ignorefod =false);
	bool				RemoveSample( const char* szSampleName);
	void				RemoveAllSamples();
	bool				IsExistSample( const char* szSampleName);

	int					Play( const char* szSampleName, const char* szID =NULL, SoundAdapter* info =NULL);
	int					Stop( const char* szID, bool bMute =false);
	void				StopAll( bool bMute =false);

	FMOD::System*		GetSystem()									{ return m_pSystem;									}
	FMOD::ChannelGroup*	GetChannelGroup( int nGroup) const			{ return m_pChannelGroups[ nGroup];					}
	const SampleMap*	GetSampleList()								{ return &m_vSamples;								}
	TIMEVAL				GetCurTime()								{ return m_Timer.GetCurTime();						}
	size_t				GetSoundsPlaying() const;
	unsigned int		GetChannelPlayingTime( int nID);
	unsigned int		GetChannelPosition( int nID);
	void				SetChannelPosition( int nID, unsigned int nPos);
	bool				GetChannelVirtual( int nID);
	float				GetChannelGroupVolume( int nGroup) const;
	void				SetChannelGroupVolume( int nGroup, float fVolume);
	void				SetChannelGroupMute( int nGroup, bool bMute);
	bool				GetChannelGroupMute( int nGroup) const;
	bool				GetHardwareAccelerate() const				{ return m_bHardwareAccelerate;						}
	void				SetHardwareAccelerate( bool bAccelerate)	{ m_bHardwareAccelerate = m_pSystem ? bAccelerate : m_bHardwareAccelerate;	}
	bool				GetDoppler() const							{ return m_bDoppler;								}
	void				SetDoppler( bool bDoppler)					{ m_bDoppler = m_pSystem ? bDoppler : m_bDoppler;	}
	bool				GetSilence() const							{ return m_bSilence;								}
	void				SetSilence( bool bSilence)					{ m_bSilence = m_pSystem ? bSilence : m_bSilence;	}
	void				SetListener( const float* pPos, const float* pForward, const float* pUp);
	int					GetReverb() const							{ return m_Reverb.nAmbient;							}
	void				SetReverb( int nReverb);
	void				GetReverbProp( int nReverb, FMOD_REVERB_PROPERTIES* prop);
	void				SetReverbProp( int nReverb, const FMOD_REVERB_PROPERTIES* prop);
	void				AddLocalReverb( int nReverb, const float* pPos, float fMinDist =100.0f, float fMaxDist =1000.0f);
	void				ClearLocalReverb();
	bool				GetCompressor() const;
	void				SetCompressor( bool bEnable);
	void				GetCompressorProp( float* threshold, float* attack, float* release, float* gainmakeup);
	void				SetCompressorProp( float threshold, float attack, float release, float gainmakeup);
	void				GetHRTF( bool* bEnable, float* fMinDegree, float* fMaxDegree, float* fScale);
	void				SetHRTF( bool bEnable, float fMinDegree, float fMaxDegree, float fScale);
	float				GetPitch() const;
	void				SetPitch( float fPitch);
	int					GetSpeakerMode() const;
	void				SetSpeakerMode( int nSpeakerMode);
	void				GetSpeakerPosition( int nSpeaker, float* x, float *y);
	void				SetSpeakerPosition( int nSpeaker, float x, float y);
	void				GetCPUUsage( float* pTotal, float* pDSP =NULL, float* pStream =NULL, float* pGeometry =NULL, float* pUpdate =NULL);

	void				Update();


	// Event handler
protected:
	virtual void		OnCreatedSystem( FMOD::System* pSystem)				{}
	virtual void		OnDestroyedSystem()									{}
	virtual FMOD_RESULT	OnCreateSound( Sample* pSample, FMOD_MODE mode);
	virtual void		OnReleaseSound( FMOD::Sound* pSound)				{}
	virtual void		OnCreateChannel( FMOD::Channel* pChannel)			{}
	virtual void		OnReleaseChannel( FMOD::Channel* pChannel)			{}
	virtual void		OnPlayChannel( FMOD::Channel* pChannel)				{}
	virtual void		OnPreUpdate( FMOD::System* pSystem)					{}
	virtual void		OnUpdate( FMOD::System* pSystem)					{}


	// Internal functions : DO NOT CALL THIS FUNCTIONS!
public:
	static FMOD_RESULT F_CALLBACK _internal_callback_channel( FMOD_CHANNEL* pChannel, FMOD_CHANNEL_CALLBACKTYPE nType, void* pCommandData1, void* pCommandData2);
	static float F_CALLBACK _internal_callback_rolloff( FMOD_CHANNEL *pChannel, float fDistance);
	static unsigned long WINAPI _internal_thread( LPVOID lpParameter);
	void _internal_update_channel( FMOD::Channel* pChannel, SoundAdapter* pAdapter, TIMEVAL fElapsed);
	bool _internal_is_thread_exit() const		{ return m_Thread.bExit;											}
	void _internal_thread_create()				{ m_Thread.hEventDestroy = CreateEvent( NULL, FALSE, FALSE, NULL);	}
	void _internal_thread_destroy()				{ SetEvent( m_Thread.hEventDestroy);								}
	void _internal_update_channelgroup_volume();
	void _internal_update_channel_volume();
	void _internal_update_release_sound();
	void _internal_stop_channel( FMOD::Channel* pChannel);
	float _internal_rolloff_channel( FMOD::Channel* pChannel, float fDistance);
};

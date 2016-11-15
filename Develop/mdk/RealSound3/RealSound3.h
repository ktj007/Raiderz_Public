#ifndef _REALSOUND3
#define _REALSOUND3

#include "RSample.h"
#include "RSound.h"
#include "RSoundTimer.h"
#include "fmod.hpp"
#include <map>
#include <list>
#include <vector>
using namespace std;



/// Type of channel groups
enum
{
	RSD_CHANNELGROUP_MASTER		= 0,				///< Master sound channel group
	RSD_CHANNELGROUP_GENERIC,						///< Generic sound channel group
	RSD_CHANNELGROUP_BGM,							///< BGM sound channel group
	RSD_CHANNELGROUP_EFFECT,						///< Effect sound channel group
	RSD_CHANNELGROUP_MAX
};


/// Type of effect channel groups
enum
{
	RSD_EFFECTCHNLGRP_GENERIC,						///< Generic effect sound channel group
	RSD_EFFECTCHNLGRP_EMPHASIZE,					///< Emphasized effect sound channel group
	RSD_EFFECTCHNLGRP_MAX
};


/// Type of roll-off model
enum
{
	RSD_ROLLOFF_LOG				= 0,				///< Logarithmic roll-off model
	RSD_ROLLOFF_LINEAR,								///< Linear roll-off model
	RSD_ROLLOFF_MAX
};


/// Type of reverb preset
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


/// Type of speaker mode
enum
{
	RSD_SPEAKERMODE_AUTO		= 0,				///< Auto
	RSD_SPEAKERMODE_STEREO,							///< 2 speaker setup
	RSD_SPEAKERMODE_QUAD,							///< 4 speaker setup
	RSD_SPEAKERMODE_5POINT1,						///< 5.1 speaker setup
	RSD_SPEAKERMODE_7POINT1,						///< 7.1 speaker setup
	RSD_SPEAKERMODE_MAX
};


/// Type of speaker
enum
{
	RSD_SPEAKER_LEFT			= 0,				///< Left speaker
	RSD_SPEAKER_RIGHT,								///< Right speaker
	RSD_SPEAKER_CENTER,								///< Center speaker
	RSD_SPEAKER_LOW_FREQUENCY,						///< Low frequency speaker
	RSD_SPEAKER_BACK_LEFT,							///< Back-left speaker
	RSD_SPEAKER_BACK_RIGHT,							///< Back-right speaker
	RSD_SPEAKER_SIDE_LEFT,							///< Side-left speaker
	RSD_SPEAKER_SIDE_RIGHT,							///< Side_right speaker
	RSD_SPEAKER_MAX
};


/// Defines of play state
#define PLAY_DEFAULT			0x00000000
#define PLAY_USER				0x00000001			///< For user
#define PLAY_USER_FIRST			0x00000001			///< For user
#define PLAY_USER_LAST			0x0000FFFF			///< For user
#define PLAY_IMPORTANT			0x00010000			///< Don't 3D sound make to virtual
#define PLAY_EMPHASIZE			0x00020000			///< Emphasized 3D sound










/// class : RChannelGrpVolFader
class RChannelGrpVolFader
{
protected:
	FMOD::ChannelGroup*	m_pChannelGroup;
	int					m_nState;
	TIMEVAL				m_tStartTime;
	TIMEVAL				m_tFadeTime;
	float				m_fStartVolume;
	float				m_fEndVolume;

public:
	RChannelGrpVolFader();

	void Create( FMOD::ChannelGroup* pChannelGroup, int nFadeTime =2000);
	void Destroy();

	void FadeIn( float fEndVolume =1.0f);
	void FadeOut( float fEndVolume =0.0f);
	void Update();
};





/// class : RealSound3
class RealSound3
{
	/// Member variables
protected:
	bool								m_bCreate;
	FMOD::System*						m_pSystem;									///< System of the FMOD
	FMOD::ChannelGroup*					m_pChannelGroups[ RSD_CHANNELGROUP_MAX];	///< Channel groups
	FMOD::ChannelGroup*					m_pEffectChnlGrp[ RSD_EFFECTCHNLGRP_MAX];	///< Effect channel groups
	map< string, RSample*>				m_vSamples;									///< List of samples
	list< pair< string, RSound*> >		m_vSounds;									///< List of sounds
	map< string, RSample*>::iterator			m_itrScanSample;					///< Sample list scanning iterator
	list< pair< string, RSound*> >::iterator	m_itrScanSound;						///< Sound list scanning iterator
	float								m_fVolume[ RSD_CHANNELGROUP_MAX];			///< Volumes of channel groups
	bool								m_bMute[ RSD_CHANNELGROUP_MAX];				///< Mutes of channel groups
	bool								m_bHardwareAccelerated;						///< Hardware acceleration
	float								m_fPanningDistance;							///< 3D panning distance
	int									m_nRollOffModel;							///< Model of roll-off
	RSound*								m_pCurrentBGM;								///< Current BGM
	bool								m_bSilence;									///< Silence
	RChannelGrpVolFader					m_MasterChnlGrpVolFader;					///< Master channel group volume fader
	RChannelGrpVolFader					m_EffectChnlGrpVolFader;					///< Effect channel group volume fader

	// System
	struct _SYSTEM_DATA
	{
		int								nMaxChannels;								///< Maximum channels
		int								nSoftwareChannels;							///< Software channels
		int								nSpeakerMode;								///< Speaker mode
		int								nSampleRate;								///< Sampling rate
	} m_SystemInfo;

	// Reverb
	struct _REVERB_DATA
	{
		bool							bEnable;									///< Enable reverb
		int								nAmbient;									///< Ambient reverb index
		vector< FMOD::Reverb*>			vLocal;										///< Local reverbs
	} m_Reverb;

	// Compressor
	struct _COMPRESSOR_DATA
	{
		bool							bEnable;									///< Enable compressor
		FMOD::DSP*						pDSP;										///< DSP pointer
		float							fThreshold;									///< Threshold factor
		float							fAttack;									///< Attack factor
		float							fRelease;									///< Release factor
		float							fGainMakeUp;								///< Gain make-up
	} m_Compressor;

	// Thread
	struct
	{
		HANDLE							hThread;									///< Handle of thread
		HANDLE							hEventDestroy;								///< Event handle of destroy thread
		bool							bExit;										///< Flag to exit thread
		CRITICAL_SECTION				csLock;										///< Critical section
	} m_Thread;

	// System shutdown info
	struct
	{
		bool							bShutdown;									///< Flag to shutdown system
		bool							bReverb;									///< Reverb flag
		bool							bCompressor;								///< Compressor flag
	} m_ShutdownInfo;



	/// Member functions
public:
	// Constructor/destructor
	RealSound3();
	virtual ~RealSound3();

	// Create/destroy
	bool Create( int nMaxChannels =100, int nSoftwareChannels =10, int nSpeakerMode =RSD_SPEAKERMODE_STEREO, int nSampleRate =44100);
	bool Destroy();

	// Add/remove sample
	virtual bool AddSample( const char* szSampleName, RSample* pSample);
	virtual bool RemoveSample( const char* szSampleName);
	RSample* GetSample( const char* szSampleName);
	bool IsExistSample( const char* szSampleName);
	void RemoveAllSamples();

	// Play/stop sound
	RSound* Play( const char* szSampleName, const char* szID =NULL, DWORD dwData =PLAY_DEFAULT);
	bool Stop( const char* szID, bool bMute =false);
	void StopAll( bool bMute =false);

	// Release all sound
	void ReleaseAllSound();

	// Set listener
	void SetListener( const float* pPos, const float* pVel, const float* pForward, const float* pUp);

	// Update
	void Update();


	// Shutdown/Restore system
protected:
	void _system_shutdown();
	void _system_restore();


	/// Get/Set functions
public:
	// Get FMOD system pointer
	FMOD::System* GetSystem() const;

	// Get sample/sound list
	const map< string, RSample*>* GetSampleList();
	const list< pair< string, RSound*> >* GetSoundList();

	// Get channel group
	FMOD::ChannelGroup* GetChannelGroup( int nGroup) const;

	// Get sounds playing
	size_t GetSoundsPlaying() const;

	// Get/Set volume
	void SetVolume( int nChannelGroup, float fVolume);
	float GetVolume( int nChannelGroup) const;

	// Get/Set silence
	void SetSilence( bool bSilence);
	bool GetSilence() const;

	// Get/Set mute
	void SetMute( int nChannelGroup, bool bMute);
	bool GetMute( int nChannelGroup) const;

	// Enable reverb
	void EnableReverb( bool bEnable);
	bool IsEnabledReverb() const;

	// Get/Set reverb
	void SetReverb( int nReverb);
	int GetReverb() const;

	// Add/Clear local reverb
	void AddLocalReverb( int nReverb, const float* pPos, float fMinDist =100.0f, float fMaxDist =1000.0f);
	void ClearLocalReverb();

	// Enable compressor
	void EnableCompressor( bool bEnable);
	bool IsEnabledCompressor() const;

	// Get/Set compressor
	void SetCompressor( float fThreshold =0.0f, float fAttack =50.0f, float fRelease =50.0f, float pGainMakeUp =0.0f);
	void GetCompressor( float* pThreshold, float* pAttack, float* pRelease, float* pGainMakeup);

	// Get/Set hardware acceleration
	void SetHardwareAccelerate( bool bAccelerate);
	bool IsHardwareAccelerate() const;

	// Get/Set 3D effect panning distance
	void SetPanningDistance( float fDistance);
	float GetPanningDistance() const;

	// Get/Set roll-off model
	void SetRollOffModel( int nModel);
	int GetRollOffModel() const;

	// Get/Set pitch
	void SetPitch( float fPitch);
	float GetPitch() const;

	// Get/Set speaker mode
	void SetSpeakerMode( int nSpeakerMode);
	int GetSpeakerMode() const;

	// Get/Set speaker position
	void SetSpeakerPosition( int nSpeaker, float x, float y);
	void GetSpeakerPosition( int nSpeaker, float* x, float *y);

	// Get CPU usage
	void GetCPUUsage( float *pTotal, float *pDSP =NULL, float *pStream =NULL, float *pGeometry =NULL, float *pUpdate =NULL);


	/// Thread
public:
	static unsigned long WINAPI _thread( LPVOID lpParameter);
	bool _is_exit_thread() const;
	void _on_create_thread();
	void _on_destroy_thread();
	void _on_update_samples();
	void _on_update_volumes();
	void Lock();
	void Unlock();


	/// Callback function
public:
	static FMOD_RESULT F_CALLBACK _callback_channel( FMOD_CHANNEL* pChannel, FMOD_CHANNEL_CALLBACKTYPE nType, void* pCommandData1, void* pCommandData2);
	static float F_CALLBACK _callback_rolloff( FMOD_CHANNEL *pChannel, float fDistance);


	/// You can override this function to create custom classes.
protected:
	virtual RSound* overrideNewSound( const RSample* pSample, DWORD dwData)		{ return new RSound();		}


	/// Event handler
protected:
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnPreUpdate();
	virtual void OnUpdate();
};

#endif

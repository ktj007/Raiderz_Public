#include "stdafx.h"
#include "RealSoundFMODEX.h"
#include "fmod_errors.h"

#include "RFMODSoundMgr.h"

#define RS2_SAMPLE_RATE          44100

namespace RS2
{
	void ERRCHECK(FMOD_RESULT result)
	{
		if (result != FMOD_OK)
		{
			dlog("[FMOD error!] %s\n", FMOD_ErrorString(result));
		}
	}

	RealSoundFMODEX::RealSoundFMODEX(void)
	:m_pFMODSystem(NULL)
	{
		m_fDopplerfactor = 1.0f;
		m_fRollfactor = 1.0f;
		m_fDistfactor = 1.0f;
	}

	RealSoundFMODEX::~RealSoundFMODEX(void)
	{
		FinalFMOD();
	}

	bool RealSoundFMODEX::InitFMOD()
	{
		//mlog("RealSoundFMODEX::InitFmod() \r");
	
		unsigned int		version;
		FMOD_CAPS			caps;
		int					numdrivers;
		FMOD_SPEAKERMODE	speakermode;
		char				name[256];



		result = FMOD::System_Create(&m_pFMODSystem);
		ERRCHECK(result);

		if (m_pFMODSystem == NULL) return false;

		result = m_pFMODSystem->getVersion(&version);
		ERRCHECK(result);

		if (version < FMOD_VERSION)
		{
			printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
			return false;
		}

		result = m_pFMODSystem->getNumDrivers(&numdrivers);
		ERRCHECK(result);

		if (numdrivers == 0)
		{
			result = m_pFMODSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
			ERRCHECK(result);
		}
		else
		{
			result = m_pFMODSystem->setOutput(FMOD_OUTPUTTYPE_DSOUND);
			ERRCHECK(result);

			result = m_pFMODSystem->getDriverCaps(0, &caps, 0, 0, &speakermode); 
			ERRCHECK(result);

			result = m_pFMODSystem->setSpeakerMode(speakermode);
			ERRCHECK(result);


			if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
			{                                                   /* You might want to warn the user about this. */
				result = m_pFMODSystem->setDSPBufferSize(1024, 10);
				ERRCHECK(result);
			}

			result = m_pFMODSystem->getDriverInfo(0, name, 256, 0);
			ERRCHECK(result);

			if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
			{
				result = m_pFMODSystem->setSoftwareFormat(RS2_SAMPLE_RATE, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
				ERRCHECK(result);
			}
			else
			{
				//샘플레이트 48000 -> 44100
				result = m_pFMODSystem->setSoftwareFormat(RS2_SAMPLE_RATE, FMOD_SOUND_FORMAT_PCM16, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
				ERRCHECK(result);
			}

		}

		result = m_pFMODSystem->init(100, FMOD_INIT_NORMAL, 0);
		ERRCHECK(result);

		if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
		{
			result = m_pFMODSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			ERRCHECK(result);

			result = m_pFMODSystem->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
			ERRCHECK(result);
		}

		int nChannel = 0;
		result = m_pFMODSystem->getSoftwareChannels(&nChannel);
		ERRCHECK(result);
		//mlog("[[[software Channels = %d]]]]\r", nChannel);

		int n2DChannel = 0;
		int n3DChannel = 0;
		result = m_pFMODSystem->getHardwareChannels(&n2DChannel, &n3DChannel, &nChannel);
		ERRCHECK(result);
		//mlog("[[[hardware 2DChannel = %d, 3DChannel = %d, TotalChannels = %d]]]]\r"
		//	, n2DChannel, n3DChannel, nChannel);

		return true;
	}

	void RealSoundFMODEX::FinalFMOD()
	{
		FMOD_RESULT       result;

		result = m_pFMODSystem->release();
		ERRCHECK(result);

		m_pFMODSystem = NULL;
	}

	void RealSoundFMODEX::Update()
	{
		if (m_pFMODSystem == NULL) return;

		FMOD_RESULT       result;
		
		result = m_pFMODSystem->update();
		ERRCHECK(result);
	}

	RealSoundChannel* RealSoundFMODEX::PlaySound( FMOD::Sound *sound, float fVolume )
	{
		RealSoundChannel* channel = 0;

		result = m_pFMODSystem->playSound(FMOD_CHANNEL_FREE
			, sound, true, (FMOD::Channel**)(&channel));
		ERRCHECK(result);

		result = channel->setVolume(fVolume);
		ERRCHECK(result);

		return channel;
	}

	RealSoundChannel* RealSoundFMODEX::PlaySound( FMOD::Sound *sound )
	{
		RealSoundChannel* channel = 0;

		result = m_pFMODSystem->playSound(FMOD_CHANNEL_FREE
			, sound, true, (FMOD::Channel**)(&channel));
		ERRCHECK(result);

		return channel;
	}

	// 플레이중이던 사운드는 그냥 정지가 되버려서 callback 함수로
	// 들어오지 않아 레퍼런스카운트가 줄지 않는다. 그래서 따로 줄여준다.
	//사운드와 뮤직 그룹을 스탑해줘야 하는데....
	//따로 해줘도 되고 마스터 그룹에 add 하게 되면 안해줘도 된다.
	void RealSoundFMODEX::Stop()
	{
		int nChannel = 0;
		result = m_pFMODSystem->getSoftwareChannels(&nChannel);
		ERRCHECK(result);

		result = m_pFMODSystem->getHardwareChannels(NULL, NULL, &nChannel);
		ERRCHECK(result);

		for (int i=0; i < nChannel; ++i)
		{
			RealSoundChannel* channel = 0;
			GetFMODSystem()->getChannel(i, (FMOD::Channel**)(&channel));

			RFMODSound* pFMODSound = NULL;
			channel->getUserData((void**)&pFMODSound);

			if (pFMODSound == NULL) continue;
			
			bool bPlaying = false;
			channel->isPlaying(&bPlaying);

			if (bPlaying)
			{
				pFMODSound->refSub();
			}
		}

		FMOD::ChannelGroup* channelgroup;
		result = GetFMODSystem()->getMasterChannelGroup(&channelgroup);
		ERRCHECK(result);

		result = channelgroup->stop();
		ERRCHECK(result);
	}

	void RealSoundFMODEX::SetMute(bool bMute)
	{
		FMOD::ChannelGroup* channelgroup;
		result = GetFMODSystem()->getMasterChannelGroup(&channelgroup);
		ERRCHECK(result);

		result = channelgroup->setMute(bMute);
		ERRCHECK(result);
	}

	// TODO : 사운드와 뮤직 채널이 생기면서 마스터채널이 쓸모가 없어졌지만 놔둔다.
	void RealSoundFMODEX::SetMasterVolume(float fVolume)
	{
		FMOD::ChannelGroup* channelgroup;
		result = GetFMODSystem()->getMasterChannelGroup(&channelgroup);
		ERRCHECK(result);

		result = channelgroup->setVolume(fVolume);
		ERRCHECK(result);
	}

	void RealSoundFMODEX::SetPan(float pan)
	{
		FMOD::ChannelGroup* channelgroup;
		result = GetFMODSystem()->getMasterChannelGroup(&channelgroup);
		ERRCHECK(result);

		result = channelgroup->overridePan(pan);
		ERRCHECK(result);
	}

	//void RealSoundFMODEX::SetDopplerFactor( float factor /*= 1.0f*/ )
	//{
	//	m_fDopplerfactor = factor;

	//	Set3DSetting(m_fDopplerfactor, m_fDistfactor, m_fRollfactor);
	//}

	//void RealSoundFMODEX::SetDistanceFactor( float factor /*= 1.0f*/ )
	//{
	//	m_fDistfactor = factor;

	//	Set3DSetting(m_fDopplerfactor, m_fDistfactor, m_fRollfactor);
	//}

	//void RealSoundFMODEX::SetRollFactor( float factor /*= 1.0f*/ )
	//{
	//	m_fRollfactor = factor;

	//	Set3DSetting(m_fDopplerfactor, m_fDistfactor, m_fRollfactor);
	//}

	void RealSoundFMODEX::Set3DSetting(float dopplerscale, float distancefactor, float rolloffscale)
	{
		result = GetFMODSystem()->set3DSettings(dopplerscale, distancefactor, rolloffscale);
		ERRCHECK(result);
	}

	void RealSoundFMODEX::Get3DSetting(float* dopplerscale, float* distancefactor, float* rolloffscale)
	{
		result = GetFMODSystem()->get3DSettings(dopplerscale, distancefactor, rolloffscale);
		ERRCHECK(result);
	}

	// Specify 0 if there is only 1 listener
	void RealSoundFMODEX::SetListener( const float* pos, const float* vel, const float* forward, const float* up )
	{
		result = m_pFMODSystem->set3DListenerAttributes(0, (FMOD_VECTOR*)pos
			, (FMOD_VECTOR*)vel, (FMOD_VECTOR*)forward, (FMOD_VECTOR*)up);
		ERRCHECK(result);
	}
}

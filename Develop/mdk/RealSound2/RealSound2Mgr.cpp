#include "stdafx.h"
#include "MTypes.h"
#include "RealSound2Mgr.h"
#include "RealSoundFMODEX.h"
#include "RTranseCoord.h"
#include "ReverdEffectType.h"

#define SOUNDBUFFERSIZE				1000000000		///< 사운드 버퍼의 사이즈

namespace RS2
{
	extern void ERRCHECK(FMOD_RESULT result);

	FMOD_RESULT	F_CALLBACK RealSound2Mgr::CHANNEL_END_CALLBACK( FMOD_CHANNEL* channel, FMOD_CHANNEL_CALLBACKTYPE type, void* commanddata1, void* commanddata2 )
	{
		if (type == FMOD_CHANNEL_CALLBACKTYPE_END)
		{
			FMOD::Channel* pChannel = (FMOD::Channel*)channel;
			if (pChannel == NULL) return FMOD_OK;

			void* userdata = NULL;
			pChannel->getUserData(&userdata);
			if(userdata == NULL) return FMOD_OK;

			RFMODSound* pSound = (RFMODSound*)userdata;
			pSound->refSub();
		}

		return FMOD_OK;
	}

	RealSound2Mgr::RealSound2Mgr( void )
	{
		m_pRealSound	= NULL;
		m_pFMODSoundMgr = NULL;
		m_Result		= FMOD_OK;

		m_MusicChannelGroup = NULL;
		m_EffectChannelGroup = NULL;

		m_bMute			= false;
		m_bBGMMute		= false;
		m_bEffectMute	= false;

		m_fEffectVolume	= 1.f;
		m_fMusicVolume	= 1.f;
		m_fMasterVolume = 1.f;

		m_nReverbEffectType	= 0;

		m_bHardwareAccelerated = true;
		m_nRollOffType = RS_LINEAR_ROLLOFF;
	}

	RealSound2Mgr::~RealSound2Mgr( void )
	{
		SAFE_DELETE(m_pFMODSoundMgr);
		m_pFMODSystem = NULL;
		SAFE_DELETE(m_pRealSound);
	}

	void RealSound2Mgr::CreateMusicChannelGroup()
	{
		if (m_MusicChannelGroup != NULL) return;

		m_Result = m_pFMODSystem->createChannelGroup("music", &m_MusicChannelGroup);
		ERRCHECK(m_Result);

		FMOD::ChannelGroup* masterchannelgroup;
		m_Result = m_pFMODSystem->getMasterChannelGroup(&masterchannelgroup);
		ERRCHECK(m_Result);

		masterchannelgroup->addGroup(m_MusicChannelGroup);
	}

	void RealSound2Mgr::CreateEffectChannelGroup()
	{
		if (m_EffectChannelGroup != NULL) return;

		m_Result = m_pFMODSystem->createChannelGroup("effect", &m_EffectChannelGroup);
		ERRCHECK(m_Result);

		FMOD::ChannelGroup* masterchannelgroup;
		m_Result = m_pFMODSystem->getMasterChannelGroup(&masterchannelgroup);
		ERRCHECK(m_Result);

		masterchannelgroup->addGroup(m_EffectChannelGroup);
	}

	void RealSound2Mgr::AddChannelToMusicChannelGroup(RealSoundChannel* channel)
	{
		//CreateMusicChannelGroup();
		_ASSERT(m_MusicChannelGroup != NULL);

		m_Result = channel->setChannelGroup(m_MusicChannelGroup);
		ERRCHECK(m_Result);
	}

	void RealSound2Mgr::AddChannelToEffectChannelGroup(RealSoundChannel* channel)
	{
		//CreateEffectChannelGroup();
		_ASSERT(m_EffectChannelGroup != NULL);

		m_Result = channel->setChannelGroup(m_EffectChannelGroup);
		ERRCHECK(m_Result);
	}

	void RealSound2Mgr::Update()
	{
		if (m_pRealSound != NULL)
		{
			m_pRealSound->Update();
		}

		m_pFMODSoundMgr->UpdateReleaseSound();
	}

	void RealSound2Mgr::IsGreatThenBuffersize()
	{
		if ( (m_pRealSound != NULL) && GetUsageMemory() > SOUNDBUFFERSIZE)
		{
			RFMODSound* pFMODSound = m_pFMODSoundMgr->GetFMODSoundOldAccessTime();
			if (pFMODSound != NULL)
			{
				pFMODSound->ReleaseSound();
			}
		}
	}

	bool RealSound2Mgr::CreateRealSound2()
	{
		m_pRealSound = new RealSoundFMODEX;
		if(m_pRealSound->InitFMOD() == false)
		{
			SAFE_DELETE(m_pRealSound);
			return false;
		}

		// distance factor를 우리게임에 맞도록.
		m_pRealSound->Set3DSetting(1.0f, 100.0f, 1.0f);

		m_pFMODSystem = m_pRealSound->GetFMODSystem();
		m_pFMODSoundMgr = new RFMODSoundMgr(m_pFMODSystem);

		CreateMusicChannelGroup();
		CreateEffectChannelGroup();

		return true;
	}

	RealSoundChannel* RealSound2Mgr::PlaySound( const string& strFileName, FMOD_MODE mode )
	{
		RFMODSound* pSound = NULL;
		RealSoundChannel* channel = 0;

		if (IsMute()) return channel;

		pSound = m_pFMODSoundMgr->GetFMODSound(strFileName, mode);
		if (pSound == NULL) return NULL;

		channel = m_pRealSound->PlaySound(pSound->GetSound());

		if (channel)
		{
			channel->setUserData(static_cast<void*>(pSound));
			SetCallback(channel);
		}

		AddChannelToEffectChannelGroup(channel);

		return channel;
	}

	RealSoundChannel* RealSound2Mgr::Play3DSound( const string& strFileName, FMOD_MODE mode, float fVolume, vec3 pos, vec3 vel, float fDistMin/*=1.0f*/, float fDistMax/*=10000.0f*/, int nPriority/*=128*/ )
	{
		RFMODSound* pSound = NULL;
		RealSoundChannel* channel = 0;

		if (IsMute()) return channel;

		pSound = m_pFMODSoundMgr->GetFMODSound(strFileName, mode, fDistMin, fDistMax);
		if (pSound == NULL) return NULL;

		//if (mode & FMOD_3D)
		//{
		//	if (Check3DSoundPlayable(pSound, pos, fDistMin, fDistMax) == false) return NULL;
		//}

		channel = m_pRealSound->PlaySound(pSound->GetSound());

		if (channel)
		{
			channel->setUserData(static_cast<void*>(pSound));
			SetCallback(channel);
		}

		SetChannelVolume(channel, fVolume);

		SetChannel3DAttributes(channel, pos, vel);

		SetPriority(channel, nPriority);

		AddChannelToEffectChannelGroup(channel);

		return channel;
	}

	void RealSound2Mgr::SetReverbEffectType( int effecttype )
	{
		m_nReverbEffectType = effecttype;

		FMOD_REVERB_PROPERTIES prop = ReverdEffectType[m_nReverbEffectType];
		m_Result = m_pFMODSystem->setReverbProperties(&prop);
		ERRCHECK(m_Result);
	}

	const char* RealSound2Mgr::GetReverbEffectType()
	{
		return ReverdEffectTypeStr[m_nReverbEffectType];
	}

	RealSoundChannel* RealSound2Mgr::PlayMusic(const string& strFileName, FMOD_MODE mode)
	{
		RFMODSound* pSound = NULL;
		RealSoundChannel* channel = 0;

		if (IsMute()) return channel;

		pSound = m_pFMODSoundMgr->GetFMODMusic(strFileName, mode);
		if (pSound == NULL) return NULL;

		//채널만 생성
		channel = m_pRealSound->PlaySound(pSound->GetSound(), 1.0f);

		if (channel)
		{
			channel->setUserData(static_cast<void*>(pSound));
			SetCallback(channel);
		}

		//m_FMODSoundMap.insert(FMODSoundMap::value_type(strFileName, pSound));

		AddChannelToMusicChannelGroup(channel);

		return channel;
	}

	void RealSound2Mgr::SetBGMMute( bool bMute )
	{
		m_bBGMMute = bMute;

		m_MusicChannelGroup->setMute(m_bBGMMute);
	}

	void RealSound2Mgr::SetEffectMute( bool bMute )
	{
		m_bEffectMute = bMute;

		m_EffectChannelGroup->setMute(m_bEffectMute);
	}

	void RealSound2Mgr::SetMute( bool bMute )
	{
		m_bMute = bMute;

		m_pRealSound->SetMute(m_bMute);
	}

	void RealSound2Mgr::SetEffectVolume( float fVolume )
	{
		m_fEffectVolume = fVolume;
		m_fEffectVolume = max(0, min(1.0f, m_fEffectVolume));

		m_EffectChannelGroup->setVolume(m_fEffectVolume * m_fMasterVolume);
	}

	void RealSound2Mgr::SetMusicVolume( float fVolume )
	{
		m_fMusicVolume = fVolume;
		m_fMusicVolume = max(0, min(1.0f, m_fMusicVolume));

		m_MusicChannelGroup->setVolume(m_fMusicVolume * m_fMasterVolume);
	}

	// TODO : 사운드와 뮤직 채널을 만들어야 할까
	void RealSound2Mgr::SetMasterVolume( float fVolume )
	{
		m_fMasterVolume = fVolume;
		m_fMasterVolume = max(0, min(1.0f, m_fMasterVolume));

		m_pRealSound->SetMasterVolume(m_fMasterVolume);

		m_EffectChannelGroup->setVolume(m_fEffectVolume * m_fMasterVolume);
		m_MusicChannelGroup->setVolume(m_fMusicVolume * m_fMasterVolume);
	}

	void RealSound2Mgr::StopSound()
	{
		if(m_pRealSound)
			m_pRealSound->Stop();
	}

	int RealSound2Mgr::GetChannelsPlaying()
	{
		int          channelsplaying = 0;
		m_pFMODSystem->getChannelsPlaying(&channelsplaying);

		return channelsplaying;
	}

	void RealSound2Mgr::SetHardwareAccelerated( bool bEnable)
	{
		m_bHardwareAccelerated = bEnable;
	}

	bool RealSound2Mgr::GetHardwareAccelerated()
	{
		return m_bHardwareAccelerated;
	}

	void RealSound2Mgr::SetRollOffType( int nType)
	{
		m_nRollOffType = nType;
	}

	int RealSound2Mgr::GetRollOffType()
	{
		return m_nReverbEffectType;
	}

	float RealSound2Mgr::GetCPUUsage()
	{
		float dsp;
		float stream;
		float update;
		float total;

		m_pFMODSystem->getCPUUsage(&dsp, &stream, NULL, &update, &total);

		return total;
	}

	int RealSound2Mgr::GetUsageMemory()
	{
		int currentalloced = 0;
		int maxalloced = 0;

		FMOD::Memory_GetStats(&currentalloced, &maxalloced);

		return currentalloced;
	}

	void RealSound2Mgr::SetPan( float fPan )
	{
		m_pRealSound->SetPan(fPan);
	}

	void RealSound2Mgr::SetListener( const float* pos, const float* vel, const float* forward, const float* up )
	{
		m_pRealSound->SetListener((pos) ? TransFromCoord(pos) : NULL, (vel) ? TransFromCoord(vel) : NULL
			, (forward) ? TransFromCoord(forward) : NULL, (up) ? TransFromCoord(up) : NULL);
	}

	void RealSound2Mgr::Set3DSetting( float dopplerscale, float distancefactor, float rolloffscale )
	{
		m_pRealSound->Set3DSetting(dopplerscale, distancefactor, rolloffscale);
	}

	void RealSound2Mgr::Get3DSetting( float* dopplerscale, float* distancefactor, float* rolloffscale )
	{
		m_pRealSound->Get3DSetting(dopplerscale, distancefactor, rolloffscale);
	}

	int RealSound2Mgr::GetLoadedFMODSoundCount()
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// 채널 설정 함수들

	void RealSound2Mgr::SetChannel3DAttributes(RealSoundChannel* channel, const vec3 pos, const vec3 vel)
	{
		m_Result = channel->set3DAttributes((FMOD_VECTOR*)&TransFromCoord(pos), (FMOD_VECTOR*)&TransFromCoord(vel));
		ERRCHECK(m_Result);
	}

	void RealSound2Mgr::Set3DMinMaxDistance(RealSoundChannel* channel, float fMinDistance, float fMaxDistance)
	{
		m_Result = channel->set3DMinMaxDistance(fMinDistance, fMaxDistance);
		ERRCHECK(m_Result);
	}

	void RealSound2Mgr::SetPriority(RealSoundChannel* channel, int nPriority)
	{
		m_Result = channel->setPriority(nPriority);
		ERRCHECK(m_Result);
	}

	void RealSound2Mgr::SetCallback(RealSoundChannel* channel)
	{
		channel->setCallback(RealSound2Mgr::CHANNEL_END_CALLBACK);
	}

	//실제 사운드가 설정된 채널을 플레이한다.
	void RealSound2Mgr::SetChannelPause(RealSoundChannel* channel, bool bPause)
	{
		if (channel == NULL) return;

		m_Result = channel->setMute(m_bMute);
		ERRCHECK(m_Result);

		if (m_bMute == true && bPause == false)	return;

		//실제 플레이
		m_Result = channel->setPaused(bPause);
		ERRCHECK(m_Result);
	}

	void RealSound2Mgr::SetChannelVolume( RealSoundChannel* channel, float volume )
	{
		if (channel == NULL) return;

		m_Result = channel->setVolume(volume);
		ERRCHECK(m_Result);
	}

	bool RealSound2Mgr::Check3DSoundPlayable( RFMODSound* pSound, vec3 pos, float fMinDistance, float fMaxDistance )
	{
		if (pSound->GetSound() == NULL) return false;

		unsigned int lenms = 0;
		m_Result = pSound->GetSound()->getLength(&lenms, FMOD_TIMEUNIT_MS);
		if (m_Result == FMOD_OK)
		{
			if (lenms < 1000)
			{
				FMOD_VECTOR listener_pos;
				if (m_pFMODSystem->get3DListenerAttributes(0, &listener_pos, NULL, NULL, NULL) == FMOD_OK)
				{
					vec3 vListenerPos;
					vListenerPos.x = listener_pos.x;
					vListenerPos.y = listener_pos.z;
					vListenerPos.z = listener_pos.y;

					float fDistance = vListenerPos.DistanceTo(pos);
					if (fDistance > (fMinDistance + (fMaxDistance - fMinDistance) * 0.5f))
					{
						return false;
					}
				}
			}
		}

		return true;
	}
}
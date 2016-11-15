#pragma once

#include <map>
#include "fmod.hpp"
#include "RFMODSoundMgr.h"
#include "RealSoundFMODEX.h"


enum
{
	RS_LOG_ROLLOFF			= 0,
	RS_LINEAR_ROLLOFF
};



namespace RS2
{
	class RealSoundFMODEX;

	class RealSound2Mgr
	{
	protected:
		RealSoundFMODEX*m_pRealSound;

		FMOD::System*	m_pFMODSystem;
		FMOD_RESULT		m_Result;

		RFMODSoundMgr*		m_pFMODSoundMgr;

		FMOD::ChannelGroup*	m_MusicChannelGroup;
		FMOD::ChannelGroup*	m_EffectChannelGroup;

		int				m_nReverbEffectType;

		bool			m_bMute;			///< ¹ÂÆ®
		bool			m_bBGMMute;
		bool			m_bEffectMute;

		float			m_fEffectVolume;
		float			m_fMusicVolume;		///< ¹ÂÁ÷ º¼·ý
		float			m_fMasterVolume;	///< ¸¶½ºÅÍ º¼·ý

		bool			m_bHardwareAccelerated;
		int				m_nRollOffType;

		void			CreateMusicChannelGroup();
		void			CreateEffectChannelGroup();

		void			AddChannelToMusicChannelGroup(RealSoundChannel* channel);
		void			AddChannelToEffectChannelGroup(RealSoundChannel* channel);

		void			Update();
		void			IsGreatThenBuffersize();

		static FMOD_RESULT F_CALLBACK RealSound2Mgr::CHANNEL_END_CALLBACK( FMOD_CHANNEL* channel, FMOD_CHANNEL_CALLBACKTYPE type, void* commanddata1, void* commanddata2 );

		void			Set3DMinMaxDistance(RealSoundChannel* channel, float fMinDistance, float fMaxDistance);
		void			SetPriority(RealSoundChannel* channel, int nPriority);
		void			SetCallback(RealSoundChannel* channel);
		void			SetChannelVolume( RealSoundChannel* channel, float volume );
		bool			Check3DSoundPlayable(RFMODSound* pSound, vec3 pos, float fMinDistance, float fMaxDistance);
	public:

		RealSound2Mgr(void);
		virtual ~RealSound2Mgr(void);

		bool			CreateRealSound2();

		RealSoundChannel* PlaySound( const string& strFileName, FMOD_MODE mode );
		RealSoundChannel*	PlayMusic(const string& strFileName, FMOD_MODE mode);

		RealSoundChannel*	Play3DSound(const string& strFileName, FMOD_MODE mode, float fVolume, vec3 pos, vec3 vel, float fDistMin=1.0f, float fDistMax=10000.0f, int nPriority=128);
		void			StopSound();

		void			SetMute(bool bMute);
		bool			IsMute()	{ return m_bMute;}

		void			SetBGMMute(bool bMute);
		void			SetEffectMute( bool bMute );

		void			SetEffectVolume(float fVolume);
		float			GetSoundVolume()	{ return m_fEffectVolume;}

		void			SetMusicVolume(float fVolume);
		float			GetMusicVolume()	{ return m_fMusicVolume;}

		void			SetMasterVolume(float fVolume);
		float			GetMasterVolume()	{ return m_fMasterVolume;}

		void			SetPan(float fPan);
		void			Set3DSetting(float dopplerscale, float distancefactor, float rolloffscale);
		void			Get3DSetting(float* dopplerscale, float* distancefactor, float* rolloffscale);

		void			SetListener(const float* pos, const float* vel, const float* forward, const float* up);
		void			SetReverbEffectType( int effecttype );
		const char*		GetReverbEffectType();

		void			SetChannelPause(RealSoundChannel* channel, bool bPause);
		//channel
		void			SetChannel3DAttributes(RealSoundChannel* channel, const vec3 pos, const vec3 vel);

		int				GetChannelsPlaying();

		FMOD::System *	GetFMODSystem() { return m_pRealSound->GetFMODSystem(); }

		void			SetHardwareAccelerated( bool bEnable);
		bool			GetHardwareAccelerated();

		void			SetRollOffType( int nType);
		int				GetRollOffType();

		//Debug
		float			GetCPUUsage();
		int				GetUsageMemory();
		int				GetLoadedFMODSoundCount();
	};
}//namespace RS2

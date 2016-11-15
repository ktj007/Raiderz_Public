#pragma once

#include <map>
#include "fmod.hpp"
#include "MXml.h"

using namespace std;

namespace RS2
{
	class RFMODSound
	{
	protected:
		FMOD::Sound*		m_pSound;		///< 
		DWORD				m_dReadTime;	///< 억세스 타임
		int					m_nRefCount;	///< 
		char*				m_pStreamBuff;	///< Stream용 버퍼

	public:
		RFMODSound::RFMODSound();
		~RFMODSound(void);

		bool			CreateSound(FMOD::System* pFMODSystem, string szKey, FMOD_MODE mode);
		bool			CreateMusic(FMOD::System* pFMODSystem, string szKey, FMOD_MODE mode);
		void			ReleaseSound();

		void			Set3DMinMaxDistance(float fMinDistance, float fMaxDistance);
		FMOD::Sound*	GetSound() const { return m_pSound; }
		unsigned int	GetSoundLength();

		void			SetAccessTime(DWORD time) { m_dReadTime = time; }
		DWORD			GetAccessTime()		{ return m_dReadTime; }

		void			refAdd()	{ ++m_nRefCount; }
		void			refSub()
		{
			--m_nRefCount; _ASSERT(m_nRefCount >= 0);
			if (m_nRefCount < 0) m_nRefCount = 0;
		}

		//debug
		int				GetrefCount()	{ return m_nRefCount;}
	};

	class RFMODSoundMgr : public map<std::string, RFMODSound*>
	{
	protected:
		FMOD::System*	m_pFMODSystem;		///< 참조 주소

		void			Insert( string szKey, RFMODSound* pFMODSound );

	public:
		RFMODSoundMgr(FMOD::System* pSystem);
		virtual ~RFMODSoundMgr(void);

		virtual RFMODSound*		GetFMODSound(string szKey, FMOD_MODE mode, float fDistMin=1.0f, float fDistMax=10000.0f);
		RFMODSound*				GetFMODMusic(string szKey, FMOD_MODE mode);

		void			UpdateReleaseSound();

		// Debug
		int				GetLoadedFMODSoundCount();
		RFMODSound*		GetFMODSoundOldAccessTime();	///< 플레이중이 아닌 오래된 사운드 찾기
	};
}

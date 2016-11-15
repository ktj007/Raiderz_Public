#pragma once

#include "fmod.hpp"

namespace RS2
{
	class RealSoundChannel : public FMOD::Channel
	{
	public:
		bool IsPlaying()
		{
			bool bPlaying = false;
			isPlaying(&bPlaying);
			return bPlaying;
		}
	};

	class RealSoundFMODEX
	{
	private:
		FMOD::System		*m_pFMODSystem;
		FMOD_RESULT			result;

		float				m_fDopplerfactor;
		float				m_fRollfactor;
		float				m_fDistfactor;

	public:
		RealSoundFMODEX(void);
		virtual ~RealSoundFMODEX(void);

		bool			InitFMOD();
		void			FinalFMOD();
		void			Update();

		RealSoundChannel*	PlaySound( FMOD::Sound *sound );
		RealSoundChannel*	PlaySound( FMOD::Sound *sound, float fVolume );
		void			Stop();

		FMOD::System *	GetFMODSystem() const { return m_pFMODSystem; }

		// Volume
		void			SetMute(bool bMute);
		void			SetMasterVolume(float fVolume);

		// 2D
		// A left/right pan level, from -1.0 to 1.0 inclusive. -1.0 = Full left, 0.0 = center, 1.0 = full right. Default = 0.0.
		void			SetPan(float pan);

		// 3D
		//void			SetDopplerFactor(float factor = 1.0f);
		//void			SetDistanceFactor(float factor = 1.0f);
		//void			SetRollFactor(float factor = 1.0f);
		void			Set3DSetting(float dopplerscale, float distancefactor, float rolloffscale);
		void			Get3DSetting(float* dopplerscale, float* distancefactor, float* rolloffscale);
		void			SetListener( const float* pos, const float* vel, const float* forward, const float* up );
		//virtual void SetMinMaxDistance(RBaseSoundSource* pSoundSource, float min, float max);
		//virtual void SetDefaultVolume(RBaseSoundSource* pSoundSource, float fDefaultVolume);

		//virtual void SetListener(const float* pos, const float* vel, const float* forward, const float* top);
		//virtual void Update();

		//2D
		//void SetPan(int iChannel, float Pan);

		//// Info
		//virtual unsigned int GetNumDriver() const;
		//virtual const char* GetDriverName(int id=DEFAULT_SOUND_DRIVER_ID);
		//virtual unsigned int GetDriverCaps(int id=DEFAULT_SOUND_DRIVER_ID);


		//// Music
		//virtual bool OpenMusic(const char* szFileName);
		//virtual bool OpenMusic(void* pData, int nLength);
		//virtual void PlayMusic(bool bLoop);
		//virtual void StopMusic();
		//virtual void CloseMusic();

		//// volume
		//virtual void SetMusicVolume(float fVolume);
		//virtual void SetMusicMute(bool m);
		//virtual void SetVolume(float fVolume);
		//virtual void SetVolume(int nChannel, float fVolume);
		//virtual void SetMute(bool m);
		//virtual void SetMute(int nChannel, bool m);
		//virtual float GetVolume() { return m_fVolume; }
		//virtual float GetMusicVolume()	{ return m_fMusicVolume; }
	};
}

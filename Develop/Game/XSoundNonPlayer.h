#pragma once

class XSoundNonPlayer
{
protected:
	bool				m_bInteraction;


public:
	XSoundNonPlayer();

	void PlayVoice( const MUID& uID, const char* szType =NULL);


	static XSoundNonPlayer* GetInstance();
};





// inline : XGetSoundNonPlayer
inline XSoundNonPlayer* XGetSoundNonPlayer()
{
	return XSoundNonPlayer::GetInstance();
}

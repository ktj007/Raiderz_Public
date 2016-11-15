#ifndef _TFRAME_SLIDER_CONTROLLER_H
#define _TFRAME_SLIDER_CONTROLLER_H

#pragma once

enum ANIMATION_PLAY_STATE
{
	APS_STOP,
	APS_PLAY,
	APS_PAUSE
};

class TFrameSliderController
{
private:
	ANIMATION_PLAY_STATE		m_eAnimationPlayState;

private:
	bool			CheckCustomMotion(TPlayMotion* pPlayMotion);

	void			Update_PlayStopButton(int nCurFrame, int nMaxFrame);

	void			ShowTalentAnimationName(TCharacter* pActor, TTalentAnimation* pTalentAnimation);
	void			ShowNormalAnimationName(TCharacter* pActor);
	void			ShowTalentHitCapsule(float fTime);

	System::Drawing::SolidBrush^ GetBrush(int n);

public:
	TFrameSliderController();
	virtual ~TFrameSliderController();

	void			Update_TalentFrame(TCharacter* pActor, TTalentAnimation* pTalentAnimation);
	void			Update_AniFrame(TCharacter* pActor);

	void			FrameSliderScroll(TCharacter* pActor, TTalentAnimation* pTalentAnimation, int nMax, int nValue);

	void			pressPlayButton();
	void			pressStopButton();
	void			pressPauseButton();

	ANIMATION_PLAY_STATE GetAnimationState() { return m_eAnimationPlayState; }
};


#endif
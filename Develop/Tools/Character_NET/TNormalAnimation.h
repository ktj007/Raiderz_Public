#ifndef _TNORMAL_ANIMATION_H
#define _TNORMAL_ANIMATION_H

#pragma once

using namespace System;
using namespace System::Windows::Forms;

class TNormalAnimation
{
private:
	TCharacter*			m_pActor;
	TPlayMotion*		m_pMotion;

	vector<RMatrix>		m_vecDummyLocLast;

	bool				m_bUseAnimationLinkListTest;
	CHECK_DUMMY_LOC_RESULT m_checkDummyLocResult;

private:
	void				CheckAnimationLinkListTest();
	void				CheckDummyLoc(string strAniName);
	void				CheckAnimationState();

public:
	TNormalAnimation();
	virtual ~TNormalAnimation();

	void				Init(TPlayMotion* pMotion);
	void				Update(float fDelta);

	void				SetActor(TCharacter* pActor) { m_pActor = pActor; }
	bool				SetAnimation(string strAniName);

	bool				AnimationPlay(bool bPrePause, bool bPlayDone);
	void				AnimationStop();
	void				AnimationPause();
	void				AnimationLinkListTestPlay(ListBox^ AnimationLinkListr);
	void				AnimationLinkListTestStop();
	void				AnimationLinkListTestSpeed(float fSpeed);
	void				AnimationLinkListTestEndTime(int nIndex, float fEndTime);
	void				UpperAnimationTestPlay(string strUpperAni, string strLowerAni, TCharacter* pActor);
	void				UpperAnimationTestStop();

	bool				CalAnimaitonPos();
	void				CheckAnimationEvent();
	void				CheckAnimationEvent(int nFrame);

	CHECK_DUMMY_LOC_RESULT GetDummyLocResult() { return m_checkDummyLocResult; }
	bool				GetUseAnimationLinkListTest() { return m_bUseAnimationLinkListTest; }

	void				InitNormalAnimation();
};


#endif
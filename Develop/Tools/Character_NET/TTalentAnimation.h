#ifndef _TTALENT_ANIMATION_H
#define _TTALENT_ANIMATION_H

#pragma once

#include "TTalentEvent.h"
#include "TTalentEffectHitCapsule.h"
#include "TAnimationHelper.h"
#include "TTalentHitEffectDirTest.h"

class TTalentAnimation
{
	enum TalentAnimationState
	{
		TAS_STOP,
		TAS_PLAY,
		TAS_PAUSE
	};

private:
	TCharacter*			m_pActor;
	TPlayMotion*		m_pMotion;
	TTalentEvent		m_TalentEvent;
	TTalentInfo*		m_pTalentInfo;
	TTalentHitEffectDirTest	m_TalentHitEffectDirTest;

	TalentAnimationState	m_nTalentAnimationState;

	bool				m_bCasting;
	bool				m_bCastingLoop;
	bool				m_bUseAct;
	bool				m_bShowHitEffect;

	int					m_nUseActAniIndex;
	int					m_nCurAniIndex;

	int					m_nCurrentTalentID;
	int					m_nCurrentTalentMode;

	float				m_fPostDelayRemainTime;

	vec3				m_vDummyLocPos;
	vec3				m_vDummyLocDir;

	vector<RMatrix>		m_vecDummyLocLast;

	vector<EFFECT_RESULT_DATA> m_vecTalentEffectResult;

	TTalentEffectHitCapsule m_TalentEffectHitCapsule;
	CHECK_DUMMY_LOC_RESULT m_checkDummyLocResult;

private:
	bool				Play();
	bool				PlayTalentAnimation();
	void				Stop();

	void				CheckTalentAnimation(float fDelta);
	void				CheckDummyLoc();
	bool				CheckTalentNomalStance();
	bool				CheckExistTalentAnimation(TTalentInfo* pTalentInfo);
	bool				CheckExistTalentAnimationByAniType(TTalentInfo* pTalentInfo, TALENT_ANIMATION_TYPE eAniType, WEAPON_TYPE eWeaponType, bool bAutoPrefix);
	bool				CheckTalentAnimationEnd();

	void				ShowTalentEffect(TALENT_EFFECT_TYPE nType);
	void				ShowTalentHitEffect(XTalentEffectDataMgr* pEffectMgr, vector<XTalentEffectInfo *>* pvecEffectInfo);
	void				ShowTalentHitEffectPosBox();

	void				DelTalentEffect(bool bImmediatlyDelete = false);

	int					GetAniCount(string strMotionName);

	void				InitTalentEvent();
	void				InitDummyLocPos();

public:
	TTalentAnimation();
	virtual ~TTalentAnimation();

	void				Init(TPlayMotion* pMotion);
	void				Update(float fDelta);
	void				Render();
	void				InitTalentAnimation();
	void				Reload();

	void				SetActor(TCharacter* pActor) { m_pActor = pActor; }
	bool				SetTalentAnimation(CSTalentInfo* pTalentInfo, int nMode);
	void				SetShowHitEffect(bool bShow) { m_bShowHitEffect = bShow; }
	bool				CalAnimaitonPos();

	bool				TalentAnimationPlay(bool bPrePause = false);
	void				TalentAnimationPause();
	void				TalentAnimationStop();
	void				TalentAnimationEnd();

	int					GetUseActAniIndex() { return m_nUseActAniIndex; }
	CHECK_DUMMY_LOC_RESULT GetDummyLocResult() { return m_checkDummyLocResult; }
	int					GetTalentID() { return m_nCurrentTalentID; }
	int					GetTalentMode() { return m_nCurrentTalentMode; }
	vector<RMatrix>*	GetDummyLocLast() { return &m_vecDummyLocLast; }

	int					GetCurTalentFrameMax();
	int					GetCurTalentCurFrame();
	int					GetTalentAnimationMaxIndex();
	int					GetTalentFrame(int nAniIndex);
	string				GetTalentAniName(int nAniIndex);

	float				GetCurTalentCurFrametime();
	float				GetCurTalentMaxFrametime();

	void				SetTalentAnimationFrame(int frame);
};


#endif
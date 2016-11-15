#ifndef _XSCREENEFFECTMANAGER_H
#define _XSCREENEFFECTMANAGER_H

#include "RScreenEffectManager.h"

enum TARGET_UI_STATE
{
	TUS_NOT = -1,
	TUS_START,
	TUS_END,
	TUS_AIM,
	TUS_LOCK
};

#define TARGET_UI_NAME						L"GunTarget"
#define TARGET_UI_EFFECT_NAME				L"crosshair"
#define TARGET_UI_STATE_START				L"target_on"
#define TARGET_UI_STATE_END					L"target_off"
#define TARGET_UI_STATE_LOCK				L"target_lock"
#define TARGET_UI_STATE_LOCK_END			L"target_lock_off"

typedef RSCREENEFFECT_EXIST_ATT				XSCREENEFFECT_EXIST_ATT;
typedef RSCREENEFFECT_CHARACTER_ATT			XSCREENEFFECT_CHARACTER_ATT;
typedef RSCREENEFFECT_LOCATION_TYPE			XSCREENEFFECT_LOCATION_TYPE;
typedef RSCREENEFFECT_ANIMATION_END_ATT		XSCREENEFFECT_ANIMATION_END_ATT;

typedef map<wstring, RScreenEffectManager*>  ScreenEfeectList;
typedef ScreenEfeectList::iterator			ScreenEfeectList_Iter;
typedef vector<RScreenEffectManager*>		multiScreenEfeectList;
typedef multiScreenEfeectList::iterator		multiScreenEfeectList_Iter;

struct XSCREENEFFECTAniInfo
{
	wstring									strAniName;
	XSCREENEFFECT_ANIMATION_END_ATT			nAniEndAtt;
};

class XScreenEffectManager : public RScreenEffectManager
{
private:
	TARGET_UI_STATE				m_nTargetUIState;
	bool						m_bTargetUIStart;
	bool						m_bTargetLock;

public:
	XScreenEffectManager();
	~XScreenEffectManager();

	void						Create(RSceneManager *pManager);
	void						Destroy();

	void						LoadScreenEffect();
	void						AddScreenEffect(wstring strListFileName, 
												XSCREENEFFECT_CHARACTER_ATT charAtt = CHARACTER_NORMAL, 
												AnimationLoopType aniLoop = RAniLoopType_HoldLastFrame);
	void						RegisterScreenEffect(wstring strScreenEffectName, 
													wstring strListFileName, 
													XSCREENEFFECT_CHARACTER_ATT charAtt = CHARACTER_NORMAL,
													AnimationLoopType aniLoop = RAniLoopType_HoldLastFrame);

	// 문자열 입력
	void						SetString(wstring strScreenEffectName, wstring strContents);
	// 위치 셋팅
	void						SetPosition(wstring strScreenEffectName, vec3 vPosEffect, XSCREENEFFECT_LOCATION_TYPE scrBase = LOCATION_LEFT);
	// 애니메이션 변경
	void						ChangeAnimation(wstring strScreenEffectName, wstring strAnimation, XSCREENEFFECT_ANIMATION_END_ATT aniDel = ANIMATION_END_DEL);
	// 그리기 시작
	void						SetDraw(wstring strScreenEffectName, bool bVisible);
	
	virtual void				Update();
	virtual void				Render();

	// 애니메이션 정지
	void						AnimationStop(wstring strScreenEffectName);

	//------------------------------------------------------------------------
	// 타겟 설정
	void						ResisterTarget();
	void						SetTarget(TARGET_UI_STATE nState, int x = 0, int y = 0);

	void						ResisterVictory();
	void						SetVictory(TARGET_UI_STATE nState, int x = 0, int y = 0);
};

#endif

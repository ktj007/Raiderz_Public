#ifndef _TTALENT_EFFECT_VIEWER_H
#define _TTALENT_EFFECT_VIEWER_H

#include <Windows.h>
#include <MMSystem.h>
#include <shlwapi.h>
#include <math.h>
#include <algorithm>

#include "MainApp.h"
#include "XTalentInfo.h"
#include "XTalentEffectInfo.h"

using namespace System;
using namespace System::Windows::Forms;

class TTalentEffectMgr;

#define EFFECTYPE_STRING_CAST_EFFECT						"캐스트"
#define EFFECTYPE_STRING_USE_EFFECT							"사용"
#define EFFECTYPE_STRING_ATTACK_EFFECT						"피격"
#define EFFECTYPE_STRING_MISS_EFFECT						"실패"
#define EFFECTYPE_STRING_SHOCK_EFFECT						"충격"
#define EFFECTYPE_STRING_DURATION_EFFECT					"지속"
#define EFFECTYPE_STRING_DEFENSE_EFFECT						"방어"
#define EFFECTYPE_STRING_CAST_END_EFFECT					"캐스트 종료"
#define EFFECTYPE_STRING_NOT								"선택바람"

#define IMPACTPOSITION_STRING_HIT_POS						"맞은 위치"
#define IMPACTPOSITION_STRING_BONE_POS						"본 위치"
#define IMPACTPOSITION_STRING_SHOCK_GROUND_POS				"지형 위치(영역)"
#define IMPACTPOSITION_STRING_TALENT_HIT_CAPSUL_POS			"판정 위치(사용)"
#define IMPACTPOSITION_STRING_MODEL_POS						"모델 위치"

#define EFFECT_DIRECTION_NONE								"방향성 없음"
#define EFFECT_DIRECTION_FRONT_USER							"이팩트사용자 정면"
#define EFFECT_DIRECTION_SWORD_DIR							"칼 방향"

class TTalentEffectViewer
{
private:
	TTalentEffectMgr *	m_pEffectMgr;

	TALENT_EFFECT_TYPE	m_eTalentEffectType;


private:
	PropertyGrid^		GetEffectViewerPropertGrid();

	// 문자열 입력
	void				SetTextEffectType(string& strBuffer, TALENT_EFFECT_TYPE nEffectType);
	void				SetTextImpactPositionList(string& strBuffer, IMPACTPOS nImapctPos);

	TALENT_EFFECT_TYPE	GetEffectType(string& strBuffer);
	IMPACTPOS			GetEffectImpactPositionList(string& strBuffer);

	// 캐스트
	bool				SetTalentEffectCastToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectCast(XTalentEffectInfo * pTalentEffectInfo);
	// 사용
	bool				SetTalentEffectUseToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectUse(XTalentEffectInfo * pTalentEffectInfo);
	// 피격
	bool				SetTalentEffectAttackToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectAttack(XTalentEffectInfo * pTalentEffectInfo);
	// 실패
	bool				SetTalentEffectMissToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectMiss(XTalentEffectInfo * pTalentEffectInfo);
	// 충격
	bool				SetTalentEffectShockToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectShock(XTalentEffectInfo * pTalentEffectInfo);
	// 지속
	bool				SetTalentEffectDurationToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectDuration(XTalentEffectInfo * pTalentEffectInfo);
	// 방어
	bool				SetTalentEffectDefenseToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectDefense(XTalentEffectInfo * pTalentEffectInfo);
	// 캐스트 종료
	bool				SetTalentEffectCastEndToProperty(XTalentEffectInfo * pTalentEffectInfo);
	bool				SetPropertyToTalentEffectCastEnd(XTalentEffectInfo * pTalentEffectInfo);


	bool	(TTalentEffectViewer::*TalentEffectFuncToProperty[8])(XTalentEffectInfo * pTalentEffectInfo);
	bool	(TTalentEffectViewer::*TalentEffectFuncToTalentEffectInfo[8])(XTalentEffectInfo * pTalentEffectInfo);
	//------------------------------------------------------------------------

public:
	TTalentEffectViewer();
	virtual ~TTalentEffectViewer() {}

	void				SetEffectManager(TTalentEffectMgr * pMgr)		{ m_pEffectMgr = pMgr; }

	TALENT_EFFECT_TYPE	GetCurrentEffectType(){ return m_eTalentEffectType; }

	//------------------------------------------------------------------------
	// 탤런트 이펙트 속성창
	void				SetEffectInfoFromPropertyGrid(XTalentEffectInfo* pEffectInfo);
	void				SetPropertyGridFromEffectInfo(XTalentEffectInfo* pEffectInfo);

	//------------------------------------------------------------------------
	// 히트 이펙트 위치 방향 속성창
	void				SetHitEffectPosDirInfoFromPropertyGrid(XTalentHitEffectPosDir* pHitEffectPosDirInfo);
	void				SetPropertyGridFromHitEffectPosDirInfo(XTalentHitEffectPosDir* pHitEffectPosDirInfo);
};


#endif
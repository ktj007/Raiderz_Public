#ifndef _TTALENT_EVENT_VIEWER_H
#define _TTALENT_EVENT_VIEWER_H

#include <Windows.h>
#include <MMSystem.h>
#include <shlwapi.h>
#include <math.h>
#include <algorithm>

#include "MainApp.h"

#define TALENT_EVENT_NORMAL		"Normal"
#define TALENT_EVENT_ACT		"Act"

using namespace System;
using namespace System::Windows::Forms;

class TTalentEventMgr;

class TTalentEventViewer
{
private:
	TTalentEventMgr *	m_pEventMgr;
	
	CSTalentInfo*		m_pSelectedTalentInfo;
private:
	PropertyGrid^		GetEventViewerPropertGrid();
	TreeView^			GetEventViewerTreeNode();

	TALENT_EVENT		GetDataEventType(char* pchrBuffer);
	TALENT_PROJECTILE_TYPE		GetDataProjectileType(string strBuffer);
	TALENT_PROJECTILE_VISUAL_TYPE GetDataProjectileVisualType(string strBuffer);

public:
	TTalentEventViewer();
	virtual ~TTalentEventViewer() {}

	TreeNode^			AddTalentEventView(int nStart);
	bool				DeleteTalentEvent();

	void				NotifyChanging();
	void				SetEventManager(TTalentEventMgr * pMgr)		{ m_pEventMgr = pMgr; }

	CSTalentInfo *		GetCurrentTalentInfo();
	CSTalentEventInfo * GetCurrentTalentEventInfo();

	void				SelTalentInfo(CSTalentInfo* pTalentInfo) { m_pSelectedTalentInfo = pTalentInfo; }
	CSTalentInfo*		GetSelTalentInfo() const { return m_pSelectedTalentInfo; }

	string				GetTextEventType(TALENT_EVENT nEventType);		// 문자열 입력
	string				GetTextProjectileType(TALENT_PROJECTILE_TYPE nProjectileType);		// 문자열 입력
	string				GetTextProjectileVisualType(TALENT_PROJECTILE_VISUAL_TYPE nProjeceilVisualType);

	void				ChangeEventTreeNodeName(string strEventTypeName, TreeNode^ tn = nullptr);

	void				GetTalentHitInfoSegmentIndex(ArrayList^ indexList);
	void				GetTalentHitInfoCapsuleIndex(ArrayList^ indexList, TALENT_EVENT nEventType);
	void				GetProjectileVisualType(ArrayList^ typeList);
	
	void				GetTalentHitInfoSegmentIndexAndCapsuleIndex(int& nSegIndex, int& nCapsuleIndex);

	//------------------------------------------------------------------------
	// 탤런트 이벤트 속성창
	void				SetCurrentPropertyTalentEvent(bool bNewData);

	// 타입 없음
	bool				SetTalentEventInfoToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventInfo(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 발사
	bool				SetTalentEventFireToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventFire(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 발사체
	bool				SetTalentEventProjectileToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventProjectile(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 이펙트
	bool				SetTalentEventEffectToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventEffect(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 사운드
	bool				SetTalentEventSoundToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventSound(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 카메라
	bool				SetTalentEventCameraToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventCamera(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 데미지
	bool				SetTalentEventDamageToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventDamage(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// custom
	bool				SetTalentEventCustomToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventCustom(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 시간차
	bool				SetTalentEventDelayedActToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventDelayedAct(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// extra_active_1
	bool				SetTalentEventExtraActive_1ToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventExtraActive_1(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// extra_active_2
	bool				SetTalentEventExtraActive_2ToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventExtraActive_2(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 카메라 락
	bool				SetTalentEventCameraLockToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventCameraLock(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	// 지형 발사
	bool				SetTalentEventGroundFireToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool				SetPropertyToTalentEventGroundFire(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);

	bool	(TTalentEventViewer::*TalentEventFuncToProperty[13])(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	bool	(TTalentEventViewer::*TalentEventFuncToTalentEventInfo[13])(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo);
	//------------------------------------------------------------------------
};




#endif
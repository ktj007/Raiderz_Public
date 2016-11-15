#include "stdafx.h"
#include "TTalentEventViewer.h"
#include "TTalentEventMgr.h"
#include "Xmlseral.h"

//////////////////////////////////////////////////////////////////////////
TTalentEventViewer::TTalentEventViewer()
{
	m_pSelectedTalentInfo = NULL;
	m_pEventMgr								= NULL;

	
	// 탤런트 이벤트 함수 포인터
	TalentEventFuncToProperty[0]			= &TTalentEventViewer::SetTalentEventInfoToProperty;
	TalentEventFuncToTalentEventInfo[0]		= &TTalentEventViewer::SetPropertyToTalentEventInfo;

	TalentEventFuncToProperty[1]			= &TTalentEventViewer::SetTalentEventFireToProperty;
	TalentEventFuncToTalentEventInfo[1]		= &TTalentEventViewer::SetPropertyToTalentEventFire;

	TalentEventFuncToProperty[2]			= &TTalentEventViewer::SetTalentEventProjectileToProperty;
	TalentEventFuncToTalentEventInfo[2]		= &TTalentEventViewer::SetPropertyToTalentEventProjectile;

	TalentEventFuncToProperty[3]			= &TTalentEventViewer::SetTalentEventEffectToProperty;
	TalentEventFuncToTalentEventInfo[3]		= &TTalentEventViewer::SetPropertyToTalentEventEffect;

	TalentEventFuncToProperty[4]			= &TTalentEventViewer::SetTalentEventSoundToProperty;
	TalentEventFuncToTalentEventInfo[4]		= &TTalentEventViewer::SetPropertyToTalentEventSound;

	TalentEventFuncToProperty[5]			= &TTalentEventViewer::SetTalentEventCameraToProperty;
	TalentEventFuncToTalentEventInfo[5]		= &TTalentEventViewer::SetPropertyToTalentEventCamera;

	TalentEventFuncToProperty[6]			= &TTalentEventViewer::SetTalentEventDamageToProperty;
	TalentEventFuncToTalentEventInfo[6]		= &TTalentEventViewer::SetPropertyToTalentEventDamage;

	TalentEventFuncToProperty[7]			= &TTalentEventViewer::SetTalentEventCustomToProperty;
	TalentEventFuncToTalentEventInfo[7]		= &TTalentEventViewer::SetPropertyToTalentEventCustom;

	TalentEventFuncToProperty[8]			= &TTalentEventViewer::SetTalentEventDelayedActToProperty;
	TalentEventFuncToTalentEventInfo[8]		= &TTalentEventViewer::SetPropertyToTalentEventDelayedAct;

	TalentEventFuncToProperty[9]			= &TTalentEventViewer::SetTalentEventExtraActive_1ToProperty;
	TalentEventFuncToTalentEventInfo[9]		= &TTalentEventViewer::SetPropertyToTalentEventExtraActive_1;

	TalentEventFuncToProperty[10]			= &TTalentEventViewer::SetTalentEventExtraActive_2ToProperty;
	TalentEventFuncToTalentEventInfo[10]	= &TTalentEventViewer::SetPropertyToTalentEventExtraActive_2;

	TalentEventFuncToProperty[11]			= &TTalentEventViewer::SetTalentEventCameraLockToProperty;
	TalentEventFuncToTalentEventInfo[11]	= &TTalentEventViewer::SetPropertyToTalentEventCameraLock;

	TalentEventFuncToProperty[12]			= &TTalentEventViewer::SetTalentEventGroundFireToProperty;
	TalentEventFuncToTalentEventInfo[12]	= &TTalentEventViewer::SetPropertyToTalentEventGroundFire;
}

PropertyGrid^ TTalentEventViewer::GetEventViewerPropertGrid()
{
	return GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt;
}

TreeView^ TTalentEventViewer::GetEventViewerTreeNode()
{
	return GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventTreeView;
}

string TTalentEventViewer::GetTextEventType( TALENT_EVENT nEventType )
{
	if(nEventType == TE_FIRE_EFFECT)
	{
		return "발사";
	}
	else if(nEventType == TE_LAUNCH_PROJECTILE)
	{
		return "발사체";
	}
	else if(nEventType == TE_EFFECT)
	{
		return "이펙트";
	}
	else if(nEventType == TE_SOUND)
	{
		return "사운드";
	}
	else if(nEventType == TE_CAMERA)
	{
		return "카메라";
	}
	else if(nEventType == TE_DAMAGE)
	{
		return "데미지";
	}
	else if(nEventType == TE_CUSTOM)
	{
		return "sh_custom";
	}
	else if(nEventType == TE_DELAYED_ACT)
	{
		return "시간차";
	}
	else if(nEventType == TE_EXTRAACTIVE_1)
	{
		return "extra_active_1";
	}
	else if(nEventType == TE_EXTRAACTIVE_2)
	{
		return "extra_active_2";
	}
	else if(nEventType == TE_CAMEAR_LOCK)
	{
		return "카메라 락";
	}
	else if(nEventType == TE_GROUND_FIRE_EFFECT)
	{
		return "지형 발사";
	}

	// 모름.
	return "없음";
}

TALENT_EVENT TTalentEventViewer::GetDataEventType( char* pchrBuffer )
{
	TALENT_EVENT eEventType = TE_NONE;

	if(!stricmp(pchrBuffer, "발사"))
	{
		eEventType = TE_FIRE_EFFECT;	
	}
	else if(!stricmp(pchrBuffer, "발사체"))
	{
		eEventType = TE_LAUNCH_PROJECTILE;	
	}
	else if(!stricmp(pchrBuffer, "이펙트"))
	{
		eEventType = TE_EFFECT;
	}
	else if(!stricmp(pchrBuffer, "사운드"))
	{
		eEventType = TE_SOUND;
	}	
	else if(!stricmp(pchrBuffer, "카메라"))
	{
		eEventType = TE_CAMERA;
	}
	else if(!stricmp(pchrBuffer, "데미지"))
	{
		eEventType = TE_DAMAGE;
	}
	else if(!stricmp(pchrBuffer, "sh_custom"))
	{
		eEventType = TE_CUSTOM;
	}
	else if(!stricmp(pchrBuffer, "시간차"))
	{
		eEventType = TE_DELAYED_ACT;
	}
	else if(!stricmp(pchrBuffer, "extra_active_1"))
	{
		eEventType = TE_EXTRAACTIVE_1;
	}
	else if(!stricmp(pchrBuffer, "extra_active_2"))
	{
		eEventType = TE_EXTRAACTIVE_2;
	}
	else if(!stricmp(pchrBuffer, "카메라 락"))
	{
		eEventType = TE_CAMEAR_LOCK;
	}
	else if(!stricmp(pchrBuffer, "지형 발사"))
	{
		eEventType = TE_GROUND_FIRE_EFFECT;
	}

	return eEventType;
}

string TTalentEventViewer::GetTextProjectileType( TALENT_PROJECTILE_TYPE nProjectileType )
{
	if(nProjectileType == TPT_MISSILE)
	{
		return TALENT_XML_VALUE_PROJECTILETYPE_MISSILE;
	}
	else if(nProjectileType == TPT_MISSILE_GUIDED)
	{
		return TALENT_XML_VALUE_PROJECTILETYPE_GUIDED;
	}
	else if(nProjectileType == TPT_HITCAPSULE_GUIDED)
	{
		return TALENT_XML_VALUE_PROJECTILETYPE_HITCAPSULE_GUIDED;
	}

	return "none";
}

TALENT_PROJECTILE_TYPE TTalentEventViewer::GetDataProjectileType( string strBuffer )
{
	TALENT_PROJECTILE_TYPE nProjectileType = TPT_NONE;

	if(strBuffer == TALENT_XML_VALUE_PROJECTILETYPE_MISSILE)
	{
		nProjectileType = TPT_MISSILE;	
	}
	else if(strBuffer == TALENT_XML_VALUE_PROJECTILETYPE_GUIDED)
	{
		nProjectileType = TPT_MISSILE_GUIDED;	
	}
	else if(strBuffer == TALENT_XML_VALUE_PROJECTILETYPE_HITCAPSULE_GUIDED)
	{
		nProjectileType = TPT_HITCAPSULE_GUIDED;
	}

	return nProjectileType;
}

string TTalentEventViewer::GetTextProjectileVisualType( TALENT_PROJECTILE_VISUAL_TYPE nProjeceilVisualType )
{
	if(nProjeceilVisualType == TPVT_STRAIGHT)
	{
		return TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_STRAIGHT;
	}
	else if(nProjeceilVisualType == TPVT_CURVE)
	{
		return TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_CURVE;
	}
	else if(nProjeceilVisualType == TPVT_PARABOLA)
	{
		return TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_PARABOLA;
	}

	return "none";
}

TALENT_PROJECTILE_VISUAL_TYPE TTalentEventViewer::GetDataProjectileVisualType( string strBuffer )
{
	TALENT_PROJECTILE_VISUAL_TYPE nProjectileVisualType = TPVT_CURVE;

	if(strBuffer == TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_STRAIGHT)
	{
		nProjectileVisualType = TPVT_STRAIGHT;
	}
	else if(strBuffer == TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_CURVE)
	{
		nProjectileVisualType = TPVT_CURVE;
	}
	else if(strBuffer == TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_PARABOLA)
	{
		nProjectileVisualType = TPVT_PARABOLA;
	}

	return nProjectileVisualType;
}

CSTalentEventInfo* TTalentEventViewer::GetCurrentTalentEventInfo()
{
	if (m_pSelectedTalentInfo == NULL) return NULL;

	TreeNode^ tn = GetEventViewerTreeNode()->SelectedNode;
	// 노드가 root라면... 삭제 할 수 없다.
	if(tn == nullptr ||
		tn->Text == TALENT_EVENT_NORMAL ||
		tn->Text == TALENT_EVENT_ACT)
	{
		return NULL;
	}

	
	CSTalentEventInfo * pTalentEventInfo = NULL;
	CSTalentInfo * pTalentInfo = m_pSelectedTalentInfo;

	// 레퍼런스란면...?
	if(m_pSelectedTalentInfo->m_Events.m_nRefID > 0)
	{
		pTalentInfo = TTalentHelper::GetTalentInfo(m_pSelectedTalentInfo->m_Events.m_nRefID, m_pSelectedTalentInfo->m_Events.m_nRefMode);
		if(pTalentInfo == NULL)
			pTalentInfo = m_pSelectedTalentInfo;
	}

	if(tn->Parent->Text == TALENT_EVENT_NORMAL)
	{
		pTalentEventInfo = m_pEventMgr->GetTalentEventInfo(pTalentInfo, tn->Index, TES_NORMAL);
	}
	else if(tn->Parent->Text == TALENT_EVENT_ACT)
	{
		pTalentEventInfo = m_pEventMgr->GetTalentEventInfo(pTalentInfo, tn->Index, TES_ACT);
	}

	return pTalentEventInfo;
}

CSTalentInfo* TTalentEventViewer::GetCurrentTalentInfo()
{
	return m_pSelectedTalentInfo;
}

TreeNode^ TTalentEventViewer::AddTalentEventView(int nStart)
{
	// 성공
	char chIndex[64]		= {0, };
	String^ strEventStart;
	TreeNode^ tn			= nullptr;
	if(nStart == 0)
	{
		strEventStart = gcnew String(TALENT_EVENT_NORMAL);
		tn = g_pMainApp->SearchNode(GetEventViewerTreeNode()->Nodes, strEventStart);

		if(tn == nullptr)
		{
			// 내용이 없다.
			return nullptr;
		}

		sprintf(chIndex, "Event_%d", ++(m_pEventMgr->m_nMaxNormalIndex));

		tn->Nodes->Add(gcnew String(chIndex));
	}
	else if(nStart == 1)
	{
		strEventStart = gcnew String(TALENT_EVENT_ACT);
		tn = g_pMainApp->SearchNode(GetEventViewerTreeNode()->Nodes, strEventStart);

		if(tn == nullptr)
		{
			// 내용이 없다.
			return nullptr;
		}

		sprintf(chIndex, "ActEvent_%d", ++(m_pEventMgr->m_nMaxActIndex));

		tn->Nodes->Add(gcnew String(chIndex));
	}

	return g_pMainApp->SearchNode(tn->Nodes, gcnew String(chIndex));
}

bool TTalentEventViewer::DeleteTalentEvent()
{
	if (m_pSelectedTalentInfo == NULL) return false;

	TreeNode^ tn = GetEventViewerTreeNode()->SelectedNode;
	// 노드가 root라면... 삭제 할 수 없다.
	if(tn == nullptr ||
		tn->Text == TALENT_EVENT_NORMAL ||
		tn->Text == TALENT_EVENT_ACT)
	{
		return false;
	}

	// 부모 노드 종류 확인
	TALENT_EVENT_STARTTYPE nStartType;
	if(tn->Parent->Text == TALENT_EVENT_NORMAL)
		nStartType = TES_NORMAL;
	else if(tn->Parent->Text == TALENT_EVENT_ACT)
		nStartType = TES_ACT;

	// 데이터 삭제
	if(m_pEventMgr)
	{
		m_pEventMgr->DeleteTalentEventInfo(m_pSelectedTalentInfo, tn->Index, nStartType);
	}

	GetEventViewerTreeNode()->Nodes->Remove( tn );
	GetEventViewerTreeNode()->SelectedNode = nullptr;
	GetEventViewerPropertGrid()->SelectedObject = nullptr;

	return true;
}

void TTalentEventViewer::SetCurrentPropertyTalentEvent(bool bNewData)
{
	// 현재 선택된 노드에서 데이터를 찾는다.
	CSTalentEventInfo * pEventInfo	= GetCurrentTalentEventInfo();
	if(pEventInfo)
	{
		CSTalentInfo * pTalentInfo		= GetCurrentTalentInfo();

		// 프로퍼티에게 셋팅
		// 1. 새로운 데이터 입력
		// 2. 변경된 데이터 입력

		int nEventTypeIndex = 0;
		XmlSerializable::TALENTEVENT^ xevent	= (XmlSerializable::TALENTEVENT^)(GetEventViewerPropertGrid()->SelectedObject);

		// 1 새로운 데이터 입력
		if(bNewData ||
			xevent == nullptr)
		{
			// 이벤트 정보에서 타입을 가져온다.
			nEventTypeIndex = (int)pEventInfo->m_nEvent;
			(this->*TalentEventFuncToProperty[nEventTypeIndex])(pEventInfo, pTalentInfo);
		}
		// 2 변경된 데이터 입력
		else
		{
			NotifyChanging();

			// 프로퍼티에서 타입을  가져온다.
			nEventTypeIndex = GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));

			// 탤런트 정보에서 타입을 가져온다.
			int nEventTypeFromInfo		= (int)pEventInfo->m_nEvent;

			// 타입이 다른지 비교
			if(nEventTypeFromInfo != nEventTypeIndex)
			{
				// 다르다
				// 우선 다뀐 타입으로 새로 셋팅 한다.
				pEventInfo->m_nEvent	= (TALENT_EVENT)nEventTypeIndex;
				(this->*TalentEventFuncToProperty[nEventTypeIndex])(pEventInfo, pTalentInfo);
			}

			// 이벤트 정보 초기화
			pEventInfo->m_nEvent	= TE_NONE;
			pEventInfo->m_fTime1	= -1;
			pEventInfo->m_fTime2	= -1;
			pEventInfo->m_nParam1	= 0;
			pEventInfo->m_nParam2	= 0;
			pEventInfo->m_strParam1.clear();
			pEventInfo->m_strParam2.clear();
			pEventInfo->m_bFollow	= false;

			(this->*TalentEventFuncToTalentEventInfo[nEventTypeIndex])(pEventInfo, pTalentInfo);		
		}

		ChangeEventTreeNodeName(GetTextEventType(pEventInfo->m_nEvent));
	}
	// 없으면... nullptr
	else
	{
		GetEventViewerPropertGrid()->SelectedObject = nullptr;
	}
}

// 일반
bool TTalentEventViewer::SetTalentEventInfoToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_NONE^ xevent = gcnew XmlSerializable::TALENTEVENT_NONE;

	xevent->이벤트타입		= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());
	xevent->시작시간		= pTalentEventInfo->m_fTime1;
	xevent->문자열1			= gcnew String(pTalentEventInfo->m_strParam1.c_str());
	xevent->문자열2			= gcnew String(pTalentEventInfo->m_strParam2.c_str());

	xevent->숫자1			= pTalentEventInfo->m_nParam1;
	xevent->숫자2			= pTalentEventInfo->m_nParam2;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventInfo(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_NONE^ xevent = (XmlSerializable::TALENTEVENT_NONE^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->문자열1));
	pTalentEventInfo->m_strParam2	= string(MStringToCharPointer(xevent->문자열2));

	pTalentEventInfo->m_nParam1		= xevent->숫자1;
	pTalentEventInfo->m_nParam2		= xevent->숫자2;

	return true;
}

// 발사체
bool TTalentEventViewer::SetTalentEventProjectileToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	if(pTalentEventInfo == NULL)
	{
		GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->SelectedObject = nullptr;
		return false;
	}

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_PROJECTILE^ xevent = gcnew XmlSerializable::TALENTEVENT_PROJECTILE;

	xevent->이벤트타입		= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());
	xevent->발사시간		= pTalentEventInfo->m_fTime1;
	xevent->발사체타입		= gcnew String(GetTextProjectileType(pTalentEventInfo->m_ProjectileInfo.m_nType).c_str());
	xevent->비쥬얼타입		= gcnew String(GetTextProjectileVisualType(pTalentEventInfo->m_ProjectileInfo.m_nVisualType).c_str());

	xevent->스피드			= pTalentEventInfo->m_ProjectileInfo.m_fSpeed;
	xevent->판정크기		= pTalentEventInfo->m_ProjectileInfo.m_fHitRadius;
	xevent->사거리			= pTalentEventInfo->m_ProjectileInfo.m_fRange / 100.0f;		// 사거리는 미터이다.

	xevent->x				= pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.x;
	xevent->y				= pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.y;
	xevent->z				= pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.z;

	xevent->본이름			= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strBoneName.c_str());

	xevent->캡슐인덱스		= pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;
	xevent->세그인덱스		= pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;

	xevent->발사체			= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strEffectName.c_str());
	xevent->피격			= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strAttackEffectName.c_str());
	xevent->지형피격		= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strHitGroundEffectName.c_str());
	xevent->방어			= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strDefenceEffectName.c_str());
	xevent->발사			= gcnew String(pTalentEventInfo->m_ProjectileInfo.m_strFireEffectName.c_str());

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventProjectile(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	if(pTalentEventInfo == NULL)
	{
		GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->SelectedObject = nullptr;
		return false;
	}

	XmlSerializable::TALENTEVENT_PROJECTILE^ xevent = (XmlSerializable::TALENTEVENT_PROJECTILE^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->발사시간;

	pTalentEventInfo->m_ProjectileInfo.m_nType			= GetDataProjectileType(MStringToCharPointer(xevent->발사체타입));
	pTalentEventInfo->m_ProjectileInfo.m_nVisualType	= GetDataProjectileVisualType(MStringToCharPointer(xevent->비쥬얼타입));

	pTalentEventInfo->m_ProjectileInfo.m_fSpeed			= xevent->스피드;
	pTalentEventInfo->m_ProjectileInfo.m_fHitRadius		= xevent->판정크기;
	pTalentEventInfo->m_ProjectileInfo.m_fRange			= xevent->사거리 * 100.0f;		// 사거리는 미터이다.

	pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.x	= xevent->x;
	pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.y	= xevent->y;
	pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.z	= xevent->z;

	pTalentEventInfo->m_ProjectileInfo.m_strBoneName		= string(MStringToCharPointer(xevent->본이름));

	pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType	= xevent->캡슐인덱스;
	pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType	= xevent->세그인덱스;


	pTalentEventInfo->m_ProjectileInfo.m_strEffectName			= string(MStringToCharPointer(xevent->발사체));
	pTalentEventInfo->m_ProjectileInfo.m_strAttackEffectName	= string(MStringToCharPointer(xevent->피격));
	pTalentEventInfo->m_ProjectileInfo.m_strHitGroundEffectName	= string(MStringToCharPointer(xevent->지형피격));
	pTalentEventInfo->m_ProjectileInfo.m_strDefenceEffectName	= string(MStringToCharPointer(xevent->방어));
	pTalentEventInfo->m_ProjectileInfo.m_strFireEffectName		= string(MStringToCharPointer(xevent->발사));

	return true;
}

// 이펙트
bool TTalentEventViewer::SetTalentEventEffectToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_EFFECT^ xevent = gcnew XmlSerializable::TALENTEVENT_EFFECT;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->이펙트이름			= gcnew String(pTalentEventInfo->m_strParam1.c_str());
	xevent->본이름				= gcnew String(pTalentEventInfo->m_strParam2.c_str());

	xevent->종료시간			= pTalentEventInfo->m_fTime2;
	xevent->즉시종료			= pTalentEventInfo->m_nParam2;

	xevent->Follow				= pTalentEventInfo->m_bFollow;

	xevent->캡슐인덱스			= pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;
	xevent->세그인덱스			= pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventEffect(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_EFFECT^ xevent = (XmlSerializable::TALENTEVENT_EFFECT^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->이펙트이름));
	pTalentEventInfo->m_strParam2	= string(MStringToCharPointer(xevent->본이름));

	pTalentEventInfo->m_fTime2		= xevent->종료시간;
	pTalentEventInfo->m_nParam2		= xevent->즉시종료;

	pTalentEventInfo->m_bFollow		= xevent->Follow;

	pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType	= xevent->캡슐인덱스;
	pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType	= xevent->세그인덱스;

	return true;
}

// 발사
bool TTalentEventViewer::SetTalentEventFireToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_FIRE_EFFECT^ xevent = gcnew XmlSerializable::TALENTEVENT_FIRE_EFFECT;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->이펙트이름			= gcnew String(pTalentEventInfo->m_strParam1.c_str());
	xevent->본이름				= gcnew String(pTalentEventInfo->m_strParam2.c_str());

	xevent->종료시간			= pTalentEventInfo->m_fTime2;
	xevent->즉시종료			= pTalentEventInfo->m_nParam2;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventFire(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_FIRE_EFFECT^ xevent = (XmlSerializable::TALENTEVENT_FIRE_EFFECT^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->이펙트이름));
	pTalentEventInfo->m_strParam2	= string(MStringToCharPointer(xevent->본이름));

	pTalentEventInfo->m_fTime2		= xevent->종료시간;
	pTalentEventInfo->m_nParam2		= xevent->즉시종료;

	return true;
}

// 사운드
bool TTalentEventViewer::SetTalentEventSoundToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_SOUND^ xevent = gcnew XmlSerializable::TALENTEVENT_SOUND;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());	
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->사운드이름			= gcnew String(pTalentEventInfo->m_strParam1.c_str());
	xevent->본이름				= gcnew String(pTalentEventInfo->m_strParam2.c_str());

	xevent->종료시간			= pTalentEventInfo->m_nParam1;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventSound(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_SOUND^ xevent = (XmlSerializable::TALENTEVENT_SOUND^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->사운드이름));
	pTalentEventInfo->m_strParam2	= string(MStringToCharPointer(xevent->본이름));

	pTalentEventInfo->m_nParam1		= xevent->종료시간;
	pTalentEventInfo->m_nParam2		= 0;

	return true;
}

// 카메라
bool TTalentEventViewer::SetTalentEventCameraToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_CAMERA^ xevent = gcnew XmlSerializable::TALENTEVENT_CAMERA;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());	
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->종료시간			= pTalentEventInfo->m_fTime2;
	xevent->카메라효과이름		= gcnew String(pTalentEventInfo->m_strParam1.c_str());

	xevent->반경				= pTalentEventInfo->m_nParam1;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventCamera(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_CAMERA^ xevent = (XmlSerializable::TALENTEVENT_CAMERA^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_fTime2		= xevent->종료시간;

	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->카메라효과이름));
	pTalentEventInfo->m_strParam2	= string("");

	pTalentEventInfo->m_nParam1		= xevent->반경;

	if(pTalentEventInfo->m_fTime2 != -1 &&
		(pTalentEventInfo->m_fTime1 > pTalentEventInfo->m_fTime2))
	{
		// 경고
		System::Windows::Forms::MessageBox::Show(L"종료시간이 시작시간보다 작습니다.", "에러");
	}
	return true;
}

// 데미지
bool TTalentEventViewer::SetTalentEventDamageToProperty(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_DAMAGE^ xevent = gcnew XmlSerializable::TALENTEVENT_DAMAGE;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());	
	xevent->데미지시간			= pTalentEventInfo->m_fTime1;
	xevent->이펙트				= gcnew String(pTalentEventInfo->m_strParam1.c_str());
	xevent->본					= gcnew String(pTalentEventInfo->m_strParam2.c_str());
	xevent->Rate				= pTalentEventInfo->m_nParam1;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventDamage(CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo)
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_DAMAGE^ xevent = (XmlSerializable::TALENTEVENT_DAMAGE^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->데미지시간;
	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->이펙트));
	pTalentEventInfo->m_strParam2	= string(MStringToCharPointer(xevent->본));
	pTalentEventInfo->m_nParam1		= xevent->Rate;

	return true;
}

// Custom
bool TTalentEventViewer::SetTalentEventCustomToProperty( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_CUSTOM^ xevent = gcnew XmlSerializable::TALENTEVENT_CUSTOM;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());	
	xevent->이벤트이름			= gcnew String(pTalentEventInfo->m_strCustomEvent.c_str());	
	xevent->PARAM				= gcnew String(pTalentEventInfo->m_strParam1.c_str());
	xevent->본					= gcnew String(pTalentEventInfo->m_strParam2.c_str());
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->종료시간			= pTalentEventInfo->m_fTime2;

	xevent->캡슐인덱스			= pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;
	xevent->세그인덱스			= pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;

	xevent->ServerDir			= pTalentEventInfo->m_bServerDir;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventCustom( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_CUSTOM^ xevent = (XmlSerializable::TALENTEVENT_CUSTOM^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_strCustomEvent = string(MStringToCharPointer(xevent->이벤트이름));
	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->PARAM));
	pTalentEventInfo->m_strParam2	= string(MStringToCharPointer(xevent->본));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_fTime2		= xevent->종료시간;

	pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType	= xevent->캡슐인덱스;
	pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType	= xevent->세그인덱스;

	pTalentEventInfo->m_bServerDir	= xevent->ServerDir;

	return true;
}

void TTalentEventViewer::NotifyChanging()
{
	GlobalObjects::g_SaveNotifier.SetSaveTalentEvent(true);
}

void TTalentEventViewer::ChangeEventTreeNodeName( string strEventTypeName, TreeNode^ tn /*= nullptr*/ )
{
	if (m_pSelectedTalentInfo == NULL) return;

	if(tn == nullptr)
	{
		tn = GetEventViewerTreeNode()->SelectedNode;
		if(tn == nullptr ||
			tn->Parent == nullptr)
		{
			return;
		}
	}

	string strNodeName = MStringToCharPointer(tn->Text);
	string strChangeNodeName = strNodeName.substr(0, strNodeName.find("(") - 1) + " (" +strEventTypeName + ")";
	tn->Text = gcnew String(strChangeNodeName.c_str());
}

void TTalentEventViewer::GetTalentHitInfoSegmentIndex( ArrayList^ indexList )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if(pTalentInfo)
	{
		for(int i = -1; i < (int)pTalentInfo->m_HitInfo.m_vSegments.size(); i++)
		{
			indexList->Add(i);
		}
	}
}

void TTalentEventViewer::GetTalentHitInfoCapsuleIndex( ArrayList^ indexList, TALENT_EVENT nEventType )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if(pTalentInfo)
	{
		int nSegmentIndex = 0;

		if(nEventType == TE_LAUNCH_PROJECTILE)
		{
			XmlSerializable::TALENTEVENT_PROJECTILE^ xevent = (XmlSerializable::TALENTEVENT_PROJECTILE^)(GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->SelectedObject);
			if(xevent == nullptr) 
				return;
			nSegmentIndex = xevent->세그인덱스;
		}
		else if(nEventType == TE_EFFECT)
		{
			XmlSerializable::TALENTEVENT_EFFECT^ xevent = (XmlSerializable::TALENTEVENT_EFFECT^)(GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->SelectedObject);
			if(xevent == nullptr) 
				return;
			nSegmentIndex = xevent->세그인덱스;
		}
		else if(nEventType == TE_CUSTOM)
		{
			XmlSerializable::TALENTEVENT_CUSTOM^ xevent = (XmlSerializable::TALENTEVENT_CUSTOM^)(GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->SelectedObject);
			if(xevent == nullptr) 
				return;
			nSegmentIndex = xevent->세그인덱스;
		}

		if(nSegmentIndex == -1)
			return;

		if((int)pTalentInfo->m_HitInfo.m_vSegments.size() > nSegmentIndex)
		{
			for(int i = -1; i < (int)pTalentInfo->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules.size(); i++)
			{
				indexList->Add(i);
			}
		}
	}
}

void TTalentEventViewer::GetProjectileVisualType( ArrayList^ typeList )
{
	CSTalentInfo* pTalentInfo = g_pMainApp->GetTalentInfo();
	if(pTalentInfo)
	{
		XmlSerializable::TALENTEVENT_PROJECTILE^ xevent = (XmlSerializable::TALENTEVENT_PROJECTILE^)(GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->SelectedObject);
		if(xevent == nullptr) 
			return;

		TALENT_PROJECTILE_TYPE eCurrentProjectileType					= GetDataProjectileType(MStringToCharPointer(xevent->발사체타입));

		if(eCurrentProjectileType == TPT_MISSILE)
		{
			typeList->Add("straight");
		}
		else if(eCurrentProjectileType == TPT_MISSILE_GUIDED ||
			eCurrentProjectileType == TPT_HITCAPSULE_GUIDED)
		{
			typeList->Add("straight");
			typeList->Add("curve");
			typeList->Add("parabola");
		}
	}
}

bool TTalentEventViewer::SetTalentEventExtraActive_1ToProperty( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_EXTRA_ACTIVE^ xevent = gcnew XmlSerializable::TALENTEVENT_EXTRA_ACTIVE;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->x					= pTalentEventInfo->m_vLocal.x;
	xevent->y					= pTalentEventInfo->m_vLocal.y;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventExtraActive_1( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_EXTRA_ACTIVE^ xevent = (XmlSerializable::TALENTEVENT_EXTRA_ACTIVE^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_vLocal.x	= xevent->x;
	pTalentEventInfo->m_vLocal.y	= xevent->y;

	return true;
}

bool TTalentEventViewer::SetTalentEventExtraActive_2ToProperty( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	return SetTalentEventExtraActive_1ToProperty(pTalentEventInfo, pTalentInfo);
}

bool TTalentEventViewer::SetPropertyToTalentEventExtraActive_2( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	return SetPropertyToTalentEventExtraActive_1(pTalentEventInfo, pTalentInfo);
}

bool TTalentEventViewer::SetTalentEventDelayedActToProperty( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	return SetTalentEventExtraActive_1ToProperty(pTalentEventInfo, pTalentInfo);
}

bool TTalentEventViewer::SetPropertyToTalentEventDelayedAct( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	return SetPropertyToTalentEventExtraActive_1(pTalentEventInfo, pTalentInfo);
}

void TTalentEventViewer::GetTalentHitInfoSegmentIndexAndCapsuleIndex( int& nSegIndex, int& nCapsuleIndex )
{
	if(GlobalObjects::g_pMainForm->m_EventTabPage->TalentEventAtt->SelectedObject == nullptr)
		return;

	XmlSerializable::TALENTEVENT^ xevent	= (XmlSerializable::TALENTEVENT^)(GetEventViewerPropertGrid()->SelectedObject);
	int nEventType = GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
			
	nSegIndex = -1;
	nCapsuleIndex =  -1;


	if(nEventType == TE_LAUNCH_PROJECTILE)
	{
		XmlSerializable::TALENTEVENT_PROJECTILE^ xevent = (XmlSerializable::TALENTEVENT_PROJECTILE^)(GetEventViewerPropertGrid()->SelectedObject);
		if(xevent == nullptr) 
			return;
		nSegIndex = xevent->세그인덱스;
		nCapsuleIndex = xevent->캡슐인덱스;
	}
	else if(nEventType == TE_EFFECT)
	{
		XmlSerializable::TALENTEVENT_EFFECT^ xevent = (XmlSerializable::TALENTEVENT_EFFECT^)(GetEventViewerPropertGrid()->SelectedObject);
		if(xevent == nullptr) 
			return;
		nSegIndex = xevent->세그인덱스;
		nCapsuleIndex = xevent->캡슐인덱스;
	}
	else if(nEventType == TE_CUSTOM)
	{
		XmlSerializable::TALENTEVENT_CUSTOM^ xevent = (XmlSerializable::TALENTEVENT_CUSTOM^)(GetEventViewerPropertGrid()->SelectedObject);
		if(xevent == nullptr) 
			return;
		nSegIndex = xevent->세그인덱스;
		nCapsuleIndex = xevent->캡슐인덱스;
	}
}

bool TTalentEventViewer::SetTalentEventCameraLockToProperty( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_CAMERA_LOCK^ xevent = gcnew XmlSerializable::TALENTEVENT_CAMERA_LOCK;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());	
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->지속시간			= pTalentEventInfo->m_fTime2;

	xevent->반경				= pTalentEventInfo->m_nParam1;

	xevent->본					= gcnew String(pTalentEventInfo->m_strParam1.c_str());

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventCameraLock( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_CAMERA_LOCK^ xevent = (XmlSerializable::TALENTEVENT_CAMERA_LOCK^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_fTime2		= xevent->지속시간;

	pTalentEventInfo->m_nParam1		= xevent->반경;

	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->본));

	return true;
}

bool TTalentEventViewer::SetTalentEventGroundFireToProperty( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 탤런트 이벤트 정보를 프로퍼티에게 셋팅

	// TODO: pElement 의 모든 속성값을 퍼라퍼티에 표시해준다.
	XmlSerializable::TALENTEVENT_GROUND_FIRE_EFFECT^ xevent = gcnew XmlSerializable::TALENTEVENT_GROUND_FIRE_EFFECT;

	xevent->이벤트타입			= gcnew String(GetTextEventType(pTalentEventInfo->m_nEvent).c_str());
	xevent->시작시간			= pTalentEventInfo->m_fTime1;
	xevent->이펙트이름			= gcnew String(pTalentEventInfo->m_strParam1.c_str());

	xevent->종료시간			= pTalentEventInfo->m_fTime2;

	//데이타 바인딩 ?
	GetEventViewerPropertGrid()->SelectedObject = xevent;

	return true;
}

bool TTalentEventViewer::SetPropertyToTalentEventGroundFire( CSTalentEventInfo * pTalentEventInfo, CSTalentInfo * pTalentInfo )
{
	// 프로퍼티 정보를 탤런트 이펙트 정보에게 셋팅

	XmlSerializable::TALENTEVENT_GROUND_FIRE_EFFECT^ xevent = (XmlSerializable::TALENTEVENT_GROUND_FIRE_EFFECT^)(GetEventViewerPropertGrid()->SelectedObject);

	pTalentEventInfo->m_nEvent		= GetDataEventType((char*)MStringToCharPointer(xevent->이벤트타입));
	pTalentEventInfo->m_fTime1		= xevent->시작시간;
	pTalentEventInfo->m_strParam1	= string(MStringToCharPointer(xevent->이펙트이름));

	pTalentEventInfo->m_fTime2		= xevent->종료시간;

	return true;
}

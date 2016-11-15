#include "stdafx.h"
#include "TTalentEventMgr.h"
#include "MUtil.h"
#include "MMath.h"
#include "MLocale.h"

using namespace System::IO;



//////////////////////////////////////////////////////////////////////////
// [11/22/2007 isnara] 탤런트 이벤트
TTalentEventMgr::TTalentEventMgr()
{
	m_nMaxNormalIndex	= 0;
	m_nMaxActIndex		= 0;
}

TTalentEventMgr::~TTalentEventMgr()
{

}

bool TTalentEventMgr::CreateTalentEventInfo(CSTalentInfo* pTalentInfo, float fStartTime, TALENT_EVENT_STARTTYPE nStartType)
{
	CSTalentEventInfo csTalentEventInfo;
	csTalentEventInfo.m_fTime1 = fStartTime;

	return AddTalentEventInfo(pTalentInfo, csTalentEventInfo, nStartType);
}

bool TTalentEventMgr::DeleteTalentEventInfo( CSTalentInfo* pTalentInfo, int nIndex, TALENT_EVENT_STARTTYPE nStartType )
{
	CSTalentEvents * pEventInfo = GetTalentEvent(pTalentInfo);

	if(pEventInfo)
	{
		int nCount = 0;

		if(nStartType == TES_NORMAL)
		{
			vector<CSTalentEventInfo>::iterator it = pEventInfo->m_vecEvents.begin();
			while(it != pEventInfo->m_vecEvents.end())
			{
				if(nCount == nIndex)
				{
					pEventInfo->m_vecEvents.erase(it);
					break;
				}

				nCount++;
				it++;
			}
		}
		else if(nStartType == TES_ACT)
		{
			vector<CSTalentEventInfo>::iterator it = pEventInfo->m_vecActEvents.begin();
			while(it != pEventInfo->m_vecActEvents.end())
			{
				if(nCount == nIndex)
				{
					pEventInfo->m_vecActEvents.erase(it);
					break;
				}

				nCount++;
				it++;
			}
		}

		return true;
	}

	return false;
}

CSTalentEventInfo * TTalentEventMgr::GetTalentEventInfo(CSTalentInfo* pTalentInfo, int nIndex, TALENT_EVENT_STARTTYPE nStartType)
{
	CSTalentEvents * pEventInfo = GetTalentEvent(pTalentInfo);

	if(pEventInfo)
	{
		int nCount = 0;

		if(nStartType == TES_NORMAL)
		{
			vector<CSTalentEventInfo>::iterator it = pEventInfo->m_vecEvents.begin();
			while(it != pEventInfo->m_vecEvents.end())
			{
				if(nCount == nIndex)
				{
					return &(*it);
				}

				nCount++;
				it++;
			}
		}
		else if(nStartType == TES_ACT)
		{
			vector<CSTalentEventInfo>::iterator it = pEventInfo->m_vecActEvents.begin();
			while(it != pEventInfo->m_vecActEvents.end())
			{
				if(nCount == nIndex)
				{
					return &(*it);
				}

				nCount++;
				it++;
			}
		}
	}

	return NULL;
}

CSTalentEvents * TTalentEventMgr::GetTalentEvent(CSTalentInfo* pTalentInfo)
{
	if(pTalentInfo)
	{
		int nCount = 0;

		return &(pTalentInfo->m_Events);
	}

	return NULL;
}

bool TTalentEventMgr::Save()
{
	string strFileName = FILENAME_TALENT_EXT_TOOL;
	if (Save(strFileName.c_str()) == false) return false;

	File::Copy(FILENAME_TALENT_EXT_TOOL, SERVER_FILENAME_TALENT_EXT, true);

	return true;
}

bool TTalentEventMgr::Save(const char* szFileName)
{
	MXml xml;
	//MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.LinkEndChild(pDec);

	MXmlElement *pMaietElement = new MXmlElement(TALENT_XML_TAG_MAIET);

	// 모든 탤런트 정보 가져오기
	for (TTalentInfoMgr::iterator k = g_pMainApp->m_TTalentInfoMgr.begin();
		k != g_pMainApp->m_TTalentInfoMgr.end(); ++k)
	{
		CSTalentInfo * pTalentInfo = k->second;

		SetTalentXmlElement(pTalentInfo, pMaietElement);

		if (pTalentInfo->ExistMode())
		{
			for (int i = 0; i < MAX_TALENT_MODE; i++)
			{
				CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(i);
				if (pModeTalentInfo == NULL) continue;

				SetTalentXmlElement(pModeTalentInfo, pMaietElement);
			}
		}
	}

	xml.LinkEndChild(pMaietElement);

	return xml.SaveFile(szFileName);	
	return true;
}

void TTalentEventMgr::SaveEvent( MXmlElement * pParentElement, CSTalentEvents & pEvent )
{
	MXmlElement *pEventInfo = new MXmlElement(TALENT_XML_TAG_EVENTS);	

	//------------------------------------------------------------------------
	// 일반
	MXmlElement *pEventDataNormal = new MXmlElement(TALENT_XML_ATTR_TAG_NOMAL);
	for(vector<CSTalentEventInfo>::iterator itNor = pEvent.m_vecEvents.begin(); itNor != pEvent.m_vecEvents.end(); ++itNor)
	{
		SaveEventData(pEventDataNormal, &(*itNor));
	}
	pEventInfo->LinkEndChild(pEventDataNormal);

	//------------------------------------------------------------------------
	// Act
	MXmlElement *pEventDataAct = new MXmlElement(TALENT_XML_ATTR_TAG_ACT);
	for(vector<CSTalentEventInfo>::iterator itAct = pEvent.m_vecActEvents.begin(); itAct != pEvent.m_vecActEvents.end(); ++itAct)
	{
		SaveEventData(pEventDataAct, &(*itAct));
	}
	pEventInfo->LinkEndChild(pEventDataAct);

	pParentElement->LinkEndChild(pEventInfo);
}

void TTalentEventMgr::SaveEventData(MXmlElement * pParentElement, CSTalentEventInfo* pEventInfo)
{
	MXmlElement * pEventData = new MXmlElement(TALENT_XML_ATTR_TAG_EVENT);

	string strEventType;

	if(pEventInfo->m_nEvent == TE_FIRE_EFFECT)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_FIRE_EFFECT);
	else if(pEventInfo->m_nEvent == TE_LAUNCH_PROJECTILE)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_PROJECTILE);
	else if(pEventInfo->m_nEvent == TE_EFFECT)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_EFFECT);
	else if(pEventInfo->m_nEvent == TE_SOUND)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_SOUND);
	else if(pEventInfo->m_nEvent == TE_CAMERA)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_CAMERA);
	else if(pEventInfo->m_nEvent == TE_DAMAGE)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_DAMAGE);
	else if(pEventInfo->m_nEvent == TE_CUSTOM)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_CUSTOM);
	else if(pEventInfo->m_nEvent == TE_DELAYED_ACT)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_DELAYED_ACT);
	else if(pEventInfo->m_nEvent == TE_EXTRAACTIVE_1)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_EXTRAACTIVE_1);
	else if(pEventInfo->m_nEvent == TE_EXTRAACTIVE_2)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_EXTRAACTIVE_2);
	else if(pEventInfo->m_nEvent == TE_CAMEAR_LOCK)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_CAMERA_LOCK);
	else if(pEventInfo->m_nEvent == TE_GROUND_FIRE_EFFECT)
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_GROUND_FIRE_EFFECT);
	else
		strEventType = string(TALENT_XML_VALUE_TALENTEVENTTYPE_NONE);

	_SetAttribute(pEventData, TALENT_XML_VALUE_TYPE, strEventType);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_TIME, pEventInfo->m_fTime1);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_TIME2, pEventInfo->m_fTime2);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_STR_CUSTOM_EVENT, pEventInfo->m_strCustomEvent);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_STR_PARAM1, pEventInfo->m_strParam1);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_STR_PARAM2, pEventInfo->m_strParam2);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_INT_PARAM1, pEventInfo->m_nParam1);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_INT_PARAM2, pEventInfo->m_nParam2);
	_SetAttribute(pEventData, TALENT_XML_ATTR_TALENT_FOLLOW, pEventInfo->m_bFollow);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_LOCAL_X, pEventInfo->m_vLocal.x);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_LOCAL_Y, pEventInfo->m_vLocal.y);
	_SetAttribute(pEventData, TALENT_XML_VALUE_TALENTEVENT_SERVER_DIR, pEventInfo->m_bServerDir);

	// 발사체라면... 정보 저장
	if(pEventInfo->m_nEvent == TE_LAUNCH_PROJECTILE)
	{
		SaveProjectile(pEventData, pEventInfo);
	}
	else
	{
		_SetAttribute(pEventData, TLAENT_XML_ATTR_PROJECTILE_SEGMENT_INDEX, pEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType);
		_SetAttribute(pEventData, TLAENT_XML_ATTR_PROJECTILE_CAPSULE_INDEX, pEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType);
	}

	pParentElement->LinkEndChild(pEventData);
}

int TTalentEventMgr::GetCount( CSTalentInfo* pTalentInfo, TALENT_EVENT_STARTTYPE nStartType )
{
	int nCount = 0;
	//XTalentInfoMgr& mgr = XTalentInfoMgr::GetInstance();

	if(pTalentInfo)
	{
		CSTalentEvents * pEventInfo = &(pTalentInfo->m_Events);

		if(nStartType == TES_NORMAL)
		{
			nCount = pEventInfo->m_vecEvents.size();
		}
		else if(nStartType == TES_ACT)
		{
			nCount = pEventInfo->m_vecActEvents.size();
		}
	}

	return nCount;
}

void TTalentEventMgr::SaveProjectile( MXmlElement * pElement, CSTalentEventInfo* pEventInfo )
{
	//------------------------------------------------------------------------
	// 데이터 입력
	if(pEventInfo->m_ProjectileInfo.m_nType == TPT_NONE)
	{
		// 아무 의미 없는 타입이다.
		return;
	}

	if (pEventInfo->m_ProjectileInfo.m_nType == TPT_MISSILE)				_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_TYPE, TALENT_XML_VALUE_PROJECTILETYPE_MISSILE);
	else if (pEventInfo->m_ProjectileInfo.m_nType == TPT_MISSILE_GUIDED)		_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_TYPE, TALENT_XML_VALUE_PROJECTILETYPE_GUIDED);
	else if (pEventInfo->m_ProjectileInfo.m_nType == TPT_HITCAPSULE_GUIDED)	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_TYPE, TALENT_XML_VALUE_PROJECTILETYPE_HITCAPSULE_GUIDED);

	if(pEventInfo->m_ProjectileInfo.m_nVisualType == TPVT_CURVE)			_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_VISUAL_TYPE, TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_CURVE);
	else if(pEventInfo->m_ProjectileInfo.m_nVisualType == TPVT_STRAIGHT)	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_VISUAL_TYPE, TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_STRAIGHT);
	else if(pEventInfo->m_ProjectileInfo.m_nVisualType == TPVT_PARABOLA)	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_VISUAL_TYPE, TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_PARABOLA);

	float fRange = pEventInfo->m_ProjectileInfo.m_fRange / 100.0f;			// 사거리를 미터로 변경한다.
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_RANGE,			fRange);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_HIT_RADIUS,		pEventInfo->m_ProjectileInfo.m_fHitRadius);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_SPEED,			pEventInfo->m_ProjectileInfo.m_fSpeed);

	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_START_X, pEventInfo->m_ProjectileInfo.m_vLocalStartPos.x);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_START_Y, pEventInfo->m_ProjectileInfo.m_vLocalStartPos.y);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_START_Z, pEventInfo->m_ProjectileInfo.m_vLocalStartPos.z);

	_SetAttribute(pElement, TALENT_XML_ATTR_TALENT_BONENAME, pEventInfo->m_ProjectileInfo.m_strBoneName);

	_SetAttribute(pElement, TLAENT_XML_ATTR_PROJECTILE_SEGMENT_INDEX, pEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType);
	_SetAttribute(pElement, TLAENT_XML_ATTR_PROJECTILE_CAPSULE_INDEX, pEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType);

	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_EFFECT_NAME,				pEventInfo->m_ProjectileInfo.m_strEffectName);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_ATTACK_EFFECT_NAME,		pEventInfo->m_ProjectileInfo.m_strAttackEffectName);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_HITGROUND_EFFECT_NAME,	pEventInfo->m_ProjectileInfo.m_strHitGroundEffectName);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_DEFENCE_EFFECT_NAME,		pEventInfo->m_ProjectileInfo.m_strDefenceEffectName);
	_SetAttribute(pElement, TALENT_XML_ATTR_PROJECTILE_FIRE_EFFECT_NAME,		pEventInfo->m_ProjectileInfo.m_strFireEffectName);
}

void TTalentEventMgr::SetTalentXmlElement( CSTalentInfo * pTalentInfo, MXmlElement * pMaietElement )
{
	MXmlElement *pTalentElement = new MXmlElement(TALENT_XML_TAG_TALENT);	

	_SetAttribute(pTalentElement, TALENT_XML_ATTR_ID, pTalentInfo->m_nID);


	if (pTalentInfo->m_nMode > 0)
	{
		string strValue;
		CSTalentInfoParserHelper helper;
		strValue = helper.MakeXmlValueFromMode(pTalentInfo->m_nMode);

		_SetAttribute(pTalentElement, TALENT_XML_ATTR_MODE, strValue);
	}

	if (pTalentInfo->m_fExtraMotionTime > 0.0f)
	{
		_SetAttribute(pTalentElement, TALENT_XML_ATTR_EXTRA_TIME, pTalentInfo->m_fExtraMotionTime);
	}
	if (pTalentInfo->m_fExtraMotionTime2 > 0.0f)
	{
		_SetAttribute(pTalentElement, TALENT_XML_ATTR_EXTRA_TIME2, pTalentInfo->m_fExtraMotionTime2);
	}
	if (pTalentInfo->m_fExtraMotionTime3 > 0.0f)
	{
		_SetAttribute(pTalentElement, TALENT_XML_ATTR_EXTRA_TIME3, pTalentInfo->m_fExtraMotionTime3);
	}

	// 모션 시간으로 AITime을 설정하기 위함
	SaveTalentActAnimationTime(pTalentInfo, pTalentElement);

	// 레퍼런스를 가지고 있나?
	if(pTalentInfo->m_Events.m_nRefID > 0)
	{
		_SetAttribute(pTalentElement, TALENT_XML_ATTR_REFERENCE_ID, pTalentInfo->m_Events.m_nRefID);
		
		if (pTalentInfo->m_Events.m_nRefMode > 0)
		{
			string strValue;
			CSTalentInfoParserHelper helper;
			strValue = helper.MakeXmlValueFromMode(pTalentInfo->m_Events.m_nRefMode);

			_SetAttribute(pTalentElement, TALENT_XML_ATTR_REFERENCE_MODE, strValue);
		}
	}
	else
	{	
		// 이벤트 정보를 가지고 있는지 확인한다.
		if((pTalentInfo->m_Events.m_vecEvents.size() + pTalentInfo->m_Events.m_vecActEvents.size()) > 0)
		{
			// 이벤트 정보
			SaveEvent(pTalentElement, pTalentInfo->m_Events);
		}
	}

	pMaietElement->LinkEndChild(pTalentElement);
}

bool TTalentEventMgr::AddTalentEventInfo( CSTalentInfo* pTalentInfo, CSTalentEventInfo& talentEvnetInfo, TALENT_EVENT_STARTTYPE eStartType )
{
	CSTalentEvents * pEventInfo = GetTalentEvent(pTalentInfo);

	if(pEventInfo)
	{
		if(eStartType == TES_NORMAL)
		{
			pEventInfo->m_vecEvents.push_back(talentEvnetInfo);
		}
		else if(eStartType == TES_ACT)
		{
			pEventInfo->m_vecActEvents.push_back(talentEvnetInfo);
		}

		return true;
	}

	return false;
}

void TTalentEventMgr::SaveTalentActAnimationTime( CSTalentInfo * pTalentInfo, MXmlElement * pMaietElement )
{
	for(vector<CSTalentActAnimationTimeInfo>::iterator itAniTime = pTalentInfo->m_ActAnimationTime.vecInfo.begin(); itAniTime != pTalentInfo->m_ActAnimationTime.vecInfo.end(); itAniTime++)
	{
		if((*itAniTime).fAniTime > 0)
		{
			MXmlElement *pAnimationTimeElement = new MXmlElement(TALENT_XML_TAG_ACTANITIME);
			_SetAttribute(pAnimationTimeElement, TALENT_XML_ATTR_TAG_WEAPONTYPE, (*itAniTime).nWeaponType);
			_SetAttribute(pAnimationTimeElement, TALENT_XML_ATTR_ACT_ANIMATION_TIME, (*itAniTime).fAniTime);

			pMaietElement->LinkEndChild(pAnimationTimeElement);
		}
	}
	
}
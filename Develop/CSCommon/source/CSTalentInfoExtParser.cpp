#include "stdafx.h"
#include "CSTalentInfoExtParser.h"
#include "CSTalentInfoDef.h"
#include "CSTalentInfoMgr.h"
#include "CSTalentInfoParser.h"
#include "MDebug.h"

void CSTalentInfoExtParser::Parse( CSTalentInfoMgr* pTalentInfoMgr, MXml* pXml, MXmlElement* pElement, CSTalentInfo* pTalentInfo/*=NULL*/ )
{
	if (pTalentInfo == NULL)
	{
		CSTalentInfoParserHelper helper;
		CSTalentInfoParserHelper::TalentInfoID idInfo = helper.ParseTalentID(pXml, pElement);

		pTalentInfo = pTalentInfoMgr->Find(idInfo.nID);

		if (idInfo.nMode > 0)
		{
			pTalentInfo = pTalentInfoMgr->Find(idInfo.nID, idInfo.nMode, true);
		}

		if (pTalentInfo == NULL) 
		{
			mlog ("Error : Loading talent_ext.xml - not exist talentinfo id (%d)\n", idInfo.nID);
			return;
		}
	}

	_Parse(pElement, pXml, pTalentInfo);
}

void CSTalentInfoExtParser::_Parse( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo )
{
	float time	=  0.f;

	//if (_Attribute(&time, pElement, TALENT_XML_ATTR_ACT_ANIMATION_TIME))
	//{
	//	pTalentInfo->m_fMotionTime = time;
	//}

	if (_Attribute(&time, pElement, TALENT_XML_ATTR_EXTRA_TIME))
	{
		pTalentInfo->m_fExtraMotionTime = time;
	}
	if (_Attribute(&time, pElement, TALENT_XML_ATTR_EXTRA_TIME2))
	{
		pTalentInfo->m_fExtraMotionTime2 = time;
	}
	if (_Attribute(&time, pElement, TALENT_XML_ATTR_EXTRA_TIME3))
	{
		pTalentInfo->m_fExtraMotionTime3 = time;
	}

	_Attribute(&(pTalentInfo->m_Events.m_nRefID),	pElement, TALENT_XML_ATTR_REFERENCE_ID);
	if (pTalentInfo->m_Events.m_nRefID >0 )
	{
		tstring strValue;
		if (_Attribute(strValue, pElement, TALENT_XML_ATTR_REFERENCE_MODE))
		{
			CSTalentInfoParserHelper helper;
			pTalentInfo->m_Events.m_nRefMode = helper.MakeModeFromXmlValue(strValue);
		}
	}

	//------------------------------------------------------------------------
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		// 애니메이션 시간
		if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_ACTANITIME))
		{
			_ParseTalentActAnimationTime(pAttElement, pXml, pTalentInfo);
		}
		// 이벤트
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_EVENTS))
		{
			_ParseTalentEvents(pAttElement, pXml, pTalentInfo);
		}
	}
	//------------------------------------------------------------------------
}

void CSTalentInfoExtParser::_ParseTalentEvents( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo )
{
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		if(!_stricmp(pAttElement->Value(), TALENT_XML_ATTR_TAG_NOMAL))
		{
			_ParseTalentEvent(pAttElement, pXml, pTalentInfo->m_Events.m_vecEvents);
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_ATTR_TAG_ACT))
		{
			_ParseTalentEvent(pAttElement, pXml, pTalentInfo->m_Events.m_vecActEvents);
		}
	}
}

void CSTalentInfoExtParser::_ParseTalentEvent( MXmlElement* pElement, MXml* pXml, vector<CSTalentEventInfo>& vecEvents )
{
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		string strValue;
		CSTalentEventInfo csNewEventInfo;

		if(!_stricmp(pAttElement->Value(), TALENT_XML_ATTR_TAG_EVENT))
		{
			_Attribute(strValue,							pAttElement, TALENT_XML_VALUE_TYPE, pXml);
			if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_NONE)
				csNewEventInfo.m_nEvent = TE_NONE;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_FIRE_EFFECT)
				csNewEventInfo.m_nEvent = TE_FIRE_EFFECT;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_PROJECTILE)
				csNewEventInfo.m_nEvent = TE_LAUNCH_PROJECTILE;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_EFFECT)
				csNewEventInfo.m_nEvent = TE_EFFECT;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_SOUND)
				csNewEventInfo.m_nEvent = TE_SOUND;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_CAMERA)
				csNewEventInfo.m_nEvent = TE_CAMERA;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_DAMAGE)
				csNewEventInfo.m_nEvent = TE_DAMAGE;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_CUSTOM)
				csNewEventInfo.m_nEvent = TE_CUSTOM;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_DELAYED_ACT)
				csNewEventInfo.m_nEvent = TE_DELAYED_ACT;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_EXTRAACTIVE_1)
				csNewEventInfo.m_nEvent = TE_EXTRAACTIVE_1;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_EXTRAACTIVE_2)
				csNewEventInfo.m_nEvent = TE_EXTRAACTIVE_2;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_CAMERA_LOCK)
				csNewEventInfo.m_nEvent = TE_CAMEAR_LOCK;
			else if(strValue == TALENT_XML_VALUE_TALENTEVENTTYPE_GROUND_FIRE_EFFECT)
				csNewEventInfo.m_nEvent = TE_GROUND_FIRE_EFFECT;
			else
			{
				_ASSERT(0);
			}


			_Attribute(&csNewEventInfo.m_fTime1,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_TIME);
			_Attribute(&csNewEventInfo.m_fTime2,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_TIME2);
			_Attribute(csNewEventInfo.m_strCustomEvent,					pAttElement, TALENT_XML_VALUE_TALENTEVENT_STR_CUSTOM_EVENT, pXml);
			_Attribute(csNewEventInfo.m_strParam1,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_STR_PARAM1, pXml);
			_Attribute(csNewEventInfo.m_strParam2,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_STR_PARAM2, pXml);
			_Attribute(&csNewEventInfo.m_nParam1,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_INT_PARAM1);
			_Attribute(&csNewEventInfo.m_nParam2,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_INT_PARAM2);
			_Attribute(&csNewEventInfo.m_bFollow,						pAttElement, TALENT_XML_ATTR_TALENT_FOLLOW);
			_Attribute(&csNewEventInfo.m_vLocal.x,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_LOCAL_X);
			_Attribute(&csNewEventInfo.m_vLocal.y,						pAttElement, TALENT_XML_VALUE_TALENTEVENT_LOCAL_Y);
			_Attribute(&csNewEventInfo.m_bServerDir,					pAttElement, TALENT_XML_VALUE_TALENTEVENT_SERVER_DIR);

			if(csNewEventInfo.m_nEvent == TE_LAUNCH_PROJECTILE)
			{
				_ParseProjectile(pAttElement, pXml, &csNewEventInfo);
			}
			else
			{
				_Attribute(&csNewEventInfo.m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType, pAttElement, TLAENT_XML_ATTR_PROJECTILE_SEGMENT_INDEX);
				_Attribute(&csNewEventInfo.m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType, pAttElement, TLAENT_XML_ATTR_PROJECTILE_CAPSULE_INDEX);
			}

			vecEvents.push_back(csNewEventInfo);
		}
	}
}

void CSTalentInfoExtParser::_ParseProjectile( MXmlElement* pElement, MXml* pXml, CSTalentEventInfo* pTalentEventInfo )
{
	string strProjectileType;

	_Attribute(strProjectileType, pElement, TALENT_XML_ATTR_PROJECTILE_TYPE);

	if(strProjectileType.empty() == false)
	{
		if (strProjectileType == TALENT_XML_VALUE_PROJECTILETYPE_MISSILE)					pTalentEventInfo->m_ProjectileInfo.m_nType = TPT_MISSILE;
		else if (strProjectileType == TALENT_XML_VALUE_PROJECTILETYPE_GUIDED)				pTalentEventInfo->m_ProjectileInfo.m_nType = TPT_MISSILE_GUIDED;
		else if (strProjectileType == TALENT_XML_VALUE_PROJECTILETYPE_HITCAPSULE_GUIDED)	pTalentEventInfo->m_ProjectileInfo.m_nType = TPT_HITCAPSULE_GUIDED;

		string strProjectileVisualType;
		_Attribute(strProjectileVisualType, pElement, TALENT_XML_ATTR_PROJECTILE_VISUAL_TYPE);
		if (strProjectileVisualType == TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_CURVE)			pTalentEventInfo->m_ProjectileInfo.m_nVisualType = TPVT_CURVE;
		else if (strProjectileVisualType == TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_STRAIGHT)	pTalentEventInfo->m_ProjectileInfo.m_nVisualType = TPVT_STRAIGHT;
		else if (strProjectileVisualType == TALENT_XML_VALUE_PROJECTILE_VISUAL_TYPE_PARABOLA)	pTalentEventInfo->m_ProjectileInfo.m_nVisualType = TPVT_PARABOLA;


		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_fRange, pElement,		TALENT_XML_ATTR_PROJECTILE_RANGE);
		pTalentEventInfo->m_ProjectileInfo.m_fRange *= 100.0f;						// 발사체 거리는 미터 단위이다.
		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_fHitRadius, pElement,	TALENT_XML_ATTR_PROJECTILE_HIT_RADIUS);
		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_fSpeed, pElement,		TALENT_XML_ATTR_PROJECTILE_SPEED);

		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.x, pElement, TALENT_XML_ATTR_PROJECTILE_START_X);
		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.y, pElement, TALENT_XML_ATTR_PROJECTILE_START_Y);
		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_vLocalStartPos.z, pElement, TALENT_XML_ATTR_PROJECTILE_START_Z);

		_Attribute(pTalentEventInfo->m_ProjectileInfo.m_strBoneName, pElement, TALENT_XML_ATTR_TALENT_BONENAME);

		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType, pElement, TLAENT_XML_ATTR_PROJECTILE_SEGMENT_INDEX);
		_Attribute(&pTalentEventInfo->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType, pElement, TLAENT_XML_ATTR_PROJECTILE_CAPSULE_INDEX);

		_Attribute(pTalentEventInfo->m_ProjectileInfo.m_strEffectName, pElement,			TALENT_XML_ATTR_PROJECTILE_EFFECT_NAME);
		_Attribute(pTalentEventInfo->m_ProjectileInfo.m_strAttackEffectName, pElement,	TALENT_XML_ATTR_PROJECTILE_ATTACK_EFFECT_NAME);
		_Attribute(pTalentEventInfo->m_ProjectileInfo.m_strHitGroundEffectName, pElement, TALENT_XML_ATTR_PROJECTILE_HITGROUND_EFFECT_NAME);
		_Attribute(pTalentEventInfo->m_ProjectileInfo.m_strDefenceEffectName, pElement, TALENT_XML_ATTR_PROJECTILE_DEFENCE_EFFECT_NAME);
		_Attribute(pTalentEventInfo->m_ProjectileInfo.m_strFireEffectName, pElement, TALENT_XML_ATTR_PROJECTILE_FIRE_EFFECT_NAME);
	}
}

void CSTalentInfoExtParser::_ParseExt_Server( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo )
{
	float fValue = 0.0f;
	if (_Attribute(&fValue,	pElement, TALENT_XML_ATTR_EXTRA_TIME))
	{
		pTalentInfo->m_fExtraPhaseTime = fValue;
	}
	if (_Attribute(&fValue,	pElement, TALENT_XML_ATTR_EXTRA_TIME2))
	{
		pTalentInfo->m_fExtraPhaseTime2 = fValue;
	}
	if (_Attribute(&fValue,	pElement, TALENT_XML_ATTR_EXTRA_TIME3))
	{
		pTalentInfo->m_fExtraPhaseTime3 = fValue;
	}
}

void CSTalentInfoExtParser::CopyTalentEventInfoByReference(CSTalentInfoMgr* pTalentInfoMgr)
{
	for(map<int , CSTalentInfo* >::iterator itTalentInfo = pTalentInfoMgr->begin(); itTalentInfo != pTalentInfoMgr->end(); itTalentInfo++)
	{
		CSTalentInfo* pTalentInfo = itTalentInfo->second;

		CopyTalentEventData(pTalentInfoMgr, pTalentInfo);

		// 모드 카피
		for(int i = 0; i < MAX_TALENT_MODE; ++i)
		{
			CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(i);
			if(pModeTalentInfo != NULL)
				CopyTalentEventData(pTalentInfoMgr, pModeTalentInfo);
		}
	}
}

void CSTalentInfoExtParser::_ParseTalentActAnimationTime( MXmlElement* pElement, MXml* pXml, CSTalentInfo* pTalentInfo )
{
	CSTalentActAnimationTimeInfo stActAnimationTime;

	int nWeaponType = 0;
	_Attribute(&nWeaponType, pElement, TALENT_XML_ATTR_TAG_WEAPONTYPE);
	_Attribute(&stActAnimationTime.fAniTime, pElement, TALENT_XML_ATTR_ACT_ANIMATION_TIME);

	stActAnimationTime.nWeaponType = (WEAPON_TYPE)nWeaponType;

	pTalentInfo->m_ActAnimationTime.vecInfo.push_back(stActAnimationTime);
}

void CSTalentInfoExtParser::CopyTalentEventData( CSTalentInfoMgr* pTalentInfoMgr, CSTalentInfo* pTalentInfo )
{
	if(pTalentInfo->m_Events.m_nRefID > 0)
	{
		CSTalentInfo* pRefTalentInfo = pTalentInfoMgr->Find(pTalentInfo->m_Events.m_nRefID, pTalentInfo->m_Events.m_nRefMode, true);

		if(pRefTalentInfo)
		{
			//// 애니메이션 시간
			//for(vector<CSTalentActAnimationTimeInfo>::iterator itRefTalentActAniTime = pRefTalentInfo->m_ActAnimationTime.vecInfo.begin(); itRefTalentActAniTime != pRefTalentInfo->m_ActAnimationTime.vecInfo.end(); itRefTalentActAniTime++)
			//{
			//	pTalentInfo->m_ActAnimationTime.vecInfo.push_back(*itRefTalentActAniTime);
			//}

			// normal
			pTalentInfo->m_Events.m_vecEvents.clear();
			for(vector<CSTalentEventInfo>::iterator itRefTalentNormalEventInfo = pRefTalentInfo->m_Events.m_vecEvents.begin(); itRefTalentNormalEventInfo != pRefTalentInfo->m_Events.m_vecEvents.end(); itRefTalentNormalEventInfo++)
			{
				pTalentInfo->m_Events.m_vecEvents.push_back(*itRefTalentNormalEventInfo);
			}

			// act
			pTalentInfo->m_Events.m_vecActEvents.clear();
			for(vector<CSTalentEventInfo>::iterator itRefTalentActEventInfo = pRefTalentInfo->m_Events.m_vecActEvents.begin(); itRefTalentActEventInfo != pRefTalentInfo->m_Events.m_vecActEvents.end(); itRefTalentActEventInfo++)
			{
				pTalentInfo->m_Events.m_vecActEvents.push_back(*itRefTalentActEventInfo);
			}
		}
	}
}

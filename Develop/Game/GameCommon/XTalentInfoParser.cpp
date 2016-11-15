#include "stdafx.h"
#include "XTalentInfoParser.h"
#include "XTalentInfoMgr.h"
#include "XTalentInfoDef.h"
#include "XTalentEffectInfo.h"

#define TALENT_XML_ATTR_OVERRIDE "override"

void XTalentInfoParser::ParseTalent( XTalentInfoMgr* pTalentInfoMgr, MXmlElement* pElement, MXml* pXml, XTalentInfo* pNewInfo /*= NULL*/ )
{
	int id = -1;
	int mode = 0;

	bool new_talent = false;

	CSTalentInfoParserHelper talentInfoParserHelper;
	CSTalentInfoParserHelper::TalentInfoID talentInfoID = talentInfoParserHelper.ParseTalentID(pXml, pElement);

	id = talentInfoID.nID;
	mode = talentInfoID.nMode;

	if (pNewInfo == NULL)
	{
		pNewInfo = pTalentInfoMgr->Get(id, mode, true);

		if (pNewInfo == NULL)
		{
			new_talent = true;
			pNewInfo = new XTalentInfo();

			if (mode > 0)
			{
				XTalentInfo* pDefaultTalentInfo = pTalentInfoMgr->Get(id);
				if (pDefaultTalentInfo)
				{
					pDefaultTalentInfo->CopyAll(pNewInfo);
				}
			}
		}
	}

	int nOverride = INVALID_ID;
	if (_Attribute(nOverride,		pElement, TALENT_XML_ATTR_OVERRIDE))
	{
		XTalentInfo* pSourceTalent = pTalentInfoMgr->Get(nOverride);
		if (pSourceTalent)
		{
			pSourceTalent->CopyAll(pNewInfo);

			pNewInfo->m_nID = id;
			pNewInfo->m_nMode = mode;
		}
		else
		{
			mlog3("invalid override talentid (source: %d, target: %d:%d)\n", nOverride, id, mode);
		}
	}

	if (pNewInfo->m_nID == INVALID_ID)
	{
		pNewInfo->m_nID = id;
	}
	pNewInfo->m_nMode = mode;

	// 클라이언트, 서버 공통으로 읽는 값
	ParseTalentCommon(pNewInfo, pElement, pXml);

	ParseTalentClient(pNewInfo, pXml, pElement);

	pTalentInfoMgr->Insert(pNewInfo);
}


void XTalentInfoParser::ParseTalentClient( XTalentInfo* pNewInfo, MXml* pXml, MXmlElement* pElement )
{
	string strName;
	string strValue;

	bool bLoop=false;
	bool bCancelEnabled = false;
	bool bWeaponAni = false;

	_Attribute(&pNewInfo->m_bUIIndividual,			pElement, TALENT_XML_ATTR_UIINDIVIDUAL);
	_Attribute(&pNewInfo->m_nHPCost,				pElement, TALENT_XML_ATTR_HP_COST);
	_Attribute(&pNewInfo->m_nENCost,				pElement, TALENT_XML_ATTR_EN_COST);
	_Attribute(&pNewInfo->m_nSTACost,				pElement, TALENT_XML_ATTR_STA_COST);
	_Attribute(pNewInfo->m_szCastingAnimation,		pElement, TALENT_XML_ATTR_CASTING_ANI);
	_Attribute(pNewInfo->m_szCastingLoopAnimation,	pElement, TALENT_XML_ATTR_CASTINGLOOP_ANI);
	_Attribute(&pNewInfo->m_fCastingTime,			pElement, TALENT_XML_ATTR_CASTING_TIME);
	_Attribute(pNewInfo->m_szUseAnimation,			pElement, TALENT_XML_ATTR_USE_ANI);
	_Attribute(pNewInfo->m_szExtraActAnimation,		pElement, TALENT_XML_ATTR_EXTRA_ACT_ANI);
	_Attribute(pNewInfo->m_szExtraActAnimation2,	pElement, TALENT_XML_ATTR_EXTRA_ACT_ANI_2);
	_Attribute(pNewInfo->m_szExtraActAnimation3,	pElement, TALENT_XML_ATTR_EXTRA_ACT_ANI_3);
	_Attribute(&pNewInfo->m_bAffectedMotionSpd,		pElement, TALENT_XML_ATTR_AFFECTED_MOTION_SPEED, false);

	_Attribute(&pNewInfo->m_bFreezeFrame,			pElement, TALENT_XML_ATTR_FREEZE_FRAME, false);

	if (_Attribute(strValue,						pElement, TALENT_XML_ATTR_ATTACK_DIRECTION))
	{
		if (strValue == TALENT_XML_VALUE_ATTACKDIRECTION_FRONT) pNewInfo->m_nAttackDirection = MDIR_FRONT;
		else if (strValue == TALENT_XML_VALUE_ATTACKDIRECTION_BACK) pNewInfo->m_nAttackDirection = MDIR_BACK;
		else if (strValue == TALENT_XML_VALUE_ATTACKDIRECTION_LEFT) pNewInfo->m_nAttackDirection = MDIR_LEFT;
		else if (strValue == TALENT_XML_VALUE_ATTACKDIRECTION_RIGHT) pNewInfo->m_nAttackDirection = MDIR_RIGHT;
	}

	// NPC만
	_Attribute(&pNewInfo->m_bHitCapsulePosSync,		pElement, TALENT_XML_ATTR_HIT_POS_SYNC);

	_Attribute(&pNewInfo->m_bUseBloodEffect,		pElement, TALENT_XML_VALUE_USE_BLOOD_EFFECT);

	_Attribute(pNewInfo->m_szIcon,					pElement, TALENT_XML_ATTR_ICONNAME);

	_Attribute(pNewInfo->m_strGrappledAni,			pElement, TALENT_XML_VALUE_GRAPPLED_ANI);
	_Attribute(pNewInfo->m_strGrappledBone,			pElement, TALENT_XML_VALUE_GRAPPLED_BONE);

	_Attribute(&pNewInfo->m_bShoveWhileMoving,		pElement, TALENT_XML_ATTR_SHOVE_WHILE_MOVING, true);

	TALENT_SHOVE_TYPE shoveType = ParseTalentShoveType(pElement, TALENT_XML_ATTR_PC_SHOVE_TYPE);
	if(shoveType == TST_NONE)
		shoveType = ParseTalentShoveType(pElement, TALENT_XML_ATTR_NPC_SHOVE_TYPE);
	
	if(shoveType == TST_NONE)
		shoveType = TST_STOP;

	pNewInfo->m_eTalentShoveType = shoveType;

	_Attribute(&pNewInfo->m_fHitBloodScale,			pElement, TALENT_XML_ATTR_HIT_BLOOD_SCALE);
}

TALENT_SHOVE_TYPE XTalentInfoParser::ParseTalentShoveType( MXmlElement* pElement, string strParseName )
{
	string strValue;

	_Attribute(strValue,							pElement, strParseName.c_str());
	if(strValue == TALENT_XML_ATTR_SHOVE_TYPE_STOP)
		return TST_STOP;
	else if(strValue == TALENT_XML_ATTR_SHOVE_TYPE_PASS)
		return TST_PASS;
	else if(strValue == TALENT_XML_ATTR_SHOVE_TYPE_SCRAPE)
		return TST_SCRAPE;
	else if(strValue == TALENT_XML_ATTR_SHOVE_TYPE_PUSH)
		return TST_PUSH;

	return TST_NONE;
}
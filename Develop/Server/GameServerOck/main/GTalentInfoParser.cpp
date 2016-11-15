#include "stdafx.h"
#include "GTalentInfoParser.h"
#include "GTalentInfoDef.h"
#include "GTalentInfoMgr.h"
#include "GMath.h"
#include "CSStrings.h"
#include "CSCommonParser.h"
#include "GGlobal.h"
#include "GValidateLogger.h"

void GTalentInfoParser::ParseTalent( GTalentInfoMgr* pTalentInfoMgr, MXmlElement* pElement, MXml* pXml )
{
	int id = -1;
	int mode = 0;

	GTalentInfo* pNewInfo = NULL;
	bool new_talent = false;

	CSTalentInfoParserHelper talentInfoParserHelper;
	CSTalentInfoParserHelper::TalentInfoID talentInfoID = talentInfoParserHelper.ParseTalentID(pXml, pElement);

	_VLE(_T(TALENT_XML_TAG_TALENT));	
	_VLA(_T(TALENT_XML_ATTR_ID), talentInfoID.nID);
	_VLP;

	id = talentInfoID.nID;
	mode = talentInfoID.nMode;

	pNewInfo = pTalentInfoMgr->Find(id, mode, true);

	if (pNewInfo == NULL)
	{
		new_talent = true;
		pNewInfo = new GTalentInfo();

		if (mode > 0)
		{
			GTalentInfo* pDefaultTalentInfo = pTalentInfoMgr->Find(id);
			if (pDefaultTalentInfo)
			{
				pDefaultTalentInfo->Clone(pNewInfo);
			}
		}
	}

	pNewInfo->m_nID = id;
	pNewInfo->m_nMode = mode;


	// 클라이언트, 서버 공통으로 읽는 값
	ParseTalentCommon(pNewInfo, pElement, pXml);

	ParseTalentServer(pNewInfo, pXml, pElement);

	if (new_talent)
	{
		pTalentInfoMgr->Insert(pNewInfo);
	}
	else
	{		
		_VLOGGER->Log(_T(TALENT_XML_ATTR_ID) IS_ALREADY_EXIST);
	}
}

/*

*/

void GTalentInfoParser::ParseTalentServer( CSTalentInfo* pTalentInfo, MXml* pXml, MXmlElement* pElement )
{
	CSTalentInfoParser::ParseTalentServer(pTalentInfo, pXml, pElement);

	wstring strName;
	wstring strValue;

	bool bLoop=false;
	bool bCancelEnabled = false;
	bool bWeaponAni = false;

	GTalentInfo* pgTalentInfo = static_cast<GTalentInfo*>(pTalentInfo);

	_Attribute(pgTalentInfo->m_strSpecialization,		pElement, TALENT_XML_ATTR_SPECIALIZATION);
	_Attribute(&pgTalentInfo->m_bBubbleHit,				pElement, TALENT_XML_ATTR_BUBBLEHIT);
	_Attribute(&pgTalentInfo->m_bNonCombatOnly,			pElement, TALENT_XML_ATTR_NONCOMBAT_ONLY);
	_Attribute(&pgTalentInfo->m_nBackHitTalent,			pElement, TALENT_XML_ATTR_BACK_HIT_TALENT);
	_Attribute(&pgTalentInfo->m_bHitOverlap,			pElement, TALENT_XML_ATTR_HIT_OVERLAP);
	_Attribute(&pgTalentInfo->m_fTalentStartRange,		pElement, TALENT_XML_ATTR_TALENT_EFFECTIVE_RANGE);
	_Attribute(&pgTalentInfo->m_bUseOnlyGM,				pElement, TALENT_XML_ATTR_USE_ONLY_GM);
}




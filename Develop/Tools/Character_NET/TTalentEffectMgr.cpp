#include "stdafx.h"
#include "TTalentEffectMgr.h"
#include "XTalentEffectInfoParser.h"
#include "MUtil.h"
#include "MMath.h"
#include "MLocale.h"

using namespace System::IO;

///////////////////////////////////////////////////////////////////////////////
TTalentEffectMgr::TTalentEffectMgr(CSTalentInfoMgr* pTalentInfoMgr)
{
	XTalentEffectInfoParser talentEffectInfoParse;
	talentEffectInfoParse.Load(FILENAME_TALENT_EFFECTINFO_TOOL, pTalentInfoMgr, m_mapEffectBase);
}
TTalentEffectMgr::~TTalentEffectMgr()
{
	map<int, XTalentEffectBase *>::iterator itBase = m_mapEffectBase.begin();
	while(itBase != m_mapEffectBase.end())
	{
		XTalentEffectBase * pBase = itBase->second;
		SAFE_DELETE(pBase);
		itBase++;
	}

	m_mapEffectBase.clear();
}


// 툴에서 새로 만들때 들어오는 코드. 기존에 이펙트 데이터 매니저가 있어야 한다.
XTalentEffectInfo *	TTalentEffectMgr::CreateTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName)
{
	if(pEffectDataMgr == NULL ||
		!stricmp(chrEffectInfoName, ""))
	{
		// Data Manager 부터 있어야 한다.
		return NULL;
	}

	// 중복 체크
	if(CheckDuplicationTalentEffectInfo(pEffectDataMgr, chrEffectInfoName) == true)
	{
		return NULL;
	}

	XTalentEffectInfo * pEffectInfo = new XTalentEffectInfo();
	pEffectInfo->m_strTalentEffectInfoName = string(chrEffectInfoName);

	pEffectDataMgr->push_back(pEffectInfo);

	return pEffectInfo;
}

bool TTalentEffectMgr::DeleteTalentEffectData(XTalentEffectDataMgr* pEffectDataMgr)
{
	pEffectDataMgr->Clear();
	return true;
}

bool TTalentEffectMgr::DeleteTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName, int& nOutTalentEffectInfoCount)
{
	vector<XTalentEffectInfo *>::iterator itInfo = pEffectDataMgr->begin();
	while(itInfo != pEffectDataMgr->end())
	{
		if(!stricmp((*itInfo)->m_strTalentEffectInfoName.c_str(), chrEffectInfoName))
		{
			SAFE_DELETE(*itInfo);
			pEffectDataMgr->erase(itInfo);
			nOutTalentEffectInfoCount = pEffectDataMgr->size();
			return true;
		}

		itInfo++;
	}

	return false;
}

bool TTalentEffectMgr::DeleteTalentEffectInfo( XTalentEffectDataMgr* pEffectDataMgr, TALENT_EFFECT_TYPE nType )
{
	vector<XTalentEffectInfo *>::iterator itInfo = pEffectDataMgr->begin();
	while(itInfo != pEffectDataMgr->end())
	{
		if((*itInfo)->m_nEffectType == nType)
		{
			SAFE_DELETE(*itInfo);
			pEffectDataMgr->erase(itInfo);
			return true;
		}

		itInfo++;
	}

	return false;
}

void TTalentEffectMgr::AddTalentEffectElement( MXmlElement* pXmlElement, TTalentInfo* pTalentInfo )
{
	XTalentEffectDataMgr* pEffectDataMgr = pTalentInfo->GetEffectDataMgr();
	if (pEffectDataMgr->IsEmpty() && (pEffectDataMgr->m_nRefID == 0)) return;

	MXmlElement* pEffectDataElement = new MXmlElement(TALENT_XML_TAG_TALENT_EFFECT);	

	CSTalentInfoParserHelper helper;
	helper.SetXmlElement_TalentIDAndMode(pEffectDataElement, pTalentInfo->m_nID, pTalentInfo->m_nMode);

	if(pEffectDataMgr->m_nRefID > 0)
	{
		_SetAttribute(pEffectDataElement, TALENT_XML_ATTR_REFERENCE_ID, pEffectDataMgr->m_nRefID);

		if (pEffectDataMgr->m_nRefMode > 0)
		{
			string strValue;
			strValue = helper.MakeXmlValueFromMode(pEffectDataMgr->m_nRefMode);

			_SetAttribute(pEffectDataElement, TALENT_XML_ATTR_REFERENCE_MODE, strValue);
		}
	}
	else
	{
		for (vector<XTalentEffectInfo*>::iterator i = pEffectDataMgr->begin(); i != pEffectDataMgr->end(); ++i)
		{
			MXmlElement *pEffect = NULL;

			if((*i)->m_nEffectType == CAST_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_CAST_EFFECT);
			else if((*i)->m_nEffectType == USE_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_USE_EFFECT);
			else if((*i)->m_nEffectType == ATTACK_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_ATTACK_EFFECT);
			else if((*i)->m_nEffectType == MISS_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_MISS_EFFECT);
			else if((*i)->m_nEffectType == SHOCK_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_SHOCK_EFFECT);
			else if((*i)->m_nEffectType == DURATION_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_DURATION_EFFECT);
			else if((*i)->m_nEffectType == DEFENSE_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_DEFENSE_EFFECT);
			if((*i)->m_nEffectType == CAST_END_EFFECT)
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_CAST_END_EFFECT);
			else if((*i)->m_nEffectType == HIT_EFFECT_POSDIR)
			{
				pEffect = new MXmlElement(TALENT_XML_TAG_TALENT_HIT_EFFECT_POSDIR);
				AddTalentHitEffectPosDirElement(pEffect, *i);
				pEffectDataElement->LinkEndChild(pEffect);
				continue;
			}

			// 한글때문에 ConvAnsiToUTF8()를 쓴다. 그러나... 조금 불안하다고 한다.
			_SetAttribute(pEffect, TALENT_XML_ATTR_TALENT_EFFECTINFONAME, MLocale::ConvAnsiToUTF8((*i)->m_strTalentEffectInfoName.c_str()));
			_SetAttribute(pEffect, TALENT_XML_ATTR_TALENT_EFFECTNAME, (*i)->m_strEffectName);
			_SetAttribute(pEffect, TALENT_XML_ATTR_TALENT_BONENAME, (*i)->m_strBoneName);
			_SetAttribute(pEffect, TALENT_XML_ATTR_TALENT_PARAMETER, (*i)->m_strParameter);

			_SetAttribute(pEffect, TALENT_XML_ATTR_TALENT_SCALE, (*i)->m_bScale);

			_SetAttribute(pEffect, TALENT_XML_ATTR_TALENT_EFFECTDIR, (*i)->m_nEffectDirection);
			_SetAttribute(pEffect, TALENT_XML_ATTR_TALENT_IMPACTPOSITION, (*i)->m_nImpactPos);

			pEffectDataElement->LinkEndChild(pEffect);
		}
	}

	pXmlElement->LinkEndChild(pEffectDataElement);

}

bool TTalentEffectMgr::Save()
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.LinkEndChild(pDec);

	MXmlElement *pMaiet = new MXmlElement(TALENT_XML_TAG_MAIET);

	// [12/12/2007 isnara] Talent Effect Base
	SaveTalentEffectBase(pMaiet);

	for (TTalentInfoMgr::iterator itor = g_pMainApp->m_TTalentInfoMgr.begin(); itor != g_pMainApp->m_TTalentInfoMgr.end(); ++itor)
	{
		TTalentInfo* pTalentInfo = (TTalentInfo*)(*itor).second;
		AddTalentEffectElement(pMaiet, pTalentInfo);

		if (pTalentInfo->ExistMode())
		{
			for (int nModeIndex = 0; nModeIndex < MAX_TALENT_MODE; nModeIndex++)
			{
				CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(nModeIndex);
				if (pModeTalentInfo == NULL) continue;

				AddTalentEffectElement(pMaiet, (TTalentInfo*)pModeTalentInfo);
			}
		}
	}

	xml.LinkEndChild(pMaiet);

	return xml.SaveFile(FILENAME_TALENT_EFFECTINFO_TOOL);	
}

void TTalentEffectMgr::SaveTalentEffectBase( MXmlElement * pParentElement )
{
	MXmlElement *pEffectBaseXML = new MXmlElement(TALENT_XML_TAG_TALENT_BASE);

	map<int, XTalentEffectBase *>::iterator it = m_mapEffectBase.begin();
	while(it != m_mapEffectBase.end())
	{
		MXmlElement *pEffectBaseAtt = NULL;
		XTalentEffectBase * pEffectBase = it->second;

		if(pEffectBase->m_nEffectBaseTYpe == TEBY_MELEE_ATTACK)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_MELEE);
		else if(pEffectBase->m_nEffectBaseTYpe == TEBY_MELEE_SECONDARY_ATTACK)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_MELEE_SECONDARY);
		else if(pEffectBase->m_nEffectBaseTYpe == TEBY_MAGIC_ATTACK)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_MAGIC);
		else if(pEffectBase->m_nEffectBaseTYpe == TEBY_ARCHERY_ATTACK)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_ARCHERY);
		else if(pEffectBase->m_nEffectBaseTYpe == TEBY_GUN_ATTACK)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_GUN);
		else if(pEffectBase->m_nEffectBaseTYpe == TEBY_DEFENSE)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_DEFENSE);
		else if(pEffectBase->m_nEffectBaseTYpe == TEBY_NOSHIELD_DEFENSE)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_NOSHIELD_DEFENSE);
		else if(pEffectBase->m_nEffectBaseTYpe == TEBY_MONSTER_DEFENSE)
			pEffectBaseAtt = new MXmlElement(TALENT_XML_TAG_TALENT_MONSTER_DEFENSE);

		_SetAttribute(pEffectBaseAtt, TALENT_XML_ATTR_TALENT_EFFECTNAME, pEffectBase->m_strEffectBaseFileName);
		_SetAttribute(pEffectBaseAtt, TALENT_XML_ATTR_TALENT_BONENAME, pEffectBase->m_strBoneName);

		_SetAttribute(pEffectBaseAtt, TALENT_XML_ATTR_TALENT_SCALE, pEffectBase->m_bScale);

		_SetAttribute(pEffectBaseAtt, TALENT_XML_ATTR_TALENT_IMPACTPOSITION, pEffectBase->m_nImpactPos);
		_SetAttribute(pEffectBaseAtt, TALENT_XML_ATTR_TALENT_EFFECTDIR, pEffectBase->m_nEffectDirection);

		pEffectBaseXML->LinkEndChild(pEffectBaseAtt);
		it++;
	}

	pParentElement->LinkEndChild(pEffectBaseXML);
}




XTalentEffectInfo *	TTalentEffectMgr::GetTalentEffectInfo(XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName)
{
	vector<XTalentEffectInfo *>::iterator itInfo = pEffectDataMgr->begin();
	while(itInfo != pEffectDataMgr->end())
	{
		if(!stricmp((*itInfo)->m_strTalentEffectInfoName.c_str(), chrEffectInfoName))
		{
			return *itInfo;
		}

		itInfo++;
	}

	return NULL;
}

void TTalentEffectMgr::GetTalentEffectInfo( XTalentEffectDataMgr* pEffectDataMgr, TALENT_EFFECT_TYPE nType, vector<XTalentEffectInfo *> &TalentEffectInfo )
{
	if(pEffectDataMgr)
	{
		pEffectDataMgr->Get(nType, TalentEffectInfo);
	}
}

void TTalentEffectMgr::GetDefaultTalentEffectInfo( TALENT_SKILL_TYPE nSkillType, TALENT_EFFECT_TYPE nType, XTalentEffectBase * pTalentEffectBaseInfo )
{
	// 단, 디폴트 이펙트는 피격과 방어뿐이다.
	if(nType == ATTACK_EFFECT)
	{
		if(nSkillType == ST_MELEE)
			pTalentEffectBaseInfo = GetTalentEffectBase(TEBY_MELEE_ATTACK);
		else if(nSkillType == ST_MAGIC)
			pTalentEffectBaseInfo = GetTalentEffectBase(TEBY_MAGIC_ATTACK);
		else if(nSkillType == ST_ARCHERY)
			pTalentEffectBaseInfo = GetTalentEffectBase(TEBY_ARCHERY_ATTACK);
		else if(nSkillType == ST_GUN)
			pTalentEffectBaseInfo = GetTalentEffectBase(TEBY_GUN_ATTACK);
	}
	else if(nType == DEFENSE_EFFECT)
	{
		pTalentEffectBaseInfo = GetTalentEffectBase(TEBY_DEFENSE);
	}
}

XTalentEffectBase * TTalentEffectMgr::GetTalentEffectBase( TALENT_EFFECT_BASE_TYPE nType )
{
	map<int, XTalentEffectBase *>::iterator it = m_mapEffectBase.find((int)nType);
	if(it != m_mapEffectBase.end())
	{
		return it->second;
	}

	return NULL;
}

void TTalentEffectMgr::AddTalentHitEffectPosDirElement( MXmlElement* pXmlElement, XTalentEffectInfo* pTalentEffectInfo )
{
	for(vector<XTalentHitEffectPosDir>::iterator it = pTalentEffectInfo->m_vecTalentHitEffectPosDir.begin(); it != pTalentEffectInfo->m_vecTalentHitEffectPosDir.end(); ++it)
	{
		MXmlElement *pData = new MXmlElement(TALENT_XML_TAG_TALENT_DATA);
		_SetAttribute(pData, TALENT_XML_ATTR_TALENT_HIT_MESH_NAME, MLocale::ConvAnsiToUTF8(it->strMeshName.c_str()));
		_SetAttribute(pData, TALENT_XML_ATTR_TALENT_HIT_TIME, it->fHitTime);

		char szBuffer[512] = {0,};

		sprintf_s(szBuffer,"%f %f %f",
			it->vHitEffectPos.x, it->vHitEffectPos.y, it->vHitEffectPos.z);
		_SetAttribute(pData, TALENT_XML_ATTR_TALENT_HIT_POS, szBuffer);

		sprintf_s(szBuffer,"%f %f %f",
			it->vSwordTrailHitUp.x, it->vSwordTrailHitUp.y, it->vSwordTrailHitUp.z);
		_SetAttribute(pData, TALENT_XML_ATTR_TALENT_HIT_UP_VEC, szBuffer);

		pXmlElement->LinkEndChild(pData);
	}

}

bool TTalentEffectMgr::CheckDuplicationTalentEffectInfo( XTalentEffectDataMgr* pEffectDataMgr, const char* chrEffectInfoName )
{
	// 중복 체크
	if (pEffectDataMgr->Get(string(chrEffectInfoName)) != NULL)
	{
		return true;
	}

	return false;
}

void SetTextEffectDirectionList(string& strBuffer, TALENT_EFFECT_DIRECTION nDirType)
{
	if(nDirType == TED_NONE)
	{
		strBuffer = "방향성 없음";
	}
	else if(nDirType == TED_FRONT_EFFECT_USER)
	{
		strBuffer = "이팩트사용자 정면";
	}
	else if(nDirType == TED_AUTO_DIR_ATTACK_EFFECT)
	{
		strBuffer = "칼 방향";
	}
}

TALENT_EFFECT_DIRECTION GetEffectDirectionList( string& strBuffer )
{
	TALENT_EFFECT_DIRECTION ted_type = TED_NONE;

	if(strBuffer == "이팩트사용자 정면")
	{
		ted_type = TED_FRONT_EFFECT_USER;	
	}
	else if(strBuffer == "칼 방향")
	{
		ted_type = TED_AUTO_DIR_ATTACK_EFFECT;
	}

	return ted_type;
}
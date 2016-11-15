#include "stdafx.h"
#include "XTalentEffectInfoParser.h"
#include "XTalentEffectInfo.h"
#include "XTalentInfoDef.h"
#include "XTalentInfoMgr.h"
#include "XTalentInfo.h"
#include "CSTalentInfoParser.h"
#include "MLocale.h"

XTalentEffectInfoParser::XTalentEffectInfoParser()
{
}

XTalentEffectInfoParser::~XTalentEffectInfoParser()
{

}

bool XTalentEffectInfoParser::Load( const TCHAR* szFileName, CSTalentInfoMgr* pTalentInfoMgr, map<int, XTalentEffectBase *>& mapEffectBase )
{
	MXml xml;

	if(!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) 
	{
		return false;
	}

	//------------------------------------------------------------------------
	// 기본으로 미리 생성한다.
	for(int i = 0; i < TEBY_BASE_MAX; i++)
	{
		XTalentEffectBase *	pEffectBase	= NULL;
		pEffectBase = new XTalentEffectBase((TALENT_EFFECT_BASE_TYPE)i);

		if(pEffectBase)
		{
			map<int, XTalentEffectBase *>::_Pairib tf = mapEffectBase.insert(map<int, XTalentEffectBase *>::value_type(pEffectBase->m_nEffectBaseTYpe, pEffectBase));
		}
	}
	//------------------------------------------------------------------------

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_BASE))
		{
			ParseTalentBase(pElement, &xml, mapEffectBase);
		}
		else if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_EFFECT))
		{
			ParseTalentEffect(pElement, &xml, pTalentInfoMgr);
		}
	}

	return true;
}

void XTalentEffectInfoParser::ParseTalentBase(MXmlElement* pElement, MXml* pXml, map<int, XTalentEffectBase *>& mapEffectBase)
{
	// 속성
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		TALENT_EFFECT_BASE_TYPE nBaseType = TEBY_BASE_MAX;
		if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_MELEE))
		{
			nBaseType = TEBY_MELEE_ATTACK;
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_MELEE_SECONDARY))
		{
			nBaseType = TEBY_MELEE_SECONDARY_ATTACK;
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_MAGIC))
		{
			nBaseType = TEBY_MAGIC_ATTACK;
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_ARCHERY))
		{
			nBaseType = TEBY_ARCHERY_ATTACK;
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_GUN))
		{
			nBaseType = TEBY_GUN_ATTACK;
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_DEFENSE))
		{
			nBaseType = TEBY_DEFENSE;
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_NOSHIELD_DEFENSE))
		{
			nBaseType = TEBY_NOSHIELD_DEFENSE;
		}
		else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_MONSTER_DEFENSE))
		{
			nBaseType = TEBY_MONSTER_DEFENSE;
		}
		else
		{
			continue;
		}

		map<int, XTalentEffectBase *>::iterator ebFind = mapEffectBase.find(nBaseType);
		if(ebFind != mapEffectBase.end())
		{
			ParseTalentBaseInfo(pAttElement, ebFind->second);
		}
	}
}

void XTalentEffectInfoParser::ParseTalentBaseInfo(MXmlElement* pAttElement, XTalentEffectBase * pEffectBase)
{
	if(pEffectBase)
	{
		int nValue = 0;

		_Attribute(pEffectBase->m_strEffectBaseFileName,	pAttElement, TALENT_XML_ATTR_TALENT_EFFECTNAME);
		_Attribute(pEffectBase->m_strBoneName,				pAttElement, TALENT_XML_ATTR_TALENT_BONENAME);
		_Attribute(&nValue,									pAttElement, TALENT_XML_ATTR_TALENT_IMPACTPOSITION);

		pEffectBase->m_nImpactPos = (IMPACTPOS)nValue;

		int nDirType = 1;
		_Attribute(&nDirType, pAttElement, TALENT_XML_ATTR_TALENT_EFFECTDIR);
		pEffectBase->m_nEffectDirection = (TALENT_EFFECT_DIRECTION)nDirType;
	}
}

void XTalentEffectInfoParser::ParseTalentEffect(MXmlElement* pElement, MXml* pXml, CSTalentInfoMgr* pTalentInfoMgr)
{
	CSTalentInfoParserHelper helper;
	CSTalentInfoParserHelper::TalentInfoID idInfo = helper.ParseTalentID(pXml, pElement);

	XTalentInfo* pTalentInfo = static_cast<XTalentInfo*>(pTalentInfoMgr->Find(idInfo.nID, idInfo.nMode, true));
	if (pTalentInfo == NULL)
	{
		return;
	}

	XTalentEffectDataMgr* pDataMgr = pTalentInfo->GetEffectDataMgr();

	_Attribute(&(pDataMgr->m_nRefID),	pElement, TALENT_XML_ATTR_REFERENCE_ID);
	if (pDataMgr->m_nRefID >0 )
	{
		tstring strValue;
		if (_Attribute(strValue, pElement, TALENT_XML_ATTR_REFERENCE_MODE))
		{
			pDataMgr->m_nRefMode = helper.MakeModeFromXmlValue(strValue);
		}
	}
	else
	{
		// 속성
		MXmlElement* pAttElement = pElement->FirstChildElement();
		for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
		{
			int nValue = 0;
			tstring strValue;
			XTalentEffectInfo *pNewInfo = new XTalentEffectInfo;

			if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_CAST_EFFECT))
				pNewInfo->m_nEffectType = CAST_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_USE_EFFECT))
				pNewInfo->m_nEffectType = USE_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_ATTACK_EFFECT))
				pNewInfo->m_nEffectType = ATTACK_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_ATTACK_SECONDARY_EFFECT))
				pNewInfo->m_nEffectType = ATTACK_SECONDARY_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_MISS_EFFECT))
				pNewInfo->m_nEffectType = MISS_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_SHOCK_EFFECT))
				pNewInfo->m_nEffectType = SHOCK_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_DURATION_EFFECT))
				pNewInfo->m_nEffectType = DURATION_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_DEFENSE_EFFECT))
				pNewInfo->m_nEffectType = DEFENSE_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_CAST_END_EFFECT))
				pNewInfo->m_nEffectType = CAST_END_EFFECT;
			else if(!_stricmp(pAttElement->Value(), TALENT_XML_TAG_TALENT_HIT_EFFECT_POSDIR))
			{
				pNewInfo->m_nEffectType = HIT_EFFECT_POSDIR;
				ParseTalentHitEffectPosDir(pAttElement, pXml, pNewInfo);
				pDataMgr->push_back(pNewInfo);
				continue;
			}
			else
			{
				// 모르는 내용
				SAFE_DELETE(pNewInfo);
				dlog("알수 없는 이펙트 타입(%d)\n", pTalentInfo->m_nID);
				continue;
			}

			_Attribute(pNewInfo->m_strTalentEffectInfoName,	pAttElement, TALENT_XML_ATTR_TALENT_EFFECTINFONAME, pXml);
			_Attribute(pNewInfo->m_strEffectName,			pAttElement, TALENT_XML_ATTR_TALENT_EFFECTNAME);

			_Attribute(&pNewInfo->m_bScale,					pAttElement, TALENT_XML_ATTR_TALENT_SCALE);

			_Attribute(pNewInfo->m_strBoneName,				pAttElement, TALENT_XML_ATTR_TALENT_BONENAME);
			_Attribute(pNewInfo->m_strParameter,			pAttElement, TALENT_XML_ATTR_TALENT_PARAMETER);

			_Attribute(&nValue,								pAttElement, TALENT_XML_ATTR_TALENT_IMPACTPOSITION);

			pNewInfo->m_nImpactPos = (IMPACTPOS)nValue;

			int nDirType = 1;
			_Attribute(&nDirType, pAttElement, TALENT_XML_ATTR_TALENT_EFFECTDIR);
			pNewInfo->m_nEffectDirection = (TALENT_EFFECT_DIRECTION)nDirType;

			pDataMgr->push_back(pNewInfo);
		}
	}
}

void XTalentEffectInfoParser::ParseTalentHitEffectPosDir( MXmlElement* pElement, MXml* pXml, XTalentEffectInfo* pTalentInfo )
{
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		XTalentHitEffectPosDir dataDir;
		_Attribute(dataDir.strMeshName,						pAttElement, TALENT_XML_ATTR_TALENT_HIT_MESH_NAME);
		_Attribute(&dataDir.fHitTime,						pAttElement, TALENT_XML_ATTR_TALENT_HIT_TIME);

		string strMat;
		_Attribute(strMat,									pAttElement, TALENT_XML_ATTR_TALENT_HIT_POS, pXml);

		sscanf_s(strMat.c_str(),"%f %f %f", 
			&dataDir.vHitEffectPos.x, &dataDir.vHitEffectPos.y, &dataDir.vHitEffectPos.z);

		_Attribute(strMat,									pAttElement, TALENT_XML_ATTR_TALENT_HIT_UP_VEC, pXml);

		sscanf_s(strMat.c_str(),"%f %f %f", 
			&dataDir.vSwordTrailHitUp.x, &dataDir.vSwordTrailHitUp.y, &dataDir.vSwordTrailHitUp.z);

		pTalentInfo->m_vecTalentHitEffectPosDir.push_back(dataDir);
	}
}
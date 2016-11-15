#include "stdafx.h"
#include "CSHitInfoLoader.h"
#include "CSTalentInfoDef_Hit.h"
#include "CSTalentInfoMgr.h"
#include "CSTalentInfoParser.h"
#include "MLocale.h"


bool CSHitInfoLoader::LoadHitInfo( CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName )
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_HIT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_HIT_XML_TAG_TALENT))
		{
			ParseHitInfo(pTalentInfoMgr, &xml, pElement);

		}
	}

	return true;
}

void CSHitInfoLoader::ParseHitInfo( CSTalentInfoMgr* pTalentInfoMgr, MXml* pXml, MXmlElement* pElement)
{
	CSTalentHitInfo hitInfo;

	CSTalentInfoParserHelper helper;
	CSTalentInfoParserHelper::TalentInfoID idInfo = helper.ParseTalentID(pXml, pElement);

	hitInfo.m_nID = idInfo.nID;

	int nReferenceTalentID = 0;
	int nReferenceTalentMode = 0;

	if (_Attribute(&nReferenceTalentID,		pElement, TALENT_HIT_XML_ATTR_REF_ID))
	{
		tstring strValue;
		if (_Attribute(strValue, pElement, TALENT_HIT_XML_ATTR_REF_MODE))
		{
			nReferenceTalentMode = helper.MakeModeFromXmlValue(strValue);
		}

		hitInfo.m_nReferenceID = nReferenceTalentID;
		hitInfo.m_nReferenceMode = nReferenceTalentMode;
	}

	if (hitInfo.m_nReferenceID == 0)
	{
		MXmlElement *pSegment = NULL;

		if ( pSegment = pElement->FirstChildElement(TALENT_HIT_XML_HITSEGMENT))
		{
			for( pSegment; pSegment != NULL; pSegment=pSegment->NextSiblingElement() )
			{
				if (_stricmp(pSegment->Value(), TALENT_HIT_XML_HITSEGMENT)) continue;

				CSHitSegment hs;

				_Attribute(&hs.m_fCheckTime,		pSegment, TALENT_HIT_XML_ATTR_CHECKTIME);
				_Attribute(&hs.m_nAddtiveDamage,	pSegment, TALENT_HIT_XML_ATTR_ADDTIVEDAMAGE);
				_Attribute(&hs.m_fHitMotionDelay,	pSegment, TALENT_HIT_XML_ATTR_HITMOTIONDELAY);

				MXmlElement *pCapsule = NULL;

				if ( pCapsule = pSegment->FirstChildElement(TALENT_HIT_XML_CAPSULE))
				{
					for( pCapsule; pCapsule != NULL; pCapsule=pCapsule->NextSiblingElement() )
					{
						if (_stricmp(pCapsule->Value(), TALENT_HIT_XML_CAPSULE)) continue;

						bool bDoHitCheck = true;
						_Attribute(&bDoHitCheck, pCapsule, TALENT_HIT_XML_ATTR_DOHITCHECK, true);

						MCapsule capsule;

						MVector3 b, t;
						float r;
						char szBuffer[256] = "";
						_Contents(szBuffer, pCapsule);

						sscanf_s(szBuffer,"%f %f %f %f %f %f %f", &b.x, &b.y, &b.z, &t.x, &t.y, &t.z, &r);

						capsule = MCapsule(b, t, r);
						hs.m_vCapsules.push_back(capsule);
						hs.m_vDoHitCheck.push_back(bDoHitCheck);
					}
				}

				hitInfo.m_vSegments.push_back(hs);
			}
		}
	}

	if (idInfo.nID > 0)
	{
		CSTalentInfo* pTalentInfo = pTalentInfoMgr->Find(idInfo.nID, idInfo.nMode, true);
		if (pTalentInfo != NULL)
		{
			pTalentInfo->m_HitInfo = hitInfo;
		}
	}
}


#include "stdafx.h"
#include "CSHitInfoSaver.h"
#include "CSTalentInfo.h"
#include "CSTalentInfoMgr.h"
#include "CSTalentInfoDef_Hit.h"
#include "CSTalentInfoParser.h"
#include "MLocale.h"

bool CSHitInfoSaver::SaveHitInfo( CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName )
{
	pTalentInfoMgr->_SortHitInfos();

	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);


	MXmlElement* pMaietElement = new MXmlElement(TALENT_HIT_XML_TAG_MAIET);

	for (CSTalentInfoMgr::iterator i = pTalentInfoMgr->begin(); i != pTalentInfoMgr->end(); ++i)
	{
		CSTalentInfo* pTalentInfo = i->second;
		SetTalentElement(pTalentInfo, pMaietElement);

		if (pTalentInfo->ExistMode())
		{
			for (int i = 0; i < MAX_TALENT_MODE; i++)
			{
				CSTalentInfo* pModeTalentInfo = pTalentInfo->GetMode(i);
				if (pModeTalentInfo == NULL) continue;

				SetTalentElement(pModeTalentInfo, pMaietElement);
			}
		}
	}

	root->LinkEndChild(pMaietElement);


	return xml.SaveFile(MLocale::ConvTCHARToAnsi(szFileName).c_str());	
}

void CSHitInfoSaver::SetTalentElement( CSTalentInfo* pTalentInfo, MXmlElement* pMaietElement )
{
	CSTalentHitInfo& s = pTalentInfo->m_HitInfo;

	MXmlElement* pTalentElement = new MXmlElement(TALENT_HIT_XML_TAG_TALENT);	

	CSTalentInfoParserHelper helper;
	helper.SetXmlElement_TalentIDAndMode(pTalentElement, pTalentInfo->m_nID, pTalentInfo->m_nMode);

	if (s.m_nReferenceID > 0)
	{
		_SetAttribute(pTalentElement, TALENT_HIT_XML_ATTR_REF_ID,	s.m_nReferenceID);

		if (s.m_nReferenceMode > 0)
		{
			tstring strValue;	
			strValue = helper.MakeXmlValueFromMode(s.m_nReferenceMode);

			_SetAttribute(pTalentElement, TALENT_HIT_XML_ATTR_REF_MODE,	MLocale::ConvTCHARToAnsi(strValue.c_str()));
		}
	}
	else
	{
		for (size_t j=0; j<s.m_vSegments.size(); j++)
		{
			CSHitSegment& seg = s.m_vSegments[j];
			MXmlElement *pSegment = new MXmlElement(TALENT_HIT_XML_HITSEGMENT);	

			_SetAttribute(pSegment, TALENT_HIT_XML_ATTR_CHECKTIME,	seg.m_fCheckTime);
			_SetAttribute(pSegment, TALENT_HIT_XML_ATTR_ADDTIVEDAMAGE,	seg.m_nAddtiveDamage);
			_SetAttribute(pSegment, TALENT_HIT_XML_ATTR_HITMOTIONDELAY,	seg.m_fHitMotionDelay);

			for (size_t k=0; k<s.m_vSegments[j].m_vCapsules.size(); k++)
			{
				MXmlElement *pCapsule = new MXmlElement(TALENT_HIT_XML_CAPSULE);

				if (seg.m_vDoHitCheck[k] == false)
				{
					_SetAttribute(pCapsule, TALENT_HIT_XML_ATTR_DOHITCHECK,	seg.m_vDoHitCheck[k]);// ? "True" : "False");
				}

				char szBuffer[128];
				szBuffer[0] = '\0';
				sprintf_s(szBuffer,"%f %f %f %f %f %f %f", 
					seg.m_vCapsules[k].bottom.x, seg.m_vCapsules[k].bottom.y, seg.m_vCapsules[k].bottom.z,
					seg.m_vCapsules[k].top.x, seg.m_vCapsules[k].top.y, seg.m_vCapsules[k].top.z, 
					seg.m_vCapsules[k].radius);
				_SetContents(pCapsule, szBuffer);

				pSegment->LinkEndChild(pCapsule);
			}
			pTalentElement->LinkEndChild(pSegment);
		}
	}

	pMaietElement->LinkEndChild(pTalentElement);
}
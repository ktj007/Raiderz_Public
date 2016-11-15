#include "stdafx.h"
#include "CSTalentPosInfoSaver.h"
#include "CSTalentInfoDef.h"
#include "CSTalentInfoMgr.h"
#include "MLocale.h"

bool CSTalentPosInfoSaver::Save( CSTalentInfoMgr* pTalentInfoMgr, const TCHAR* szFileName )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	root->LinkEndChild(pDec);

	MXmlElement* pMaiet = new MXmlElement(TALENT_XML_TAG_MAIET);

	for (CSTalentInfoMgr::iterator itor = pTalentInfoMgr->begin(); itor != pTalentInfoMgr->end(); itor++)
	{
		CSTalentInfo* pTalentInfo = static_cast<CSTalentInfo*>(itor->second);
		if(pTalentInfo->m_MovingInfoList.size() == 0) continue;

		MXmlElement* pTalentInfoElement = new MXmlElement(TALENT_XML_TAG_TALENT_INFO);	

		_SetAttribute(pTalentInfoElement, TALENT_XML_ATTR_ID,	pTalentInfo->m_nID);

		pMaiet->LinkEndChild(pTalentInfoElement);

		if ( pTalentInfo->m_vCastingPos != vec3::ZERO  ||  pTalentInfo->m_fCastingRot != 0.0f)
		{
			MXmlElement* pMovSeg = new MXmlElement(TALENT_XML_TAG_CASTING_MOVE_SEGMENT);
			_SetAttribute(pMovSeg, TALENT_XML_ATTR_TIME_INDEX,	0);

			pTalentInfoElement->LinkEndChild(pMovSeg);

			MXmlElement* pPos = new MXmlElement(TALENT_XML_TAG_POS);
			char szText[512]="";
			sprintf_s(szText, "%f %f %f", pTalentInfo->m_vCastingPos.x, pTalentInfo->m_vCastingPos.y, pTalentInfo->m_vCastingPos.z);
			_SetContents(pPos, szText);
			pMovSeg->LinkEndChild(pPos);

			MXmlElement* pRot = new MXmlElement(TALENT_XML_TAG_ROT);
			sprintf_s(szText, "%f", pTalentInfo->m_fCastingRot);
			_SetContents(pRot, szText);
			pMovSeg->LinkEndChild(pRot);
		}

		if ( pTalentInfo->m_vExtraPos != vec3::ZERO  ||  pTalentInfo->m_fExtraRot != 0.0f)
		{
			MXmlElement* pMovSeg = new MXmlElement(TALENT_XML_TAG_CASTINGLOOP_MOVE_SEGMENT);
			_SetAttribute(pMovSeg, TALENT_XML_ATTR_TIME_INDEX,	0);

			pTalentInfoElement->LinkEndChild(pMovSeg);

			MXmlElement* pPos = new MXmlElement(TALENT_XML_TAG_POS);
			char szText[512]="";
			sprintf_s(szText, "%f %f %f", pTalentInfo->m_vCastingLoopPos.x, pTalentInfo->m_vCastingLoopPos.y, pTalentInfo->m_vCastingLoopPos.z);
			_SetContents(pPos, szText);
			pMovSeg->LinkEndChild(pPos);

			MXmlElement* pRot = new MXmlElement(TALENT_XML_TAG_ROT);
			sprintf_s(szText, "%f", pTalentInfo->m_fCastingLoopRot);
			_SetContents(pRot, szText);
			pMovSeg->LinkEndChild(pRot);
		}

		for (size_t j = 0; j < pTalentInfo->m_MovingInfoList.size(); j++)
		{
			int nTimeIndex	= (int)j;
			vec3 vPos		= pTalentInfo->m_MovingInfoList[j].vPos;
			float fRot		= pTalentInfo->m_MovingInfoList[j].fRot;

			MXmlElement* pMovSeg = new MXmlElement(TALENT_XML_TAG_MOVE_SEGMENT);
			_SetAttribute(pMovSeg, TALENT_XML_ATTR_TIME_INDEX,	nTimeIndex);

			pTalentInfoElement->LinkEndChild(pMovSeg);

			MXmlElement* pPos = new MXmlElement(TALENT_XML_TAG_POS);
			char szText[512] = "";
			sprintf_s(szText, "%f %f %f", vPos.x, vPos.y, vPos.z);
			_SetContents(pPos, szText);
			pMovSeg->LinkEndChild(pPos);

			MXmlElement* pRot = new MXmlElement(TALENT_XML_TAG_ROT);
			sprintf_s(szText, "%f", fRot);
			_SetContents(pRot, szText);
			pMovSeg->LinkEndChild(pRot);
		}

		if ( pTalentInfo->m_vExtraPos != vec3::ZERO  ||  pTalentInfo->m_fExtraRot != 0.0f)
		{
			MXmlElement* pMovSeg = new MXmlElement(TALENT_XML_TAG_EXTRA_MOVE_SEGMENT);
			_SetAttribute(pMovSeg, TALENT_XML_ATTR_TIME_INDEX,	0);

			pTalentInfoElement->LinkEndChild(pMovSeg);

			MXmlElement* pPos = new MXmlElement(TALENT_XML_TAG_POS);
			char szText[512]="";
			sprintf_s(szText, "%f %f %f", pTalentInfo->m_vExtraPos.x, pTalentInfo->m_vExtraPos.y, pTalentInfo->m_vExtraPos.z);
			_SetContents(pPos, szText);
			pMovSeg->LinkEndChild(pPos);

			MXmlElement* pRot = new MXmlElement(TALENT_XML_TAG_ROT);
			sprintf_s(szText, "%f", pTalentInfo->m_fExtraRot);
			_SetContents(pRot, szText);
			pMovSeg->LinkEndChild(pRot);
		}
	}

	root->LinkEndChild(pMaiet);

	xml.SaveFile(MLocale::ConvTCHARToAnsi(szFileName).c_str());

	return true;
}
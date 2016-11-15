#include "stdafx.h"
#include "VEffectInfoLoader.h"
#include "CSTalentInfoDef.h"
#include "XTalentInfoDef.h"
#include "VTalentInfoMgr.h"
#include "CSBuffInfoDef.h"
#include "VBuffInfoMgr.h"

VEffectInfoLoader::VEffectInfoLoader()
{
	m_nEffectCount = 0;
	m_nTalentEffectCount = 0;
	m_nBuffEFfectCount = 0;
}

VEffectInfoLoader::~VEffectInfoLoader()
{

}

void VEffectInfoLoader::Init()
{
	// effect.xml
	m_nEffectCount = Load_Ready_Effect(string(V_FILENAME_EFFECT_INFO_TOOL));

	// talent_effect_info.xml
	m_nTalentEffectCount = Load_Ready_TalentEffect(string(V_FILENAME_TALENT_EFFECTINFO_TOOL));

	// buff_Effect_info.xml
	m_nBuffEFfectCount = Load_Ready_BuffEffect(string(V_FILENAME_BUFFEFFECTINFO_TOOL));
}

int VEffectInfoLoader::Load_Ready_Effect( string& strFileName )
{
	if(!m_xmlEffect.LoadFile(strFileName.c_str())) 
		return -1;

	int nCount = 0;
	//------------------------------------------------------------------------
	// 로딩
	MXmlHandle docHandle = m_xmlEffect.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(EFFECT_NORMALIZE_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), EFFECT_NORMALIZE_XML_TAG_EFFECT))
		{
			++nCount;
		}
	}

	return nCount;
}

int VEffectInfoLoader::Load_Ready_TalentEffect( string& strFileName )
{
	int nCount = 0;

	if(!m_xmlTalentEffect.LoadFile(strFileName.c_str())) 
		return -1;

	MXmlHandle docHandle = m_xmlTalentEffect.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_BASE))
		{
			++nCount;
		}
		else if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_EFFECT))
		{
			++nCount;
		}
	}

	return nCount;
}

int VEffectInfoLoader::Load_Ready_BuffEffect( string& strFileName )
{
	int nCount = 0;

	if (!m_xmlBuffEffect.LoadFile(strFileName.c_str())) 
		return -1;

	MXmlHandle docHandle = m_xmlBuffEffect.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_BUFF_EFFECT))
		{
			++nCount;
		}
	}

	return nCount;
}

void VEffectInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	LoadEffect(worker, pValidatorInfoMgr->m_pEffectInfoMgr, nFullCompleteCount, nCurrCompleteCount, nMsgIndex);
	LoadTalentEffect(worker, pValidatorInfoMgr->m_pTalentInfoMgr, nFullCompleteCount, nCurrCompleteCount, nMsgIndex);
	LoadBuffEffect(worker, pValidatorInfoMgr->m_pBuffInfoMgr, nFullCompleteCount, nCurrCompleteCount, nMsgIndex);
}

bool VEffectInfoLoader::LoadEffect( BackgroundWorker^ worker, VEffectInfoMgr* pEffectInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	pEffectInfoMgr->LoadingFileInfo();

	// 에러 체크
	if(m_nEffectCount == -1)
	{
		CheckExistXmlFile(V_FILENAME_EFFECT_INFO_TOOL, &(pEffectInfoMgr->m_ErrorMsg));
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlEffect.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(EFFECT_NORMALIZE_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), EFFECT_NORMALIZE_XML_TAG_EFFECT))
		{
			pEffectInfoMgr->ParseEffectInfo(pElement, &m_xmlEffect);

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nEffectInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nEffectCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Effect Info Loading... {0}%", nEffectInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Effect Info Complete."));

	return true;
}

bool VEffectInfoLoader::LoadTalentEffect( BackgroundWorker^ worker, VTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	// 에러 체크
	if(m_nTalentEffectCount == -1)
	{
		CheckExistXmlFile(V_FILENAME_TALENT_EFFECTINFO_TOOL, &(pTalentInfoMgr->m_ErrorMsg));
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlTalentEffect.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		bool bSet = false;
		if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_BASE))
		{
			XTalentEffectInfoParser::ParseTalentBase(pElement, &m_xmlTalentEffect, pTalentInfoMgr->m_mapEffectBase);

			bSet = true;
		}
		else if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT_EFFECT))
		{
			XTalentEffectInfoParser::ParseTalentEffect(pElement, &m_xmlTalentEffect, &(pTalentInfoMgr->m_TalentInfoMgr));

			bSet = true;
		}

		if(bSet)
		{
			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nTalentEffectInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nTalentEffectCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Talent Effect Info Loading... {0}%", nTalentEffectInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Talent Effect Info Complete."));

	return true;
}

bool VEffectInfoLoader::LoadBuffEffect( BackgroundWorker^ worker, VBuffInfoMgr* pBuffInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	// 에러 체크
	if(m_nBuffEFfectCount == -1)
	{
		CheckExistXmlFile(V_FILENAME_BUFFEFFECTINFO_TOOL, &(pBuffInfoMgr->m_ErrorMsg));
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlBuffEffect.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_BUFF_EFFECT))
		{
			pBuffInfoMgr->ParseBuffEffectInfo(pElement, &m_xmlBuffEffect);

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nBuffEffectInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nBuffEFfectCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Buff Effect Info Loading... {0}%", nBuffEffectInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Buff Effect Info Complete."));

	return true;
}
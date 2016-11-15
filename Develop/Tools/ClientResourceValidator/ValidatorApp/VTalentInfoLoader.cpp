#include "stdafx.h"
#include "VTalentInfoLoader.h"
#include "XTalentInfoParser.h"
#include "XTalentInfoExtParser.h"
#include "VTalentInfoMgr.h"
#include "CSHitInfoLoader.h"
#include "CSTalentInfoDef_Hit.h"

VTalentInfoLoader::VTalentInfoLoader()
: XTalentInfoLoader()
{
	m_nTalentCount			= 0;
	m_nTalentExtCount		= 0;
	m_nTalentHitInfoCount	= 0;
}

void VTalentInfoLoader::Init()
{ 
	m_nTalentCount			= Load_Ready_Talent(string(V_FILENAME_TALENT_TOOL));
	m_nTalentExtCount		= Load_Ready_TalentExt(string(V_FILENAME_TALENT_EXT_TOOL));
	m_nTalentHitInfoCount	= Load_Ready_TalentHitInfo(string(V_FILENAME_TALENT_HIT_INFO));
}

void VTalentInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	if (m_nTalentCount == -1)
		CheckExistXmlFile(V_FILENAME_TALENT_TOOL, &(pValidatorInfoMgr->m_pTalentInfoMgr->m_ErrorMsg));
	else
		LoadTalent(worker, &(pValidatorInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr), nFullCompleteCount, nCurrCompleteCount, nMsgIndex);

	if(m_nTalentExtCount == -1)
		CheckExistXmlFile(V_FILENAME_TALENT_EXT_TOOL, &(pValidatorInfoMgr->m_pTalentInfoMgr->m_ErrorMsg));
	else
		LoadExt(worker, &(pValidatorInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr), nFullCompleteCount, nCurrCompleteCount, nMsgIndex);

	if(m_nTalentHitInfoCount == -1)
		CheckExistXmlFile(V_FILENAME_TALENT_HIT_INFO, &(pValidatorInfoMgr->m_pTalentInfoMgr->m_ErrorMsg));
	else
		LoadHitInfo(worker, &(pValidatorInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr), nFullCompleteCount, nCurrCompleteCount, nMsgIndex);;
}

bool VTalentInfoLoader::LoadTalent( BackgroundWorker^ worker, XTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	XTalentInfoParser talentInfoParser;
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlTalent.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			talentInfoParser.ParseTalent(pTalentInfoMgr, pElement, &m_xmlTalent);

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nTalentInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nTalentCount);
			
			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Talent Info Loading... {0}%", nTalentInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Talent Ext Info Complete."));

	return true;
}

bool VTalentInfoLoader::LoadExt( BackgroundWorker^ worker, XTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	XTalentInfoExtParser talentInfoExtParser;
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlTalentExt.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			talentInfoExtParser.Parse(pTalentInfoMgr, &m_xmlTalentExt, pElement);

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nTalentExtInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nTalentExtCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Talent Ext Info Loading... {0}%", nTalentExtInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Talent Ext Info Complete."));

	// 레퍼런스 데이터를 복사한다.
	talentInfoExtParser.CopyTalentEventInfoByReference(pTalentInfoMgr);

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Talent Ext Info Reference Copy Complete."));

	return true;
}


bool VTalentInfoLoader::LoadHitInfo( BackgroundWorker^ worker, CSTalentInfoMgr* pTalentInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	CSHitInfoLoader hitInfoLoader;
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlTalentHitInfo.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_HIT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_HIT_XML_TAG_TALENT))
		{
			hitInfoLoader.ParseHitInfo(pTalentInfoMgr, &m_xmlTalentHitInfo, pElement);

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nTalentHitInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nTalentHitInfoCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Talent Hit Info Loading... {0}%", nTalentHitInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Talent Hit Info Complete."));

	return true;
}

int VTalentInfoLoader::Load_Ready_Talent( string& strFileName )
{
	int nCount = 0;
	if (!m_xmlTalent.LoadFile(strFileName.c_str())) 
		return -1;

	MXmlHandle docHandle = m_xmlTalent.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			++nCount;
		}
	}

	return nCount;
}

int VTalentInfoLoader::Load_Ready_TalentExt( string& strFileName )
{
	int nCount = 0;
	if(!m_xmlTalentExt.LoadFile(strFileName.c_str()))
		return -1;

	MXmlHandle docHandle = m_xmlTalentExt.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), TALENT_XML_TAG_TALENT))
		{
			++nCount;
		}
	}

	return nCount;
}

int VTalentInfoLoader::Load_Ready_TalentHitInfo( string& strFileName )
{
	int nCount = 0;

	if (!m_xmlTalentHitInfo.LoadFile(strFileName.c_str())) 
		return -1;

	MXmlHandle docHandle = m_xmlTalentHitInfo.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_HIT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_HIT_XML_TAG_TALENT))
		{
			++nCount;
		}
	}

	return nCount;
}

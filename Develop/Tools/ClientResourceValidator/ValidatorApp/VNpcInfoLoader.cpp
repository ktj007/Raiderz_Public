#include "stdafx.h"
#include "VNpcInfoLoader.h"
#include "VNpcInfoMgr.h"

VNpcInfoLoader::VNpcInfoLoader()
{
	m_nNpcCount = 0;
}

VNpcInfoLoader::~VNpcInfoLoader()
{

}

void VNpcInfoLoader::Init()
{
	if ( m_xmlNpcInfo.LoadFile(V_FILENAME_NPC_TOOL) == false)
	{
		m_nNpcCount = -1;
		return;
	}

	MXmlHandle docHandle = m_xmlNpcInfo.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( "maiet").FirstChildElement().Element();

	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( _stricmp( pElement->Value(), NPC_XML_TAG_NPC) == 0)
		{
			++m_nNpcCount;
		}
	}
}

void VNpcInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	VNpcInfoMgr * pNpcInfoMgr = pValidatorInfoMgr->m_pNpcInfoMgr;

	// 에러 체크
	if(m_nNpcCount == -1)
	{
		CheckExistXmlFile(V_FILENAME_NPC_TOOL, &(pNpcInfoMgr->m_ErrorMsg));
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlNpcInfo.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(EFFECT_NORMALIZE_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if (!_stricmp( pElement->Value(), NPC_XML_TAG_NPC))
		{
			pNpcInfoMgr->ParseNPCInfo(pElement, &m_xmlNpcInfo);

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nNpcInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nNpcCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Npc Info Loading... {0}%", nNpcInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Npc Info Complete."));

	return;
}

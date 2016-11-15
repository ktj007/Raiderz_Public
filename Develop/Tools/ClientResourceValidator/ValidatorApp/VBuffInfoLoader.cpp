#include "stdafx.h"
#include "VBuffInfoLoader.h"
#include "CSBuffInfoDef.h"
#include "VBuffInfoMgr.h"

VBuffInfoLoader::VBuffInfoLoader()
{
	m_nBuffCount = 0;
}

VBuffInfoLoader::~VBuffInfoLoader()
{

}

void VBuffInfoLoader::Init()
{
	if (!m_xmlBuff.LoadFile(V_FILENAME_BUFFINFO_TOOL)) 
	{
		m_nBuffCount = -1;
		return;
	}

	MXmlHandle docHandle = m_xmlBuff.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_BUFF))
		{
			++m_nBuffCount;
		}
	}
}

void VBuffInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	if (m_nBuffCount == -1)
	{
		CheckExistXmlFile(V_FILENAME_BUFFINFO_TOOL, &(pValidatorInfoMgr->m_pBuffInfoMgr->m_ErrorMsg));
		return;
	}

	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlBuff.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(BUFF_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), BUFF_XML_TAG_BUFF))
		{
			pValidatorInfoMgr->m_pBuffInfoMgr->ParseBuffInfo(pElement, &m_xmlBuff);

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nBuffInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nBuffCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Buff Info Loading... {0}%", nBuffInfoCompletePercent));
		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Buff Info Complete."));
}

#include "stdafx.h"
#include "VItemInfoLoader.h"
#include "VItemInfoMgr.h"

VItemInfoLoader::VItemInfoLoader()
{
	m_nItemCount = 0;
}

VItemInfoLoader::~VItemInfoLoader()
{

}

void VItemInfoLoader::Init()
{
	if ( m_xmlItem.LoadFile(V_FILENAME_XITEM_TOOL) == false)
	{
		m_nItemCount = -1;
		return;
	}

	MXmlHandle docHandle = m_xmlItem.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( "maiet").FirstChildElement().Element();

	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), "ITEM"))
		{
			++m_nItemCount;
		}
	}
}

void VItemInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	//////////////////////////////////////////////////////////////////////////
	if(m_nItemCount == -1)
	{
		CheckExistXmlFile(V_FILENAME_XITEM_TOOL, &(pValidatorInfoMgr->m_pSoundInfoMgr->m_ErrorMsg));
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	int nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	MXmlHandle docHandle = m_xmlItem.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild("maiet").FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), "ITEM"))
		{
			CSItemData* pNewItemData = pValidatorInfoMgr->m_pItemInfoMgr->NewItemData();
			if(false == pValidatorInfoMgr->m_pItemInfoMgr->ParseItem(pElement, pNewItemData, &m_xmlItem))
			{
				SAFE_DELETE(pNewItemData);
			}

			++nCompleteCount;
			++nCurrCompleteCount;

			nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
			int nItemInfoCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_nItemCount);

			ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Item Info Loading... {0}%", nItemInfoCompletePercent));

		}
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Item Info Complete."));

	return;
}

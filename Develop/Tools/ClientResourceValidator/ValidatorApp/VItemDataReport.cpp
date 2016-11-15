#include "stdafx.h"
#include "VItemDataReport.h"

VItemDataReport::VItemDataReport()
:VDataReport()
{

}

void VItemDataReport::Start( VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	int	nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	for(XItemDataMap::iterator it = vInfoMgr->m_pItemInfoMgr->m_ItemDataMap.begin(); it != vInfoMgr->m_pItemInfoMgr->m_ItemDataMap.end(); ++it)
	{
		XItemData* pItemData = it->second;

		m_ItemDataValidator.CheckItemModel(vInfoMgr->m_pMeshInfoMgr, pItemData);

		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nItemInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, GetCheckCount());

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Item Info Validate Checking... {0}%", nItemInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Item Info Validate Check Complete."));
}

void VItemDataReport::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_ItemDataValidator.Init(vInfoMgr);
}

int VItemDataReport::GetCheckCount()
{
	return m_ItemDataValidator.GetCheckCount();
}

VErrorMsg& VItemDataReport::GetItemInfoErrorMsg()
{
	return m_ItemDataValidator.m_ItemValidatorError;
}

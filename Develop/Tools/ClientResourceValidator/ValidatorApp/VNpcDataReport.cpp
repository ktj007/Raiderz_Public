#include "stdafx.h"
#include "VNpcDataReport.h"

VNpcDataReport::VNpcDataReport()
:VDataReport()
{

}

void VNpcDataReport::Start( VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	int	nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	for(map<int, CSNPCInfo*>::iterator it = vInfoMgr->m_pNpcInfoMgr->begin(); it != vInfoMgr->m_pNpcInfoMgr->end(); ++it)
	{
		XNPCInfo* pNpcInfo = vInfoMgr->m_pNpcInfoMgr->Get(it->first);		//// animation.xml

		m_NpcDataValidator.CheckNpcInfo(pNpcInfo, vInfoMgr);

		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nNpcInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, GetCheckCount());

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Npc Info Validate Checking... {0}%", nNpcInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Npc Info Validate Check Complete."));
}

void VNpcDataReport::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_NpcDataValidator.Init(vInfoMgr);
}

int VNpcDataReport::GetCheckCount()
{
	return m_NpcDataValidator.GetCheckCount();
}

VErrorMsg& VNpcDataReport::GetNpcInfoErrorMsg()
{
	return m_NpcDataValidator.m_NpcMeshValidatorError;
}
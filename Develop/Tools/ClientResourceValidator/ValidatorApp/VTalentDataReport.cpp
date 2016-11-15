#include "stdafx.h"
#include "VTalentDataReport.h"

VTalentDataReport::VTalentDataReport()
:VDataReport()
{

}

void VTalentDataReport::Start( VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	int	nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	for(map<int , CSTalentInfo* >::iterator it = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.begin(); it != vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.end(); ++it)
	{
		XTalentInfo* pTalentInfo = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.Get(it->second->m_nID);

		// talent.xml
		m_TalentDataValidator.CheckTalentInfoXml();

		// talent_ext.xml
		m_TalentDataValidator.CheckTalentEvent(&vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr, pTalentInfo, vInfoMgr);


		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nTalentInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, GetCheckCount());

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Talent Info Validate Checking... {0}%", nTalentInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Talent Info Validate Check Complete."));
}

void VTalentDataReport::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_TalentDataValidator.Init(vInfoMgr);
}

int VTalentDataReport::GetCheckCount()
{
	return m_TalentDataValidator.GetCheckCount();
}

VErrorMsg& VTalentDataReport::GetTalentEventErrorMsg()
{
	return m_TalentDataValidator.m_TalentEventValidatorError;
}
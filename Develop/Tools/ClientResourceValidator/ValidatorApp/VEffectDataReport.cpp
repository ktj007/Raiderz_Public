#include "stdafx.h"
#include "VEffectDataReport.h"

VEffectDataReport::VEffectDataReport()
:VDataReport()
{

}

void VEffectDataReport::Start( VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	int	nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	//////////////////////////////////////////////////////////////////////////
	// effect info
	for(vector<XEffectInfo *>::iterator itInfo = vInfoMgr->m_pEffectInfoMgr->m_vecEffectInfo.begin(); itInfo != vInfoMgr->m_pEffectInfoMgr->m_vecEffectInfo.end(); ++itInfo)
	{
		m_EffectDataValidator.CheckEffectInfo(*itInfo, vInfoMgr);

		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nEffectInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_EffectDataValidator.m_nEffectInfoCount);

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Effect Info Validate Checking... {0}%", nEffectInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Effect Info Validate Check Complete."));

	//////////////////////////////////////////////////////////////////////////
	// talent info
	nCompleteCount = 0;
	for(map<int , CSTalentInfo* >::iterator itTalentInfo = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.begin(); itTalentInfo != vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.end(); ++itTalentInfo)
	{
		XTalentInfo* pTalentInfo = vInfoMgr->m_pTalentInfoMgr->m_TalentInfoMgr.Get(itTalentInfo->second->m_nID);

		m_EffectDataValidator.CheckTalentEffectInfo(pTalentInfo, vInfoMgr);

		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nTalentEffectInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_EffectDataValidator.m_nTalentEffectInfoCount);

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Talent Effect Info Validate Checking... {0}%", nTalentEffectInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Talent Effect Info Validate Check Complete."));

	//////////////////////////////////////////////////////////////////////////
	// buff info
	nCompleteCount = 0;
	for(map<int , CSBuffInfo* >::iterator itBuffInfo = vInfoMgr->m_pBuffInfoMgr->begin(); itBuffInfo != vInfoMgr->m_pBuffInfoMgr->end(); ++itBuffInfo)
	{
		XBuffInfo* pBuffInfo = vInfoMgr->m_pBuffInfoMgr->Get(itBuffInfo->second->m_nID);

		m_EffectDataValidator.CheckBuffEffectInfo(pBuffInfo, vInfoMgr);
	
		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nBuffEffectInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, m_EffectDataValidator.m_nBuffEffectInfoCount);

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Buff Effect Info Validate Checking... {0}%", nBuffEffectInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Buff Effect Info Validate Check Complete."));
}

void VEffectDataReport::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_EffectDataValidator.Init(vInfoMgr);
}

int VEffectDataReport::GetCheckCount()
{
	return m_EffectDataValidator.GetCheckCount();
}

VErrorMsg& VEffectDataReport::GetEffectInfoErrorMsg()
{
	return m_EffectDataValidator.m_EffectInfoValidatorError;
}

VErrorMsg& VEffectDataReport::GetTalentEffectInfoErrorMsg()
{
	return m_EffectDataValidator.m_TalentEffectInfoValidatorError;
}

VErrorMsg& VEffectDataReport::GetBuffEffectInfoErrorMsg()
{
	return m_EffectDataValidator.m_BuffEffectInfoValidatorError;
}
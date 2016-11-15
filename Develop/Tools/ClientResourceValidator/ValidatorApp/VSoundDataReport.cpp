#include "stdafx.h"
#include "VSoundDataReport.h"

VSoundDataReport::VSoundDataReport()
:VDataReport()
{

}

void VSoundDataReport::Start( VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	int	nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	for(map< string, RSampleInfo>::iterator it = vInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.begin(); it != vInfoMgr->m_pSoundInfoMgr->m_mapSoundInfo.end(); ++it)
	{
		m_SoundDataValidator.CheckSoundInfo(vInfoMgr->m_pSoundInfoMgr, (string)it->first, &(it->second), vInfoMgr);

		m_SoundDataValidator.CheckSoundUsable((string)it->first, vInfoMgr);

		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nSoundInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, GetCheckCount());

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Sound Info Validate Checking... {0}%", nSoundInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Sound Info Validate Check Complete."));
}

void VSoundDataReport::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_SoundDataValidator.Init(vInfoMgr);
}

int VSoundDataReport::GetCheckCount()
{
	return m_SoundDataValidator.GetCheckCount();
}

VErrorMsg& VSoundDataReport::GetSoundInfoErrorMsg()
{
	return m_SoundDataValidator.m_SoundInfoValidatorError;
}

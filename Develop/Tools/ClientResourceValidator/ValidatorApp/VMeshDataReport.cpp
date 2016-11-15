#include "stdafx.h"
#include "VMeshDataReport.h"

VMeshDataReport::VMeshDataReport()
:VDataReport()
{

}

void VMeshDataReport::Start( VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	int	nCompleteCount = 0;
	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	for(map<string, VMeshInfo*>::iterator it = vInfoMgr->m_pMeshInfoMgr->m_mapMeshInfo.begin(); it != vInfoMgr->m_pMeshInfoMgr->m_mapMeshInfo.end(); ++it)
	{
		// animation.xml
		m_MeshDataValidator.CheckAnimationXMl(it->second);

		// animationevent.xml
		m_MeshDataValidator.CheckAnimationEvent(it->second, vInfoMgr);

		// animationinfo.xml
		m_MeshDataValidator.CheckAnimationInfo(it->second);

		//////////////////////////////////////////////////////////////////////////
		++nCompleteCount;
		++nCurrCompleteCount;

		nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
		int nMeshInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, GetCheckCount());

		ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Mesh Info Validate Checking... {0}%", nMeshInfoCheckCompletePercent));
	}

	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Mesh Info Validate Check Complete."));
}

void VMeshDataReport::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_MeshDataValidator.Init(vInfoMgr);
}

int VMeshDataReport::GetCheckCount()
{
	return m_MeshDataValidator.GetCheckCount();
}

VErrorMsg& VMeshDataReport::GetAnimationListErrorMsg()
{
	return m_MeshDataValidator.m_AnimationListValidatorError;
}

VErrorMsg& VMeshDataReport::GetAnimationEventErrorMsg()
{
	return m_MeshDataValidator.m_AnimationEventValidatorError;
}

VErrorMsg& VMeshDataReport::GetAnimationInfoErrorMsg()
{
	return m_MeshDataValidator.m_AnimationInfoValidatorError;
}

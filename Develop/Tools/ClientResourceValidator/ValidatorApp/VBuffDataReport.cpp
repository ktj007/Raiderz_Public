#include "stdafx.h"
#include "VBuffDataReport.h"

VBuffDataReport::VBuffDataReport()
:VDataReport()
{

}

void VBuffDataReport::Start( VValidatorInfoMgr* vInfoMgr, BackgroundWorker^ worker, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	//int	nCompleteCount = 0;
	//int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);

	//for(map<string, VMeshInfo*>::iterator it = vInfoMgr->m_pMeshInfoMgr->m_mapMeshInfo.begin(); it != vInfoMgr->m_pMeshInfoMgr->m_mapMeshInfo.end(); ++it)
	//{
	//	// animation.xml
	//	m_MeshDataValidator.CheckAnimationXMl(it->second);

	//	// animationevent.xml
	//	m_MeshDataValidator.CheckAnimationEvent(it->second, vInfoMgr);

	//	// animationinfo.xml
	//	m_MeshDataValidator.CheckAnimationInfo(it->second);

	//	//////////////////////////////////////////////////////////////////////////
	//	++nCompleteCount;
	//	++nCurrCompleteCount;

	//	nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
	//	int nMeshInfoCheckCompletePercent = CalLoagingCompletePercent(nCompleteCount, GetCheckCount());

	//	ValidatorApp::ReportProgress(worker, nMsgIndex, nFullCompletePercent, String::Format("Mesh Info Validate Checking... {0}%", nMeshInfoCheckCompletePercent));
	//}

	//ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Mesh Info Validate Check Complete."));
}

void VBuffDataReport::Init( VValidatorInfoMgr* vInfoMgr )
{
	m_BuffDataValidator.Init(vInfoMgr);
}

int VBuffDataReport::GetCheckCount()
{
	return m_BuffDataValidator.GetCheckCount();
}

#include "stdafx.h"
#include "VCameraInfoLoader.h"
#include "VCameraInfoMgr.h"

VCameraInfoLoader::VCameraInfoLoader()
{
	m_nCameraCount = 0;
}

VCameraInfoLoader::~VCameraInfoLoader()
{

}

void VCameraInfoLoader::Init()
{

}

void VCameraInfoLoader::Load( BackgroundWorker^ worker, VValidatorInfoMgr * pValidatorInfoMgr, int nFullCompleteCount, int& nCurrCompleteCount, int& nMsgIndex )
{
	pValidatorInfoMgr->m_pCameraInfoMgr->LoadingInfo();

	int nFullCompletePercent = CalLoagingCompletePercent(nCurrCompleteCount, nFullCompleteCount);
	ValidatorApp::ReportProgress(worker, nMsgIndex++, nFullCompletePercent, String::Format("Camera Info Loading Complete"));
}

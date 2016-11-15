#include "stdafx.h"
#include "VGlobalApp.h"

void ValidatorApp::ReportProgress( BackgroundWorker^ worker, int nIndex, int nLoadingCompletePercent, String^ strText )
{
	CLoadingProgress^ loadingState = gcnew CLoadingProgress;

	if(nLoadingCompletePercent == 0)
	{
		int a = 0;
	}
	loadingState->m_nIndex = nIndex;
	loadingState->m_strText = strText;
	worker->ReportProgress(nLoadingCompletePercent, loadingState);
}
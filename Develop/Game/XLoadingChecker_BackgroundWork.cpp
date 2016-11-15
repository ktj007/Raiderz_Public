#include "stdafx.h"
#include "XLoadingChecker_BackgroundWork.h"


bool XLoadingChecker_BackgroundWork::CheckLoaded()
{
	RBackgroundWorker& bg_woker = REngine::GetBackgroundWorker();
	int nWorkerCount = bg_woker.GetRemainedWorkableTicketSize();

	if (nWorkerCount > 0)
	{
		return false;
	}


	return true;
}
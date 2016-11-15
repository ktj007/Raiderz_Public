#include "stdafx.h"
#include "GAIEventInvoker.h"
#include "GGlobal.h"
#include "GAIEventMgr.h"
#include "GAIProfiler.h"

void GAIEventInvoker::InvokeEvent(GAIEvent event)
{
	gmgr.pAIEventMgr->Notify(event);
}

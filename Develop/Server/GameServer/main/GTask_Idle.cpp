#include "stdafx.h"
#include "GTask_Idle.h"
#include "GEntityNPC.h"

GTask_Idle::GTask_Idle(GEntityNPC* pEntity, float fIdleTime) 
: GTask(pEntity)
{
	m_rgrWaiter.SetTickTime(fIdleTime);
}

GTask_Idle::~GTask_Idle()
{

}

bool GTask_Idle::OnStart()
{
	m_rgrWaiter.Start();
	return true;
}

GTaskResult GTask_Idle::OnRun(float fDelta)
{
	PFI_BLOCK_THISFUNC(1800);

	if (m_rgrWaiter.IsReady(fDelta))
		return TR_COMPLETED;

	return TR_RUNNING;
}


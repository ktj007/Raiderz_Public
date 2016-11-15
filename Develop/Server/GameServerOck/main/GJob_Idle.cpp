#include "stdafx.h"
#include "GJob_Idle.h"
#include "GModuleAI.h"

GJob_Idle::GJob_Idle(GModuleAI* pmAI, float fIdleTime) 
					: GJob(pmAI), m_fIdleTime(fIdleTime)
{
	
}

GJob_Idle::~GJob_Idle()
{

}

bool GJob_Idle::OnStart()
{
	SetRunningTask(m_TaskBroker.New_Idle(GetOwnerNPC(), m_fIdleTime));

	return true;
}

GJobResult GJob_Idle::OnRun(float fDelta)
{
	return JR_RUNNING;
}

string GJob_Idle::GetParamString() const
{
	char buff[512];
	sprintf(buff, "IdleTime: %.1f", 
		m_fIdleTime);
	return buff;
}


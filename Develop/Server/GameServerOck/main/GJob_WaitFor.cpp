#include "StdAfx.h"
#include "GJob_WaitFor.h"
#include "GEntityNPC.h"

GJob_WaitFor::GJob_WaitFor( GModuleAI* pmAI, MUID uidTarget )
: GJob(pmAI)
, m_uidTarget(uidTarget)
{

}
GJob_WaitFor::~GJob_WaitFor(void)
{
}

bool GJob_WaitFor::OnStart()
{
	SetRunningTask(m_TaskBroker.New_WaitFor(GetOwnerNPC(), m_uidTarget));

	return true;
}

GJobResult GJob_WaitFor::OnRun( float fDelta )
{
	return JR_RUNNING;
}

string GJob_WaitFor::GetParamString() const
{
	GEntityNPC* pTarget = GetOwnerNPC()->FindNPC(m_uidTarget);
	if (NULL == pTarget)	
		return "Target: invalid_target";

	char buff[512];
	sprintf(buff, "Target: %s", 
		MLocale::ConvUTF16ToAnsi(pTarget->GetName()).c_str());
	return buff;
}

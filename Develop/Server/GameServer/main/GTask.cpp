#include "stdafx.h"
#include "GTask.h"
#include "GJob.h"
#include "GAIProfiler.h"
#include "GJobMgr.h"

#include "GEntityNPC.h"	// 의존성 줄여야함
#include "GModuleAI.h"	// 의존성 줄여야함

GTask::GTask(GEntityNPC* pOwner) 
: m_pOwner(pOwner)
, m_pParent(NULL) 
{
	if (m_pOwner && m_pOwner->GetModuleAI())
	{
		GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
		m_pParent = pJobMgr->GetCurrJob();
	}
}

bool GTask::Start()
{
	AI_PFI_TASK_BLOCK(GetID(), GetTaskName());
	return OnStart();
}

GTaskResult GTask::Run(float fDelta)
{
	AI_PFI_TASK_BLOCK(GetID(), GetTaskName());
	return OnRun(fDelta);
}

void GTask::Complete()
{
	OnComplete();
}

void GTask::Cancel()
{
	OnCancel();
}

bool GTask::Reset( GTask* pNewTask )
{
	assert(pNewTask);
	if (!pNewTask)						return false;
	if (pNewTask->GetID() != GetID())
	{
		_ASSERT(0);
		return false;
	}

	return OnStart();
}
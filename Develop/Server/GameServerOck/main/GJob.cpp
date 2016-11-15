#include "stdafx.h"
#include "GJob.h"
#include "GModuleAI.h"
#include "GTask.h"
#include "GEntityNPC.h"
#include "GAIProfiler.h"

/////////////////////////////////////////////////////////////////////////
// GJob //////////////////////////////////////////////////////////////
GJob::GJob(GModuleAI* pOwner) 
: m_pOwner(pOwner)
, m_fDurationTime(JOB_DURATION_INFINITE_TIME)
, m_pRunningTask(NULL)
, m_bSuspended(false)
{
	if (pOwner)
	{
		m_uidOwner = pOwner->GetOwnerUID();
	}
}

GJob::~GJob() 
{
	if (m_pRunningTask)
	{
		SAFE_DELETE(m_pRunningTask);
	}
}

bool GJob::Start()
{
	AI_PFI_JOB_BLOCK(GetID(), this->GetJobName());

	blog("[JOB-start] %s\n", this->GetJobName());
	return OnStart();
}

GJobResult GJob::Run(float fDelta)
{
	GJobResult result = UpdateTask(fDelta);
	bool bExpired = UpdateDuration(fDelta);

	if ( bExpired == true)				
	{
		m_fDurationTime = JOB_DURATION_INFINITE_TIME;
		return JR_TIMEOUT;
	}
	else if ( result != JR_RUNNING)		
	{
		return result;
	}

	PFI_B(9066, "GJob::OnRun");
	result = OnRun(fDelta);
	PFI_E(9066);

	return result;
}

GJobResult GJob::UpdateTask(float fDelta)
{
	PFI_BLOCK_THISFUNC(308);

	if (m_bSuspended)
		return JR_RUNNING;

	GJobResult job_result = JR_RUNNING;

	if (m_pRunningTask)
	{
		PFI_B(9068, "GTask::Run");
		GTaskResult task_result = m_pRunningTask->Run(fDelta);
		PFI_E(9068);

		switch (task_result)
		{
		case TR_COMPLETED:
			{
				PFI_B(9069, "GTask::Complete");
				GTaskID nTaskID = m_pRunningTask->GetID();
				m_pRunningTask->Complete();
				SAFE_DELETE(m_pRunningTask);

				OnTaskComplete(nTaskID);

				job_result = JR_COMPLETED;
				PFI_E(9069);
			}
			break;
		case TR_CANCELED:
			{
				PFI_B(9070, "GTask::Cancel");
				GTaskID nTaskID = m_pRunningTask->GetID();
				m_pRunningTask->Cancel();
				SAFE_DELETE(m_pRunningTask);

				OnTaskCancel(nTaskID);

				job_result = JR_CANCELED;
				PFI_E(9070);
			}
			break;
		}

		if (m_pRunningTask == NULL)
		{
			return job_result;
		}
	}

	return JR_RUNNING;
}

void GJob::Complete()
{
	if (m_pRunningTask)
	{
		m_pRunningTask->Complete();
		SAFE_DELETE(m_pRunningTask);
	}
	
	OnFinished();
	OnComplete();
}

void GJob::Cancel()
{
	if (m_pRunningTask)
	{
		m_pRunningTask->Cancel();
		SAFE_DELETE(m_pRunningTask);
	}

	OnFinished();
	OnCancel();
}

void GJob::TimeOut()
{
	if (m_pRunningTask)
	{
		m_pRunningTask->Cancel();
		SAFE_DELETE(m_pRunningTask);
	}

	OnFinished();
	OnTimeout();
}

void GJob::SetDurationTime(double fTime)
{
	m_fDurationTime = fTime;
}

GEntityNPC* GJob::GetOwnerNPC() 
{	
	if (!m_pOwner)
		return NULL;

	return m_pOwner->GetOwnerNPC(); 
}

const GEntityNPC* GJob::GetOwnerNPC() const
{	
	if (!m_pOwner)
		return NULL;

	return m_pOwner->GetOwnerNPC(); 
}

bool GJob::UpdateDuration(float fDelta)
{
	PFI_BLOCK_THISFUNC(9067);

	if (m_fDurationTime != JOB_DURATION_INFINITE_TIME)
	{
		m_fDurationTime -= fDelta;

		if (m_fDurationTime <= 0.0f)
		{
			m_fDurationTime = JOB_DURATION_INFINITE_TIME;

			return true;
		}
	}

	return false;
}


bool GJob::SetRunningTask( GTask* pTask )
{
	_ASSERT(pTask);
	if (!pTask)
		return false;

	if (m_pRunningTask)
	{
		if (m_pRunningTask->GetID() == pTask->GetID())
		{
			bool bResetOK = m_pRunningTask->Reset(pTask);
			if (bResetOK)
			{
				SAFE_DELETE(pTask);
				return true;
			}
			else
			{
				m_pRunningTask->Cancel();
				SAFE_DELETE(m_pRunningTask);
				SAFE_DELETE(pTask);
				return false;
			}
		}

		m_pRunningTask->Cancel();
		delete m_pRunningTask;
	}
	
	m_pRunningTask = pTask;
	if (m_pRunningTask->Start() == false)
	{
		SAFE_DELETE(m_pRunningTask);
		return false;
	}

	return true;
}

GTask* GJob::GetTask()
{
	return m_pRunningTask;
}

const wchar_t* GJob::GetTaskName()
{
	if (!m_pRunningTask)	return L"";
	
	return m_pRunningTask->GetTaskName();
}

void GJob::OnSuspend()
{
	if (!m_pRunningTask)	return;
	m_pRunningTask->OnSuspend();
}

void GJob::OnResume()
{
	if (!m_pRunningTask)	return;
	m_pRunningTask->OnResume();
}

GJobResult GJob::OnRun( float fDelta )
{
	return JR_COMPLETED;
}

void GJob::Suspend()
{
	m_bSuspended = true;
	OnSuspend();
}

void GJob::Resume()
{
	m_bSuspended = false;
	OnResume();
}

void GJob::OnFinished()
{
	VALID(m_pOwner);
	VALID(m_pOwner->GetOwnerNPC());
	m_pOwner->GetOwnerNPC()->OnJobFinished(this);
}

MUID GJob::GetOwnerUID()
{
	return m_uidOwner;
}

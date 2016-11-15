#include "stdafx.h"
#include "GJobMgr.h"
#include "GJob.h"
#include "GAIProfiler.h"
#include "GConst.h"
#include "GEntityNPC.h"
#include "GAIMonitorPort.h"
#include "GModuleAI.h"

#define SEND_JOBMGR_MESSAGE_TO_VIEWER(MESSAGE) \
if (GConst::AIMON_ENABLE && \
	m_pOwner && \
	GAIMonitorPort::Inst().IsSetedNPC(m_pOwner->GetOwnerUID())) \
	{ \
		GAIMonitorPort::Inst().OnJobEvent(false, MESSAGE); \
	}

GJobMgr::GJobMgr( GModuleAI* pOwner/*=NULL*/ )
: m_pOwner(pOwner)
, m_pCurrJob(NULL)
, m_bEnableClearJob(true)
, m_bLazyDeleteCurJob(false)
{	
}

GJobMgr::~GJobMgr()
{
	Clear(true);	// all delete

	if (m_bLazyDeleteCurJob)
	{
		if (m_pCurrJob &&
			m_pCurrJob->Attr.bAutoDeletable)
		{
			delete m_pCurrJob;
		}
	}
}

void GJobMgr::Clear(bool bCancelCurJob)
{
	if (!m_bEnableClearJob)
		return;

	if (m_pCurrJob &&
		bCancelCurJob)
	{
		m_pCurrJob->Attr.bClearJobListWhenCanceled = false;
		m_bLazyDeleteCurJob = true;
	}


	for (list<GJob*>::iterator itor = m_Jobs.begin(); itor != m_Jobs.end(); ++itor)
	{
		GJob* pJob = *itor;

		if (!pJob->Attr.bAutoDeletable) 
			continue;

		delete pJob;
	}

	m_Jobs.clear();

	PostMessage(GMessage(GMSG_AI_EVT_JOB_CLEAR));

	SEND_JOBMGR_MESSAGE_TO_VIEWER("ClearJob - Execution!");
}

void GJobMgr::Push(GJob* pJob)
{
	if (NULL == pJob) return;

	m_Jobs.push_back(pJob);
}

void GJobMgr::PushFront(GJob* pJob)
{
	if (NULL == pJob) return;

	if (m_pCurrJob)
	{
		m_pCurrJob->Cancel();
		m_Jobs.push_front(m_pCurrJob);
	}

	m_pCurrJob = pJob;

	do 
	{
		if (m_pCurrJob->Start())
			break;

		if (!PopJob())
			break;

	} while(m_pCurrJob);
}

bool GJobMgr::PopAndStartJob()
{
	PFI_BLOCK_THISFUNC(1799);
	
	do 
	{
		// 현재 태스크가 없으면 새로운 태스크를 꺼내서 시작한다.
		if (!PopJob())
			return false;
		
		VALID_RET(m_pCurrJob, false);

		if (GConst::LOG_JOB)
		{
			VALID_RET(m_pCurrJob->GetOwnerNPC(), false);
			mlog("[JobLog] <Run_Job -> %d remain> Instant: %d, Owner:%s(%I64d), Job:%s\n", 
				GetCount(), m_pCurrJob->IsInstant(), MLocale::ConvUTF16ToAnsi(m_pCurrJob->GetOwnerNPC()->GetName()).c_str(), m_pCurrJob->GetOwnerNPC()->GetUID(), MLocale::ConvUTF16ToAnsi(m_pCurrJob->GetJobName()).c_str());
		}

		PFI_B(9065, "GJobMgr::StartJob");
		if (m_pCurrJob->IsInstant())
		{	
			// GJobT_Attack 등 Start()에서 BehaviorState가 변경되는 경우 m_pCurrJob이 NULL이 된다.
			GJob* pJob = m_pCurrJob;
			
			pJob->Start();
			OnJobFinished(pJob, JR_COMPLETED);
		}
		else
		{
			if (m_pCurrJob->Start())
				return true;
			
			if (m_pCurrJob->Attr.bAutoDeletable)
				delete m_pCurrJob;

			OnJobFinished(m_pCurrJob, JR_COMPLETED);
			m_pCurrJob = NULL;
		}

		PFI_E(9065);
	} while(m_pCurrJob);

	return false;
}

GJobResult GJobMgr::Run(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(306);

	if (m_bLazyDeleteCurJob)
	{
		m_bLazyDeleteCurJob = false;
		CancelCurrJob();
	}

	if (!m_pCurrJob)
	{
		if (!PopAndStartJob())
			return JR_NONE;
		return JR_RUNNING;
	}

	AI_PFI_JOB_BLOCK(m_pCurrJob->GetID(), m_pCurrJob->GetJobName());

	PFI_B(9008, "GJob::Run");
	// 현재 태스크가 있으면 실행
	GJobResult ret = m_pCurrJob->Run(fDelta);
	PFI_E(9008);

	PFI_B(9009, "GJobMgr::OnJobFinished");

	if (IsFinished(ret))
	{
		OnJobFinished(m_pCurrJob, ret);
	}

	switch (ret)
	{
	case JR_COMPLETED: { CompleteCurrJob();	}break;
	case JR_CANCELED:  { CancelCurrJob();	}break;
	case JR_TIMEOUT:   { TimeoutCurrJob();	}break;
	}

	PFI_E(9009);

	return ret;
}

bool GJobMgr::PopJob()
{
	PFI_BLOCK_THISFUNC(9064);

	if (m_pCurrJob &&
		m_pCurrJob->Attr.bAutoDeletable)
	{
		SAFE_DELETE(m_pCurrJob);
		m_pCurrJob = NULL;
	}
	
	if (m_Jobs.empty()) 
	{
		m_pCurrJob = NULL;
		return false;
	}

	list<GJob*>::iterator itor = m_Jobs.begin();
	m_pCurrJob = (*itor);
	m_Jobs.erase(itor);

	return true;
}


void GJobMgr::CancelCurrJob()
{
	if (!m_pCurrJob) return;

	m_pCurrJob->Cancel();

	//bool bClearJobList = m_pCurrJob->Attr.bClearJobListWhenCanceled;

	if (m_pCurrJob->Attr.bAutoDeletable)
		delete m_pCurrJob;

	m_pCurrJob = NULL;
	m_bLazyDeleteCurJob = false;

	//if (bClearJobList)
	//{
	//	SEND_JOBMGR_MESSAGE_TO_VIEWER("ClearJob - cause by 'CancelCurJob'");
	//	Clear(false);
	//}
}

void GJobMgr::TimeoutCurrJob()
{
	if (!m_pCurrJob) return;

	m_pCurrJob->TimeOut();

	if (m_pCurrJob->GetTask())
		return;	// OnTimeout에서 또 다른 태스크를 넣어준 경우, 태스크를 끝까지 수행함

	bool bClearJobList = m_pCurrJob->Attr.bClearJobListWhenTimedout;

	if (m_pCurrJob->Attr.bAutoDeletable)
		delete m_pCurrJob;

	m_pCurrJob = NULL;

	if (bClearJobList)
	{
		SEND_JOBMGR_MESSAGE_TO_VIEWER("ClearJob - cause by 'TimeoutCurJob'");
		Clear(false);
	}
}

void GJobMgr::CompleteCurrJob()
{
	if (!m_pCurrJob) return;

	m_pCurrJob->Complete();

	if (m_pCurrJob->Attr.bAutoDeletable)
		delete m_pCurrJob;

	m_pCurrJob = NULL;
}

void GJobMgr::Cancel()
{
	CancelCurrJob();
}

GTask* GJobMgr::GetCurrTask()
{
	if (m_pCurrJob == NULL)
	{
		return NULL;
	}

	return m_pCurrJob->GetTask();
}

GJobID GJobMgr::GetCurrJobID()
{
	if (m_pCurrJob)
	{
		return m_pCurrJob->GetID();
	}
	return GJOB_NA;
}

void GJobMgr::Suspend()
{
	if (!m_pCurrJob) return;

	m_pCurrJob->Suspend();
}

void GJobMgr::Resume()
{
	if (!m_pCurrJob) return;

	m_pCurrJob->Resume();
}

GJob* GJobMgr::GetJob( size_t nIndex )
{
	if (nIndex < 0)
		return NULL;

	if (nIndex >= m_Jobs.size())
		return NULL;

	list<GJob*>::iterator it = m_Jobs.begin();
	while(nIndex--)
		++it;
	return *it;
}

bool GJobMgr::IsFinished(GJobResult nResult)
{
	switch (nResult)
	{
	case JR_COMPLETED:		return true;
	case JR_CANCELED:		return true;
	case JR_TIMEOUT:		return true;
	}

	return false;
}

void GJobMgr::OnJobFinished(GJob* pJob, GJobResult nResult )
{
	if (!pJob)
		return;

	if (!IsFinished(nResult))
		return;

	switch (nResult)
	{
	case JR_COMPLETED:		PostMessage(GMessage(GMSG_AI_EVT_JOB_COMPLETED, pJob));		break;
	case JR_CANCELED:		PostMessage(GMessage(GMSG_AI_EVT_JOB_CANCELED, pJob));		break;
	case JR_TIMEOUT:		PostMessage(GMessage(GMSG_AI_EVT_JOB_TIMEDOUT, pJob));		break;
	}
	
	PostMessage(GMessage(GMSG_AI_EVT_JOB_FINISHED, pJob));
}

deque<GJob*> GJobMgr::GetJobs() const
{
	deque<GJob*> ret(m_Jobs.begin(), m_Jobs.end());
	ret.push_front(m_pCurrJob);
	return ret;
}

GJob* GJobMgr::GetCurrJob()
{
	if (m_bLazyDeleteCurJob)
		return NULL;

	return m_pCurrJob;
}

void GJobMgr::PostMessage( GMessage& m )
{
	if (!m_pOwner)
		return;

	GEntityNPC* pNPC = m_pOwner->GetOwnerNPC();
	if (!pNPC)
		return;

	pNPC->Message(m);
}

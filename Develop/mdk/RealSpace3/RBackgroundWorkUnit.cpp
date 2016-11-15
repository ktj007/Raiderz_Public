#include "stdafx.h"
#include "RBackgroundWorkUnit.h"
#include "RBackgroundWorker.h"

namespace rs3 {


RBackgroundWorkUnit::RBackgroundWorkUnit() 
: m_pTicket(NULL)
, m_pBackgroundWorker(NULL)
, m_bRecursiveChained(false)
, m_bBackgroundWorkFinished(false)
, m_nState(WS_INVALID)
{

}

RBackgroundWorkUnit::~RBackgroundWorkUnit() 
{
	_ASSERT(NULL == m_pTicket );
	if( m_pTicket )
	{
		mlog("Caution ! : in RBackgroundWorkUnit::~RBackgroundWorkUnit() , m_pTicket is not NULL. \n");
	}
}

void RBackgroundWorkUnit::FinishWork()
{
	// 백그라운드 작업에서 FinishWork를 호출하면 안되고, InvalidateWork를 호출해야 한다.
	_ASSERT(m_bBackgroundWorkFinished == true);

	if (m_pTicket != NULL)
	{
		m_pTicket->InvalidateWork();
		m_pTicket = NULL;
	}

	m_nState = WS_FINISHED;
}

void RBackgroundWorkUnit::InvalidateWork()
{
	if (m_pTicket != NULL && m_pBackgroundWorker)
	{
		m_pBackgroundWorker->m_csTicket.Lock();

		m_pTicket->InvalidateWork();
		m_pTicket = NULL;

		m_pBackgroundWorker->m_csTicket.Unlock();
	}

	m_nState = WS_INVALID;
}

void RBackgroundWorkUnit::InvalidateWorkWithNoLock()
{
	if (m_pTicket != NULL && m_pBackgroundWorker)
	{
		m_pTicket->InvalidateWork();
		m_pTicket = NULL;
	}
	m_nState = WS_INVALID;
}

void RBackgroundWorkUnit::BackgroundWork()
{
	bool bNeedToLock = m_pBackgroundWorker && m_pBackgroundWorker->IsInited();
		
	if (bNeedToLock) m_pBackgroundWorker->LockResource();

	OnBackgroundWork();

	if (bNeedToLock) m_pBackgroundWorker->UnLockResource();

	m_bBackgroundWorkFinished = true;
}

void RBackgroundWorkUnit::UpdateCompleteWork()
{
	OnUpdateCompleteWork();

	if (m_bRecursiveChained == true && IsFinished() == false)
	{
		// for next job
		if (m_pBackgroundWorker)
		{
			m_pBackgroundWorker->AddToReadyQueue(this);
		}
	}
}

void RBackgroundWorkUnit::WorkInTheForeground()
{
	SetState(WS_RUNNING);

	do 
	{
		BackgroundWork();	// lock을 걸어줘야 하기 때문에 OnBackgroundWork() 가 아니다.

		OnUpdateCompleteWork();

	} while (m_bRecursiveChained = true && GetState() == WS_RUNNING);
}

}	// namespace rs3

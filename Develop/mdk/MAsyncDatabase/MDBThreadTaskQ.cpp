#include "stdafx.h"
#include "MDBThreadTaskQ.h"


namespace mdb
{
	MDBThreadTaskQ::MDBThreadTaskQ()
	{
		m_dwWaitEventTime = 0;
	}


	MDBThreadTaskQ::~MDBThreadTaskQ()
	{
		
	}


	bool MDBThreadTaskQ::Init(const DWORD dwWaitEventTime)
	{
		m_hPushEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		_ASSERT(NULL != m_hPushEvent);
		if (NULL == m_hPushEvent)
		{
			return false;
		}

		m_dwWaitEventTime = dwWaitEventTime;

		return true;
	}


	void MDBThreadTaskQ::Push(MDBThreadTask* pTask)
	{
		_ASSERT(NULL != pTask);

		if (NULL == pTask)
		{
			return;
		}

		{
			MDBGuard Guard(&m_CS);
			m_TaskList.push_back(pTask);
		}

		SetPushEvent();
	}


	MDBThreadTask* MDBThreadTaskQ::FetchTask()
	{
		if (0 < m_TaskList.size())
		{
			MDBGuard Guard(&m_CS);
			MDBThreadTask* pTask = _FetchTaskUnsafe();
			return pTask;
		}

		const DWORD dwEvent = WaitForMultipleObjects(1, &m_hPushEvent, TRUE, m_dwWaitEventTime);
		_ASSERT(WAIT_FAILED != dwEvent);
		if (WAIT_FAILED == dwEvent)
		{
			return NULL;
		}

		switch(dwEvent)
		{
		case WAIT_OBJECT_0 :
			{
				MDBThreadTask* pTask = NULL;
				{
					MDBGuard Guard(&m_CS);
					pTask = _FetchTaskUnsafe();
				}
				
				if (0 < m_TaskList.size())
				{
					SetPushEvent();
				}

				return pTask;
			}
			break;	

		case WAIT_TIMEOUT :
			{
				return NULL;
			}
			break;
		}

		return NULL;
	}


	MDBThreadTask* MDBThreadTaskQ::_FetchTaskUnsafe()
	{
		if (m_TaskList.empty())
		{
			return NULL;
		}

		MDBThreadTask* pFetchedTask = *(m_TaskList.begin());
		m_TaskList.pop_front();

		return pFetchedTask;
	}


	void MDBThreadTaskQ::Release()
	{
		MDBGuard Guard(&m_CS);

		for (MDB_THRTASK_LIST::iterator it = m_TaskList.begin()
			; it != m_TaskList.end()
			; ++it)
		{
			MDBThreadTask* pTask = (*it);
			delete pTask;
		}

		m_TaskList.clear();
	}


	size_t MDBThreadTaskQ::GetSize()
	{
		return m_TaskList.size();
	}


	void MDBThreadTaskQ::SetPushEvent()
	{
		SetEvent(m_hPushEvent);
	}
}
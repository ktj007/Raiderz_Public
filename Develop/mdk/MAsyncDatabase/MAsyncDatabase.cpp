#include "stdafx.h"
#include "MAsyncDatabase.h"
#include "MDBThreadTaskResultQ.h"
#include "MDBProfileDefine.h"
#include "MLocale.h"

namespace mdb
{
	MAsyncDatabase::MAsyncDatabase()
	{
		m_dwAsyncExecCount		= 0;
		m_dwAsyncCompltedCount	= 0;
		m_dwUpdateCount			= 0;
		m_nMaxTaskQSize			= MDB_THREAD_QSIZE_FREE;
		m_bIsCreated			= false;
	}


	MAsyncDatabase::~MAsyncDatabase()
	{
		if (true == m_bIsCreated)
		{
			Release();

			m_bIsCreated = false;
		}
	}


	bool MAsyncDatabase::ExecuteAsync(MDBThreadTask* pTask)
	{
		if ((MDB_THREAD_QSIZE_FREE != m_nMaxTaskQSize) 
			&& (m_nMaxTaskQSize < m_TaskQ.GetSize()))
		{
			return false;
		}

		m_Profiler.AddExecute();

		MDBR_DWORD_ADD_COUNT(m_dwAsyncExecCount);

		m_TaskQ.Push(pTask);
		
		return true;
	}


	MDB_AYSNC_RESULT MAsyncDatabase::Create(const string& strDSN
		, const DWORD dwThreadCount		
		, const size_t nMaxTaskQSize
		, const DWORD dwWaitEventTime)
	{
		return CreateW(MLocale::ConvAnsiToUCS2(strDSN.c_str()), dwThreadCount, nMaxTaskQSize, dwWaitEventTime);
	}

	MDB_AYSNC_RESULT MAsyncDatabase::CreateW(const wstring& strDSN
		, const DWORD dwThreadCount
		, const size_t nMaxTaskQSize
		, const DWORD dwWaitEventTime)
	{
		if (!m_TaskQ.Init(dwWaitEventTime))
		{
			return MDBAR_TASKQ_ERR;
		}

		m_nMaxTaskQSize = nMaxTaskQSize;

		if (!m_TaskResultQ.Init())
		{
			return MDBAR_TASKRESQ_ERR;
		}

		if (!m_ThreadPool.Create(dwThreadCount, strDSN, &m_TaskQ, &m_TaskResultQ))
		{
			return MDBAR_THRPOOL_ERR;
		}

		if (!OnCreate(dwThreadCount, strDSN, nMaxTaskQSize, dwWaitEventTime))
		{
			return MDBAR_ONCREATE_ERR;
		}

		return MDBAR_SUCCESS;
	}


	bool MAsyncDatabase::OnCreate(const DWORD dwThreadCount
		, const wstring& strDSN
		, const size_t nMaxTaskQSize
		, const DWORD dwWaitEventTime)
	{
		return true;
	}


	void MAsyncDatabase::Update()
	{
		m_Profiler.BeginOnCompleted();

		MDB_THRTASK_RESULT_LIST*	pTaskResultQ	= NULL; 
		MDBThreadTask*				pTask			= NULL;

		while (true)
		{
			pTaskResultQ = m_TaskResultQ.GetResultList();
			_ASSERT(NULL != pTaskResultQ);
			if (NULL == pTaskResultQ)
			{
				return;
			}

			if (pTaskResultQ->empty())
			{
				return;
			}

			for (MDB_THRTASK_RESULT_LIST::iterator it = pTaskResultQ->begin()
				; it != pTaskResultQ->end()
				; ++it)
			{
				pTask = (*it);
				if (MDBTR_SUCESS == pTask->OnCompleted())
				{
					delete pTask;
				}

				m_Profiler.AddCompleted();
				MDBR_DWORD_ADD_COUNT(m_dwAsyncCompltedCount);
			}

			pTaskResultQ->clear();
		}		

		m_Profiler.EndOnCompleted();

		MDBR_DWORD_ADD_COUNT(m_dwUpdateCount);
	}


	MDBProfiler& MAsyncDatabase::GetProfiler()
	{
		return m_Profiler;
	}


	DWORD MAsyncDatabase::GetExecCount()
	{
		return m_dwAsyncExecCount;
	}

	DWORD MAsyncDatabase::GetCompletedCount()
	{
		return m_dwAsyncCompltedCount;
	}


	DWORD MAsyncDatabase::GetUpdateCount()
	{
		return m_dwUpdateCount;
	}


	DWORD MAsyncDatabase::GetTaskQSize()
	{
		// 정확한 수치는 아니다.
		// TaskResultQ에 있는 작업은 계산에서 제외된다.

		return m_dwAsyncExecCount - m_dwAsyncCompltedCount;
	}


	bool MAsyncDatabase::IsAllTaskCompleted()
	{
		return 0 == GetTaskQSize();
	}


	void MAsyncDatabase::WaitForThreadExitCompleted()
	{
		m_ThreadPool.SetExit();
		while (false == m_ThreadPool.IsExitComplete())
		{
			m_TaskQ.SetPushEvent();
		}
	}


	void MAsyncDatabase::Release()
	{
		// 여기선 큐에 남은 작업에 대해선 책임지지 않는다.
		
		WaitForThreadExitCompleted();
		
		m_ThreadPool.Release();
		m_TaskQ.Release();
		m_TaskResultQ.Release();

		OnRelease();
	}


	void MAsyncDatabase::OnRelease()
	{

	}
}
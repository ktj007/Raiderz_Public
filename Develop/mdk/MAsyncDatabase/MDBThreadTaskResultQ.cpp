#include "stdafx.h"
#include "MDBThreadTaskResultQ.h"


namespace mdb
{
	MDBThreadTaskResultQ::MDBThreadTaskResultQ()
	{
		m_bIsFatching		= false;
		m_pWorkingTaskList	= &m_TaskList[0];
		m_pFetchingTaskList = &m_TaskList[1];
	}


	MDBThreadTaskResultQ::~MDBThreadTaskResultQ()
	{

	}


	bool MDBThreadTaskResultQ::Init()
	{
		return true;
	}


	/*
	이 함수는 하나의 쓰레드에서만 호출이 되어야 한다.
	멀티 쓰레드에서 사용하면 안정성을 보장하지 못함.
	*/
	MDB_THRTASK_RESULT_LIST* MDBThreadTaskResultQ::GetResultList()
	{
		_ASSERT(false == m_bIsFatching);
		if (m_bIsFatching)
		{
			// 중복 호출이 되었음... 이건 멀티 쓰레드에서 사용하면 위험함.
			return NULL;
		}

		m_bIsFatching = true;
		
		{
			MDBGuard Guard(&m_CS);

			MDB_THRTASK_RESULT_LIST* pTmp = m_pWorkingTaskList;
			m_pWorkingTaskList = m_pFetchingTaskList;
			m_pFetchingTaskList = pTmp;
		}

		m_bIsFatching = false;
		
		return m_pFetchingTaskList;
	}


	void MDBThreadTaskResultQ::Push(MDBThreadTask* pTask)
	{
		MDBGuard Guard(&m_CS);
		m_pWorkingTaskList->push_back(pTask);
	}


	void MDBThreadTaskResultQ::Release()
	{
		MDBGuard Guard(&m_CS);

		for (MDB_THRTASK_RESULT_LIST::iterator it = m_pWorkingTaskList->begin()
			; it != m_pWorkingTaskList->end()
			; ++it)
		{
			MDBThreadTask* pTask = (*it);
			delete pTask;
		}
		m_pWorkingTaskList->clear();


		for (MDB_THRTASK_RESULT_LIST::iterator it = m_pFetchingTaskList->begin()
			; it != m_pFetchingTaskList->end()
			; ++it)
		{
			MDBThreadTask* pTask = (*it);
			delete pTask;
		}
		m_pFetchingTaskList->clear();
	}


	size_t MDBThreadTaskResultQ::GetWoringQSize()
	{
		MDBGuard Guard(&m_CS);

		size_t nSize = m_pWorkingTaskList->size();

		return nSize;
	}


	size_t MDBThreadTaskResultQ::GetFetchingQSize()
	{
		MDBGuard Guard(&m_CS);

		size_t nSize = m_pFetchingTaskList->size();

		return nSize;
	}
}
#pragma once


#include <list>
using std::list;


#include "MDBCriticalSection.h"


namespace mdb
{
	class MDBThreadTask;

	static const int MDB_TASK_RESULTQ_COUNT = 2;

	typedef list<MDBThreadTask*> MDB_THRTASK_RESULT_LIST;


	class MDBThreadTaskResultQ
	{
	public :
		MDBThreadTaskResultQ();
		~MDBThreadTaskResultQ();


		bool						Init();


		MDB_THRTASK_RESULT_LIST*	GetResultList();
		void						Push(MDBThreadTask* pTask);


		size_t						GetWoringQSize();
		size_t						GetFetchingQSize();

		void						Release();


	private :
		MDB_THRTASK_RESULT_LIST		m_TaskList[MDB_TASK_RESULTQ_COUNT];
		MDB_THRTASK_RESULT_LIST*	m_pWorkingTaskList;
		MDB_THRTASK_RESULT_LIST*	m_pFetchingTaskList;

		bool						m_bIsFatching;

		MDBCriticalSecton			m_CS;
	};
}
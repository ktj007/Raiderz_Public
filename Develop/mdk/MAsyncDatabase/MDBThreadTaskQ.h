#pragma once


#include <list>
using std::list;


#include "MDBCriticalSection.h"


namespace mdb
{
	class MDBThreadTask;


	static const DWORD	MDB_TASK_Q_WAIT_IFINITE = INFINITE;
	static const DWORD	MDB_TASK_Q_DEFAULT_WAIT = 1000; // 1초.


	class MDBThreadTaskQ
	{
	public :
		MDBThreadTaskQ();
		~MDBThreadTaskQ();

		bool				Init(const DWORD dwWaitEventTime = MDB_TASK_Q_DEFAULT_WAIT 	// 큐에 Task가 없을때 다음 Task가 큐에 등록될때까지 
																						// 기다리는 시간.
																						// 0이면 기다리지 않는다.
																						// MDB_TASK_Q_WAIT_IFINITE이면 입력될때까지 계속 기다린다.
							);

		void				Push(MDBThreadTask* pTask);
		MDBThreadTask*		FetchTask();
		void				SetPushEvent();

		
		size_t				GetSize();


		void				Release();


	private :
		MDBThreadTask*		_FetchTaskUnsafe();


	private :
		typedef list<MDBThreadTask*> MDB_THRTASK_LIST;


		MDBCriticalSecton	m_CS;
		HANDLE				m_hPushEvent;
		MDB_THRTASK_LIST	m_TaskList;
		DWORD				m_dwWaitEventTime;	
	};
}
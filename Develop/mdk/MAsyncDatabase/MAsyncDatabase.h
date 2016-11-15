#pragma once

#include <string>
using namespace std;

#include "MADBLib.h"
#include "MDBThreadPool.h"
#include "MDBThreadTaskQ.h"
#include "MDBThreadTaskResultQ.h"
#include "MDBProfiler.h"


namespace mdb
{
	enum MDB_AYSNC_RESULT
	{
		MDBAR_SUCCESS			= 0
		, MDBAR_DBCONN_ERR		= 1
		, MDBAR_TASKQ_ERR		= 2
		, MDBAR_TASKRESQ_ERR	= 3
		, MDBAR_THRPOOL_ERR		= 4
		, MDBAR_ONCREATE_ERR	= 5
	};


	static const size_t MDB_THREAD_QSIZE_FREE	= 0; // Q에 저장될 수 있는 크기를 설정한다. 이 이상 등록하면 실패함.
	static const DWORD	MDB_THREAD_COUNT		= 4;


	class MADB_API MAsyncDatabase
	{
	public :
		MAsyncDatabase();
		virtual ~MAsyncDatabase();


		MDB_AYSNC_RESULT			Create(const string& strDSN
										, const DWORD dwThreadCount = MDB_THREAD_COUNT
										, const size_t nMaxTaskQSize = MDB_THREAD_QSIZE_FREE																									
										, const DWORD dwWaitEventTime = MDB_TASK_Q_DEFAULT_WAIT
										);

		MDB_AYSNC_RESULT			CreateW(const wstring& strDSN
										, const DWORD dwThreadCount = MDB_THREAD_COUNT
										, const size_t nMaxTaskQSize = MDB_THREAD_QSIZE_FREE																									
										, const DWORD dwWaitEventTime = MDB_TASK_Q_DEFAULT_WAIT
										);


		bool						ExecuteAsync(MDBThreadTask* pTask);
		void						Update();


		DWORD						GetExecCount();
		DWORD						GetCompletedCount();
		DWORD						GetUpdateCount();
		DWORD						GetTaskQSize();
		bool						IsAllTaskCompleted();

		void						Release();


		MDBProfiler&				GetProfiler();


	public :
		virtual bool				OnCreate(const DWORD dwThreadCount
											, const wstring& strDSN
											, const size_t nMaxTaskQSize
											, const DWORD dwWaitEventTime);
		virtual void				OnRelease();


	protected :
		void						WaitForThreadExitCompleted();
		

	protected :
		MDBThreadPool				m_ThreadPool;
		MDBThreadTaskQ				m_TaskQ;
		MDBThreadTaskResultQ		m_TaskResultQ;		
		MDBProfiler					m_Profiler;

		size_t						m_nMaxTaskQSize;

		DWORD						m_dwAsyncExecCount;
		DWORD						m_dwAsyncCompltedCount;
		DWORD						m_dwUpdateCount;

		bool						m_bIsCreated;
	};
}
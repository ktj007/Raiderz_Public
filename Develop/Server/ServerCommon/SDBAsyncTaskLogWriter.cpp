#include "stdafx.h"
#include "SDBAsyncTaskLogWriter.h"
#include "SDBTask.h"


uint32 SDBAsyncTaskLogCounter::m_nCount = 0;


SDBAsyncTaskLogWriter::SDBAsyncTaskLogWriter( SDBAsyncTask* pTask ) : m_pTask(pTask)
{

}

SDBAsyncTaskLogWriter::~SDBAsyncTaskLogWriter()
{
	m_pTask = NULL;
}

void SDBAsyncTaskLogWriter::Write()
{
	if (NULL == m_pTask)
		return;

#ifdef _DEBUG
	WriteLogHead();
	WriteFirstQuery();
#endif

	if (NULL != m_pTask->GetStatus())
	{
		WriteLog(m_pTask->GetStatus()->GetStateVec(), 0);
		WriteLog(m_pTask->GetStatus()->GetUserStateVec(), m_pTask->GetStatus()->GetStateVec().size());
	}
	else
	{
		MLog3("DB ERROR: TASK ID : %d, Init Status fail.\n", m_pTask->GetID());	
	}

	WriteTracingProc();

	
#ifdef _DEBUG
	WriteLogEnd();
#endif
}

void SDBAsyncTaskLogWriter::WriteLogHead()
{
	MLog3("==== DB REPORT START ====\n");
	MLog3("REPORT COUNT : %d\n", m_Counter.GetCount());
}

void SDBAsyncTaskLogWriter::WriteLogEnd()
{
	MLog3("==== DB REPORT END ====\n");

	m_Counter.Add();
}

void SDBAsyncTaskLogWriter::WriteFirstQuery()
{
	static const int MAX_SQL_LENGTH = 128;
	char szSQL[MAX_SQL_LENGTH]	= {0, };

	_snprintf_s(szSQL, MAX_SQL_LENGTH, MAX_SQL_LENGTH - 1, "DB ERROR : SQL : DBTASK ID : %d, QSIZE : %d, %s\n"
		, m_pTask->GetID()
		, m_pTask->GetQuerySize()
		, m_pTask->GetSQL(0).c_str());	
	MLog3(szSQL);
}

void SDBAsyncTaskLogWriter::WriteLog( mdb::MDB_STATE_VEC& vecState, const size_t nStartIndex )
{
	if (vecState.empty())
		return;

	// MDebug.cpp에 #define MLOG_TEMP_BUF_LEN		8192		// 8k 이렇게 정의되어 있음.
	wchar_t szQueryError[8192]		= {0, };

	const size_t nSize = vecState.size();
	for (size_t i = 0; i < nSize; ++i)
	{			
		_snwprintf_s(szQueryError, 8192, 8191, L"\tDB ERROR : DBTASK ID : %d, DB STATE : NUM(%d), %s\n"
			, m_pTask->GetID()
			, nStartIndex + i
			, vecState[i].ToStringW().c_str());
		
		MLog3(szQueryError);
	}
}

void SDBAsyncTaskLogWriter::WriteTracingProc()
{
	if (SDBAsyncTask::GetTaskTracer().IsTracingID(m_pTask->GetID()))
	{
		for (size_t i = 0; i < m_pTask->GetQuerySize(); ++i)
		{
			if (SDBAsyncTask::GetTaskTracer().GetTaceCheckCount(m_pTask->GetID()) 
				< SDBAsyncTask::GetTaskTracer().GetMaxTraceCheckCount())
			{
				MLog3("DB ERROR : TRACING PROC(%d, U) = %s\n", i, m_pTask->GetSQLW(i).c_str());			
				MLog3("DB ERROR : TRACING PROC(%d, A) = %s\n", i, m_pTask->GetSQL(i).c_str());			
				SDBAsyncTask::GetTaskTracer().IncTraceCheckCount(m_pTask->GetID());
			}			
		}
	}
}

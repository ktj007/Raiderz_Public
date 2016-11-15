#ifndef _SDBASYNC_TASK_LOG_WRITER_H
#define  _SDBASYNC_TASK_LOG_WRITER_H


#include "MDatabaseInfo.h"


class SDBAsyncTask;


class SDBAsyncTaskLogCounter
{
public :
	void	Add()		{ ++m_nCount; }
	uint32 GetCount()	{ return m_nCount; }

public :
	static uint32 m_nCount;
};


class SDBAsyncTaskLogWriter
{
public :
	SDBAsyncTaskLogWriter(SDBAsyncTask* pTask);
	~SDBAsyncTaskLogWriter();

	void Write();

private :
	void WriteLogHead();
	void WriteFirstQuery();
	void WriteLog(mdb::MDB_STATE_VEC& vecState, const size_t nStartIndex);
	void WriteTracingProc();
	void WriteLogEnd();

private :
	SDBAsyncTask*			m_pTask;
	SDBAsyncTaskLogCounter	m_Counter;
};



#endif
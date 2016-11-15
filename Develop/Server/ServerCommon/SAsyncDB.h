#ifndef _SASYNCDB_H
#define _SASYNCDB_H


#include "MAsyncDatabase.h"
#include "MDatabaseDesc.h"
#include "ServerCommonLib.h"
#include "SDatabaseType.h"


class SDBAsyncTask;


class SCOMMON_API SAsyncDB : public MTestMemPool<SAsyncDB>
{
public :
	SAsyncDB();
	virtual ~SAsyncDB();

	SDBT_DBTYPE GetDBType();

	virtual mdb::MDB_AYSNC_RESULT Init(const SDBT_DBTYPE DBType
		, mdb::MDatabaseDesc& DBDesc
		, const DWORD dwThreadCount		= mdb::MDB_THREAD_COUNT
		, const size_t nMaxTaskQSize	= mdb::MDB_THREAD_QSIZE_FREE
		, const DWORD dwWaitEventTime	= mdb::MDB_TASK_Q_DEFAULT_WAIT);
	
	virtual void Release();
	virtual void Update();

	virtual bool ExecuteAsync(SDBAsyncTask* pTask);


protected :
	virtual bool ExecuteAsyncForTest(SDBAsyncTask* pTask);


protected :
	mdb::MDatabaseDesc  m_DBDesc;
	mdb::MAsyncDatabase m_AsyncDB;
	SDBT_DBTYPE			m_DBType;
};


#endif
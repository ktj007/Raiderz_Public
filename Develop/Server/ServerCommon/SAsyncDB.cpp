#include "stdafx.h"
#include "SAsyncDB.h"
#include "SDBTask.h"

SAsyncDB::SAsyncDB()
{

}

SAsyncDB::~SAsyncDB()
{

}

mdb::MDB_AYSNC_RESULT SAsyncDB::Init( const SDBT_DBTYPE DBType
									 , mdb::MDatabaseDesc& DBDesc 
									 , const DWORD dwThreadCount /*= mdb::MDB_THREAD_COUNT */
									 , const size_t nMaxTaskQSize /*= mdb::MDB_THREAD_QSIZE_FREE */
									 , const DWORD dwWaitEventTime /*= mdb::MDB_TASK_Q_DEFAULT_WAIT*/ )
{
	m_DBType = DBType;
	m_DBDesc = DBDesc;
	return m_AsyncDB.Create(DBDesc.BuildDSNString(), dwThreadCount, nMaxTaskQSize, dwWaitEventTime);		
}

void SAsyncDB::Release()
{
	while (0 < m_AsyncDB.GetTaskQSize())
		m_AsyncDB.Update();

	m_AsyncDB.Release();
}

void SAsyncDB::Update()
{
	m_AsyncDB.Update();
}

bool SAsyncDB::ExecuteAsync( SDBAsyncTask* pTask )
{
	_ASSERT(NULL != pTask);

	if (NULL == pTask)
		return false;

	_ASSERT(GetDBType() == pTask->GetDBType());

	if (GetDBType() != pTask->GetDBType())
		return false;

	return ExecuteAsyncForTest(pTask);
}

bool SAsyncDB::ExecuteAsyncForTest( SDBAsyncTask* pTask )
{
	return m_AsyncDB.ExecuteAsync(pTask);	
}

SDBT_DBTYPE SAsyncDB::GetDBType()
{
	return m_DBType;
}


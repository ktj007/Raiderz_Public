#include "stdafx.h"
#include "SDBTask.h"
#include "MDatabase.h"
#include "MDBRecordSet.h"
#include "SDBAsyncTaskLogWriter.h"
#include "SDBTaskExecuter.h"

SDBTaskTracer SDBAsyncTask::m_TaskTracer;

SDBAsyncTask::SDBAsyncTask( const SDBT_DBTYPE DBType, const SDBTASK_ID TaskID, const size_t nReserveSQLSize /*= 1*/ ) 
: MDBThreadTask(nReserveSQLSize), m_pStatus(NULL), m_Result(SDBTR_FAIL), m_DBType(DBType), m_TaskID(TaskID)
{
}

SDBAsyncTask::~SDBAsyncTask()
{
	SAFE_DELETE(m_pStatus);	
}

void SDBAsyncTask::ExecuteSync(mdb::MDatabase& rfDB, const wstring& strSQL)
{
	if (strSQL.empty())
		return;

	__super::PushSQLW(strSQL);

	OnExecute(rfDB);
}

bool SDBAsyncTask::CheckResult()
{
	if ((NULL != m_pStatus) || (SDBTR_FAIL == GetResult()))
		WriteLog();
	
	return (SDBTR_FAIL != GetResult());
}

void SDBAsyncTask::WriteLog()
{
	SDBAsyncTaskLogWriter w(this);
	w.Write();
}

bool SDBAsyncTask::Execute( mdb::MDatabaseQuery& rfDBQry, const wstring& strSQL )
{
	SetTaskFail();

	SDBTaskExecuter<mdb::MDatabaseQuery> ex(rfDBQry);
	const bool bSuccess = ex.Execute(strSQL);
	ResetStatus(ex.GetStatus());

	if (!bSuccess)
		return false;
	
	SetTaskSuccess();

	return true;
}


bool SDBAsyncTask::Execute(mdb::MDBRecordSet& rfRs, const wstring& strSQL)
{
	SetTaskFail();

	SDBTaskExecuter<mdb::MDBRecordSet> ex(rfRs);
	const bool bSuccess = ex.Execute(strSQL);
	ResetStatus(ex.GetStatus());

	if (!bSuccess)
		return false;

	SetTaskSuccess();

	return true;
}

bool SDBAsyncTask::ExecuteW( mdb::MDatabaseQuery& rfDBQry, const wstring& strSQL )
{
	SetTaskFail();

	SDBTaskExecuter<mdb::MDatabaseQuery> ex(rfDBQry);
	const bool bSuccess = ex.ExecuteW(strSQL);
	ResetStatus(ex.GetStatus());

	if (!bSuccess)
		return false;

	SetTaskSuccess();

	return true;
}

bool SDBAsyncTask::ExecuteW( mdb::MDatabaseQuery& rfDBQry, const wstring& strSQL, int& nRet )
{
	SetTaskFail();

	SDBTaskExecuter<mdb::MDatabaseQuery> ex(rfDBQry);
	const bool bSuccess = ex.ExecuteW(strSQL, nRet);
	ResetStatus(ex.GetStatus());

	if (!bSuccess)
		return false;

	SetTaskSuccess();

	return true;
}

bool SDBAsyncTask::ExecuteW(mdb::MDBRecordSet& rfRs, const wstring& strSQL)
{
	SetTaskFail();

	SDBTaskExecuter<mdb::MDBRecordSet> ex(rfRs);
	const bool bSuccess = ex.ExecuteW(strSQL);
	ResetStatus(ex.GetStatus());

	if (!bSuccess)
		return false;

	SetTaskSuccess();

	return true;
}

bool SDBAsyncTask::ExecuteW( mdb::MDBRecordSet& rfRs, const wstring& strSQL, int& nRet )
{
	SetTaskFail();

	SDBTaskExecuter<mdb::MDBRecordSet> ex(rfRs);
	const bool bSuccess = ex.ExecuteW(strSQL, nRet);
	ResetStatus(ex.GetStatus());

	if (!bSuccess)
		return false;

	SetTaskSuccess();

	return true;
}

void SDBAsyncTask::ResetStatus( mdb::MDatabaseStatus* pStatus )
{
	if (NULL == pStatus)
		return;

	if (NULL != m_pStatus)
		SAFE_DELETE(m_pStatus);

	m_pStatus = pStatus;
}

SDBAsyncTask& SDBAsyncTask::PushSQL( CString& strSQL )
{
	__super::PushSQLW(strSQL.GetString());
	return *this;
}
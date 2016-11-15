#ifndef _SDBTASK_H
#define _SDBTASK_H


#include "MDBRecordSet.h"
#include "MDBThreadTask.h"
#include "MDatabaseQuery.h"
#include "MDatabaseInfo.h"
#include "ServerCommonLib.h"
#include "SDBTaskID.h"
#include "SDatabaseType.h"
#include "MTstring.h"
#include "atlstr.h"
#include "SDBTaskTracer.h"

#include <map>

enum SDBTASK_RESULT
{
	SDBTR_SUCCESS	= 0, 
	SDBTR_FAIL		= 1
};

namespace mdb
{
	class MDatabase;
}


class SCOMMON_API SDBAsyncTask : public mdb::MDBThreadTask
{
public :
	SDBAsyncTask(const SDBT_DBTYPE DBType, const SDBTASK_ID TaskID, const size_t nReserveSQLSize = 1);
	virtual ~SDBAsyncTask();

	SDBTASK_RESULT					GetResult()						{ return m_Result; }	
	mdb::MDatabaseStatus*			GetStatus()						{ return m_pStatus; }
	SDBTASK_ID						GetID()							{ return m_TaskID; }
	SDBT_DBTYPE						GetDBType()						{ return m_DBType; }

	void							SetTaskSuccess()				{ m_Result = SDBTR_SUCCESS; }
	void							SetTaskFail()					{ m_Result = SDBTR_FAIL; }
	bool							IsSuccess()						{ return SDBTR_SUCCESS == m_Result; }

	SDBAsyncTask&					PushSQL(CString& strSQL);

	static SDBTaskTracer&			GetTaskTracer()						{ return m_TaskTracer; }
	
protected :
	bool							CheckResult();
	virtual void					WriteLog();
	void							ResetStatus(mdb::MDatabaseStatus* pStatus);
	virtual bool					Execute(mdb::MDatabaseQuery& rfDBQry, const wstring& strSQL);
	virtual bool					Execute(mdb::MDBRecordSet& rfRs, const wstring& strSQL);
	virtual bool					ExecuteW(mdb::MDatabaseQuery& rfDBQry, const wstring& strSQL);
	virtual bool					ExecuteW(mdb::MDatabaseQuery& rfDBQry, const wstring& strSQL, int& nRet);
	virtual bool					ExecuteW(mdb::MDBRecordSet& rfRs, const wstring& strSQL);
	virtual bool					ExecuteW(mdb::MDBRecordSet& rfRs, const wstring& strSQL, int& nRet);
	void							ExecuteSync(mdb::MDatabase& rfDB, const wstring& strSQL);

protected :
	SDBTASK_RESULT					m_Result;
	mdb::MDatabaseStatus*			m_pStatus;
	SDBT_DBTYPE						m_DBType;
	SDBTASK_ID						m_TaskID;
	static map<SDBTASK_ID, int>		m_mapTraceID;			
	static SDBTaskTracer			m_TaskTracer;
};

#endif
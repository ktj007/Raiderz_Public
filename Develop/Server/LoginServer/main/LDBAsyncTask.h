#ifndef _LASYNCDBTASK_H
#define _LASYNCDBTASK_H


#include "SDBTask.h"
#include "SDBTaskID.h"


class LDBAsyncTask : public SDBAsyncTask
{
public :
	LDBAsyncTask(const SDBT_DBTYPE DBType, const SDBTASK_ID TaskID, const size_t nReserveSQLSize = 1);
	virtual ~LDBAsyncTask();

	virtual mdb::MDB_THRTASK_RESULT	OnCompleted();

	virtual void					_OnPreCheck()					{}								// 결과 검사를 하기전에 실행 되는 부분. 결과와 상관없이 항상 실행된다.
	virtual mdb::MDB_THRTASK_RESULT	_OnCompleted()					{ return mdb::MDBTR_SUCESS; }	// 성공 처리는 이걸 재정의 해서 처리.
	virtual mdb::MDB_THRTASK_RESULT	_OnFailed()						{ return mdb::MDBTR_SUCESS; }	// 실패 처리는 이걸 상속 받아서 처리.
	virtual void					_OnProfile()					{}


protected :
};


#endif
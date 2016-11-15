#ifndef _GDBTASK_UPDATE_H
#define _GDBTASK_UPDATE_H


#include "LDBTask.h"
#include "LDBAsyncTask.h"
#include "MMemPool.h"


/*
아래 쿼리 객체들은 쿼리가 실패를 하더라도 그에 대한 처리를 하지 않고 계속 진행을 한다.
그렇기 때문에 중간에 쿼리가 실패 하더라도 상관이 없는 작업에 대해서나 완료 결과가
필요없는 작업에 대해서만 사용해야 한다.
*/


class LAccountDBTaskQuery : public LDBAsyncTask, public MMemPool<LAccountDBTaskQuery>
{
public :
	LAccountDBTaskQuery(const SDBTASK_ID nID);
	virtual ~LAccountDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();	
};

class LGameDBTaskQuery : public LDBAsyncTask, public MMemPool<LGameDBTaskQuery>
{
public :
	LGameDBTaskQuery(const SDBTASK_ID nID);
	virtual ~LGameDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();	
};

class LLogDBTaskQuery : public LDBAsyncTask, public MMemPool<LLogDBTaskQuery>
{
public :
	LLogDBTaskQuery(const SDBTASK_ID nID);
	virtual ~LLogDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();	
};


#endif

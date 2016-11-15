#ifndef _GDBTASK_UPDATE_H
#define _GDBTASK_UPDATE_H


#include "GDBAsyncTask.h"
#include "MMemPool.h"


/*
아래 쿼리 객체들은 쿼리가 실패를 하더라도 그에 대한 처리를 하지 않고 계속 진행을 한다.
그렇기 때문에 중간에 쿼리가 실패 하더라도 상관이 없는 작업에 대해서나 완료 결과가
필요없는 작업에 대해서만 사용해야 한다.
*/


class GGameDBTaskQuery : public GDBAsyncTask, public MMemPool<GGameDBTaskQuery>
{
public :
	GGameDBTaskQuery(const MUID& uidReqPlayer, const SDBTASK_ID nTaskID);
	virtual ~GGameDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;	
};

class GGameDBTaskQueryIter : public GDBAsyncTask, public MMemPool<GGameDBTaskQueryIter>
{
public :
	GGameDBTaskQueryIter(const MUID& uidReqPlayer, const SDBTASK_ID nTaskID, const size_t nReserveSQLSize);
	virtual ~GGameDBTaskQueryIter();

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;	
};

class GLogDBTaskQuery : public GDBAsyncTask, public MMemPool<GLogDBTaskQuery>
{
public :
	GLogDBTaskQuery(const MUID& uidReqPlayer, const SDBTASK_ID nTaskID);
	virtual ~GLogDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;	
};

#endif
#ifndef _PDBTASK_SQL_H
#define _PDBTASK_SQL_H


#include "SDBTask.h"
#include "MMemPool.h"


class PGameDBTaskQuery : public SDBAsyncTask, public MMemPool<PGameDBTaskQuery>
{
public :
	PGameDBTaskQuery(const SDBTASK_ID TaskID);
	~PGameDBTaskQuery() {}

	void OnExecute(mdb::MDatabase& rfDB) override;

	mdb::MDB_THRTASK_RESULT	OnCompleted() override;
};


class PLogDBTaskQuery : public SDBAsyncTask, public MMemPool<PLogDBTaskQuery>
{
public :
	PLogDBTaskQuery(const SDBTASK_ID TaskID);
	~PLogDBTaskQuery();

	void OnExecute(mdb::MDatabase& rfDB) override;

	mdb::MDB_THRTASK_RESULT	OnCompleted() override;
};


#endif
#ifndef _GDBTASK_SQL_H
#define _GDBTASK_SQL_H



#include "SDBTask.h"
#include "MMemPool.h"


class LAccountDBTaskQuery : public SDBAsyncTask, public MMemPool<LAccountDBTaskQuery>
{
public:
	LAccountDBTaskQuery(const SDBTASK_ID TaskID);
	~LAccountDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT OnCompleted() override;
};

class LGameDBTaskQuery : public SDBAsyncTask, public MMemPool<LGameDBTaskQuery>
{
public :
	LGameDBTaskQuery(const SDBTASK_ID TaskID);
	~LGameDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	OnCompleted() override;	
};

class LLogDBTaskQuery : public SDBAsyncTask, public MMemPool<LLogDBTaskQuery>
{
public :
	LLogDBTaskQuery(const SDBTASK_ID TaskID);
	~LLogDBTaskQuery();

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	OnCompleted() override;	
};



#endif
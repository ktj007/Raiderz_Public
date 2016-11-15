#pragma once

#include "LDBAsyncTask.h"
#include "LDBTaskDataWorld.h"

class LDBTaskGetWorldInfo: public LDBAsyncTask, public MMemPool<LDBTaskGetWorldInfo>
{
public:
	enum { GET_WORLD_INFO = 0	};

public:
	LDBTaskGetWorldInfo();
	virtual ~LDBTaskGetWorldInfo();

	void					OnExecute(mdb::MDatabase& rfDB);
	mdb::MDB_THRTASK_RESULT	_OnCompleted();

private:
	vector<DBTASK_WORLD_INFO>	m_vecWorldInfo;
};

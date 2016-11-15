#ifndef _GDBTASK_QUEST_FAIL_H
#define _GDBTASK_QUEST_FAIL_H


#include "GDBAsyncTask.h"
#include "GDBTaskQuestData.h"
#include "MMemPool.h"


class GEntityPlayer;


class GDBTaskQuestFail : public GDBAsyncTask, public MMemPool<GDBTaskQuestFail>
{
public :
	GDBTaskQuestFail(const MUID& uidReqPlayer);
	~GDBTaskQuestFail();


	enum
	{
		QUEST_FAIL = 0
		,
	};


	void					Input(const GDBT_QUEST_COMMON& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;

	
protected :
	GDBT_QUEST_COMMON m_Data;
};


#endif
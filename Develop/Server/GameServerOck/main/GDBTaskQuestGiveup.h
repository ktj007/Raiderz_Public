#ifndef _GDBTASK_QUEST_GIVEUP_H
#define _GDBTASK_QUEST_GIVEUP_H


#include "GDBAsyncTask.h"
#include "GDBTaskQuestData.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GDBTaskQuestGiveup : public GDBAsyncTask, public MMemPool<GDBTaskQuestGiveup>
{
public :
	GDBTaskQuestGiveup(const MUID& uidReqPlayer);
	~GDBTaskQuestGiveup();

	enum
	{
		QUEST_GIVEUP = 0
		,
	};


	bool					Input(GDBT_QUEST_GIVEUP& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;


protected :
	bool					BeginCacheSync(GDBT_QUEST_GIVEUP& data);
	void					_OnPreCompleted() override;

	
private :
	class Completer
	{
	public :
		Completer(GDBT_QUEST_GIVEUP& Data);
		void Do();

	private :
		GDBT_QUEST_GIVEUP& m_Data;
	};


protected :
	GSyncingCacheList	m_SyncingList;
	GDBT_QUEST_GIVEUP	m_Data;
};


#endif
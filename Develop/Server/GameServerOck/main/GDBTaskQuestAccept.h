#ifndef _GDBTASK_QUEST_ACCEPT_H
#define _GDBTASK_QUEST_ACCEPT_H


#include "GDBAsyncTask.h"
#include "GDBTaskQuestData.h"
#include "MMemPool.h"
#include "GSyncingCacheList.h"


class GDBTaskQuestAccept : public GDBAsyncTask, public MMemPool<GDBTaskQuestAccept>
{
public :
	GDBTaskQuestAccept(const MUID& uidReqPlayer);
	~GDBTaskQuestAccept();

	enum
	{
		QUEST_ACCEPT = 0
		,
	};

	bool					Input(GDBT_QUEST_ACCEPT& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed()	override;
	virtual void			_OnPreCompleted() override;
	
	void EndInteraction(const MUID& uidPlayer);

	
private :
	class Completer
	{
	public :
		Completer(GDBT_QUEST_ACCEPT& Data);
		void Do();

	private :
		GDBT_QUEST_ACCEPT& m_Data;
	};


protected :
	GDBT_QUEST_ACCEPT	m_Data;	
	GSyncingCacheList	m_SyncingList;
};


#endif
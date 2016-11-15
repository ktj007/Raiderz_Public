#ifndef _GDBTASK_QUEST_DONE_H
#define _GDBTASK_QUEST_DONE_H


#include "GDBAsyncTask.h"
#include "GITEM_STACK_AMT.h"
#include "GDBTaskQuestData.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GDBTaskQuestDone : public GDBAsyncTask, public MMemPool<GDBTaskQuestDone>
{
public : 
	GDBTaskQuestDone(const MUID& uidReqPlayer);
	~GDBTaskQuestDone();

	enum
	{
		QUEST_DONE = 0
		,
	};


	bool					Input(GDBT_QEUST_DONE& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;


protected :
	bool					BeginCacheSync(GDBT_QEUST_DONE& data);
	void					_OnPreCompleted() override;
	
private :
	class Completer
	{
	public :
		Completer(GDBT_QEUST_DONE& Data, const bool bSuccess) : m_Data(Data), m_bSuccess(bSuccess) {}
		void Do();
		
	private :
		GDBT_QEUST_DONE&	m_Data;
		bool				m_bSuccess;
	};


public :
	GDBT_QEUST_DONE				m_Data;	
	GSyncingCacheList			m_SyncingList;
};


#endif
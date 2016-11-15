#ifndef _GDBTASK_ITEM_MERGE_H
#define _GDBTASK_ITEM_MERGE_H


#include "GDBAsyncTask.h"
#include "GDBTaskItemData.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GDBTaskItemMerge : public GDBAsyncTask, public MMemPool<GDBTaskItemMerge>
{
public :
	GDBTaskItemMerge(const MUID& uidReqPlayer);
	~GDBTaskItemMerge();

	enum
	{
		ITEM_MERGE = 0,
	};

	bool Input(const GDBT_ITEM_MERGE_AND_SPLIT& data);


protected :
	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	void					_OnPreCompleted() override;
	bool					BeginItemCacheSync(const MUID& uidPlaeyr, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID);
	mdb::MDB_THRTASK_RESULT _OnFailed() override;

protected :
	class Completer
	{
	public :
		Completer(GDBT_ITEM_MERGE_AND_SPLIT& Data) : m_Data(Data) {}
		
		void Do();

	private :
		GDBT_ITEM_MERGE_AND_SPLIT& m_Data;
	};


protected :
	GSyncingCacheList	m_SyncingList;
	GDBT_ITEM_MERGE_AND_SPLIT		m_Data;
};


#endif
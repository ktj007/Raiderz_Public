#ifndef _GDBTASK_ITEM_ENCHANT_H
#define _GDBTASK_ITEM_ENCHANT_H


#include "GDBAsyncTask.h"
#include "GDBTaskItemData.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GDBTaskItemEnchant : public GDBAsyncTask, public MMemPool<GDBTaskItemEnchant>
{
public :
	GDBTaskItemEnchant(const MUID& uidReqPlayer);

	enum
	{
		ENCHANT = 0
	};

	bool Input(const GDBT_ITEM_ENCH& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	void					_OnPreCompleted() override;

protected :
	bool CharSync(const MUID& uidPlayer);
	bool ItemSync(const MUID& uidPlayer, const SH_ITEM_SLOT_TYPE nSlotType, const int16 nSlotID, const IUID nIUID);

private :
	GSyncingCacheList	m_SyncingList;
	GDBT_ITEM_ENCH		m_Data;
};


#endif
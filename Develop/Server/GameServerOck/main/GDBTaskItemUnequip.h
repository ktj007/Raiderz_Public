#ifndef _GDBTASK_ITEM_UNEQUIP_H
#define _GDBTASK_ITEM_UNEQUIP_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataItemEquip.h"
#include "GItemDBCacheSyncer.h"
#include "MMemPool.h"


class GDBTaskItemUnequip : public GDBAsyncTask, public MMemPool<GDBTaskItemUnequip>
{
public :
	GDBTaskItemUnequip(const MUID& uidReqPlayer);
	~GDBTaskItemUnequip();

	enum
	{
		ITEM_UNEQUIP = 0,
	};

	bool Input(GDBT_ITEM_UNEQUIP& data);


	void					OnExecute(mdb::MDatabase& rfDB) override;
	virtual void			_OnPreCompleted()	override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

protected:
	bool BeginCacheSync(GDBT_ITEM_UNEQUIP& data);
	bool SyncItem(const MUID& uidPlayer, const IUID nIUID);
	void EndCacheSync( const bool bZero );

private:
	void MakeSyncNeedIUID(GDBT_ITEM_UNEQUIP& data, GDBSYNC_ITEM_VEC& vSyncItem);

protected :
	GDBT_ITEM_UNEQUIP	m_Data;
	GItemDBCacheSyncer  m_Syncer;
};



#endif
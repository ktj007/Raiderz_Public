#ifndef _GDBTASK_ITEM_EQUIP_H
#define _GDBTASK_ITEM_EQUIP_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataItemEquip.h"
#include "GItemDBCacheSyncer.h"
#include "MMemPool.h"


class GDBTaskItemEquip : public GDBAsyncTask, public MMemPool<GDBTaskItemEquip>
{
public :
	GDBTaskItemEquip(const MUID& uidReqPlayer);
	~GDBTaskItemEquip();

	enum
	{
		ITEM_EQUIP = 0,
	};

	bool Input(GDBT_ITEM_EQUIP& data);	
	
	void					OnExecute(mdb::MDatabase& rfDB) override;
	virtual void			_OnPreCompleted()	override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;	

private:
	void MakeSyncNeedIUID(GDBT_ITEM_EQUIP &data, GDBSYNC_ITEM_VEC& vSyncItem);

protected :
	GDBT_ITEM_EQUIP		m_Data;
	GItemDBCacheSyncer  m_Syncer;
};


#endif
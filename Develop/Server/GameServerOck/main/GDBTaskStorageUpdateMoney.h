#pragma once

#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GDBTaskDataStorage.h"

class GDBTaskStorageUpdateMoney : public GDBAsyncTask, public MMemPool<GDBTaskStorageUpdateMoney>
{
public:
	GDBTaskStorageUpdateMoney(const MUID& uidReqPlayer);
	~GDBTaskStorageUpdateMoney(void);

	bool					Input(const GDBT_STORAGE_UPDATE_MONEY& data);
	void					OnExecute(mdb::MDatabase& rfDB) override;
	virtual void			_OnPreCompleted()	override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

	GDBT_STORAGE_UPDATE_MONEY	m_data;
};

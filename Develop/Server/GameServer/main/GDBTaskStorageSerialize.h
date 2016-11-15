#pragma once

#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GDBTaskDataStorage.h"


class GDBTaskStorageSerialize : public GDBAsyncTask, public MMemPool<GDBTaskStorageSerialize>
{
public:
	GDBTaskStorageSerialize(const MUID& uidReqPlayer);
	~GDBTaskStorageSerialize(void);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

	GDBT_STORAGE_SERIALIZE	m_data;
};

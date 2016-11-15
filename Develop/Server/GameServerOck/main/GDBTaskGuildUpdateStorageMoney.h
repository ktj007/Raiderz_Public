#pragma once

#include "GDBAsyncTask.h"
#include "MMemPool.h"
#include "GDBTaskDataGuild.h"

class GDBTaskGuildUpdateStorageMoney : public GDBAsyncTask, public MMemPool<GDBTaskGuildUpdateStorageMoney>
{
public:
	GDBTaskGuildUpdateStorageMoney(const MUID& uidReqPlayer);
	~GDBTaskGuildUpdateStorageMoney(void);

	bool					Input(const GDBT_GUILD_UPDATE_STORAGEMONEY& data);
	void					OnExecute(mdb::MDatabase& rfDB) override;
	virtual void			_OnPreCompleted()	override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;
	GDBT_GUILD_UPDATE_STORAGEMONEY	m_data;
};

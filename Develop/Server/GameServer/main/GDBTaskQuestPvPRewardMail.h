#ifndef _GDBTASK_QUEST_PVP_REWARD_MAIL_H
#define _GDBTASK_QUEST_PVP_REWARD_MAIL_H

#include "GDBAsyncTask.h"
#include "GDBTaskDataQuestPVP.h"
#include "MMemPool.h"
#include "GSyncingCacheList.h"

class GDBTaskQuestPvPRewardMail : public GDBAsyncTask, public MMemPool<GDBTaskQuestPvPRewardMail>
{
public :
	GDBTaskQuestPvPRewardMail(const MUID& uidPlayer);

	enum
	{
		QPVP_REWARD_MAIL = 0,
	};

	bool					Input(GDBT_QPER_TOMAIL& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed()	override;

protected :
	void					_OnPreCompleted() override;

	bool					BeginCharCacheSync(GDBT_QPER_TOMAIL& data);

protected :
	GDBT_QPER_TOMAIL	m_Data;
	GSyncingCacheList	m_SyncingList;
};

#endif
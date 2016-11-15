#ifndef _GDBTASK_QUEST_PVP_REWARD_INVEN
#define _GDBTASK_QUEST_PVP_REWARD_INVEN

#include "GDBAsyncTask.h"
#include "GDBTaskDataQuestPVP.h"
#include "MMemPool.h"
#include "GSyncingCacheList.h"

class GDBTaskQuestPvPRewardInven : public GDBAsyncTask, public MMemPool<GDBTaskQuestPvPRewardInven>
{
public :
	GDBTaskQuestPvPRewardInven(const MUID& uidPlayer);

	enum
	{
		QPVP_REWARD_INVEN = 0
	};

	bool Input(GDBT_QPER_TOINVEN& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT _OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed()	override;

protected :
	void					_OnPreCompleted() override;

	bool					BeginCharCacheSync(GDBT_QPER_TOINVEN& data);
	bool					BeginItemCacheSync(GDBT_QPER_TOINVEN& data);

	void					AddQPvPRewardItem(GEntityPlayer* pPlayer);
	bool					IsSameSlotID(GDBT_QPER_ITEM& qi, pair<int16, int64> iuid);
	bool					IsNewRewardItem(GDBT_QPER_ITEM& qi);
	bool					SetRwardItemUID(GDBT_QPER_ITEM& qi, pair<int16, int64> iuid);
	
protected :
	GDBT_QPER_TOINVEN	m_Data;
	GSyncingCacheList	m_SyncingList;
};

#endif
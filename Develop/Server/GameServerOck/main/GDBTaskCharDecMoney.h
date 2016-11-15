#ifndef _GDBTASK_CHAR_DEC_MONEY_H
#define _GDBTASK_CHAR_DEC_MONEY_H


#include "GDBAsyncTask.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"
#include "GDBTaskCharData.h"


class GDBTaskCharDecMoney : public GDBAsyncTask, public MMemPool<GDBTaskCharDecMoney>
{
public :
	GDBTaskCharDecMoney(const MUID& uidPlayer, SDBTASK_ID TaskID);

	enum
	{
		LOOT_MONEY = 0,
	};

	bool Input(const GDBT_CHAR_MONEY_INC_DEC& data);

	bool UpdateCache( const int nDecMoney, const int nMoney );
	virtual void			_OnPreCompleted() override;
	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

protected :
	GDBT_CHAR_MONEY_INC_DEC	m_Data;

	GSyncingCacheList	m_SyncingList;	
};


#endif
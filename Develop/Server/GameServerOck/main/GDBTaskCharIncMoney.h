#ifndef _GDBTASK_CHAR_INC_MONEY_H
#define _GDBTASK_CHAR_INC_MONEY_H


#include "GDBAsyncTask.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"
#include "GDBTaskCharData.h"


class GDBTaskCharIncMoney : public GDBAsyncTask, public MMemPool<GDBTaskCharIncMoney>
{
public :
	GDBTaskCharIncMoney(const MUID& uidPlayer, SDBTASK_ID TaskID);

	enum
	{
		LOOT_MONEY = 0,
	};

	bool Input(const GDBT_CHAR_MONEY_INC_DEC& data);

	bool					UpdateCache( const int nIncMoney, const int nMoney );
	virtual void			_OnPreCompleted() override;
	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;

protected :
	GDBT_CHAR_MONEY_INC_DEC m_Data;

	int64				m_nAID;
	int64				m_nCID;
	int					m_nCharPtm;
	int					m_nDeltaCharPtm;
	int					m_nXP;
	int					m_nIncMoney;
	int					m_nMoney;
	int					m_nLogCode;
	int					m_nNpcID;

	GSyncingCacheList	m_SyncingList;

};


#endif
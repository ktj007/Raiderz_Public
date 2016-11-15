#ifndef _GDBTAS_TRADE_H
#define _GDBTAS_TRADE_H


#include "GDBAsyncTask.h"
#include "GDBTaskDataTrade.h"
#include "GPlayerTrade.h"
#include "GSyncingCacheList.h"
#include "MMemPool.h"


class GPlayerDBCache;


class GTradeSyncingCacheList : public GSyncingCacheList, public MMemPool<GTradeSyncingCacheList>
{
public :
	GTradeSyncingCacheList() : GSyncingCacheList(0) {}
	
};


class GDBTaskTrade : public GDBAsyncTask, public MTestMemPool<GDBTaskTrade>
{
public :
	GDBTaskTrade();
	~GDBTaskTrade();

	enum
	{
		TRADE = 0
	};

	bool Input(GTRADE_CHAR_VEC& vTradeChar);

	void					OnExecute(mdb::MDatabase& rfDB) override;

	GDBT_TRADE_DATA&		GetData();


protected :
	void					_OnPreCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;
	
	bool					BeginCacheSync(GTRADE_CHAR_VEC& vTradeChar);
	bool					SyncCharacter(const MUID& uidPlayer, const CHAR_ORDER nOrdNum);
	bool					SyncItem(const MUID& uidPlayer, GTRADE_ITEM_VEC& vItem, const CHAR_ORDER nOrdNum);
	void					EndSync(GTRADE_CHAR_VEC& vTradeChar, const CHAR_ORDER nOrdNum, const bool bZero);

	
protected :	
	class Completer
	{
	public :
		Completer(GDBT_TRADE_DATA& Data) : m_Data(Data) {}
		~Completer() {}

		void Do();
	private :
		GDBT_TRADE_DATA& m_Data;
	};


protected :
	GDBT_TRADE_DATA			m_Data;
	GTradeSyncingCacheList	m_SyncingList[TRADE_CHAR_SIZE];
};


#endif
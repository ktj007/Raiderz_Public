#pragma once

#include "GDBAsyncTask.h"
#include "GItemDBCacheSyncer.h"
#include "MMemPool.h"
#include "GDBTaskTradeMarketData.h"

class GDBTaskTradeMarketPutOn : public GDBAsyncTask,  public MMemPool<GDBTaskTradeMarketPutOn>
{
public :
	GDBTaskTradeMarketPutOn(const MUID& uidReqPlayer);
	~GDBTaskTradeMarketPutOn();

	bool Input(const GDBT_TRADEMARKET_PUT_ON& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	virtual void			_OnPreCompleted()	override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;

protected :
	GDBT_TRADEMARKET_PUT_ON	m_Data;
	GItemDBCacheSyncer		m_Syncer;
};
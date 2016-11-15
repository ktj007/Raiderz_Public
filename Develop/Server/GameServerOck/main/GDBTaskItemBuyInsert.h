#ifndef _GDBTASK_ITEM_BUY_INSERT_H
#define _GDBTASK_ITEM_BUY_INSERT_H


#include "GDBTaskNPCShop.h"
#include "MMemPool.h"


class GEntityPlayer;


class GDBTaskItemBuyInsert : public GDBTaskNPCShopTrade, public MMemPool<GDBTaskItemBuyInsert>
{
public :
	GDBTaskItemBuyInsert(const MUID& uidReqPlayer);
	~GDBTaskItemBuyInsert();

	enum
	{
		ITEM_BUY = 0
		,
	};

	bool Input(const GDBT_NPC_SHOP_TRADE_DATA& InputData);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
	mdb::MDB_THRTASK_RESULT	_OnFailed() override;
	

protected :
	struct _RESULT
	{
		_RESULT() : nIUID(0) {}

		IUID nIUID;
	};

	class Completer
	{
	public :
		Completer(GDBT_NPC_SHOP_TRADE_DATA& Data, _RESULT& Result);

		void Do();

	private :
		void InsertItem();
		void DecreaseMoney(GEntityPlayer* pPlayer);

	protected :
		GDBT_NPC_SHOP_TRADE_DATA&	m_Data;		
		_RESULT&					m_Result;
	};


protected :
	GDBT_NPC_SHOP_TRADE_DATA	m_InputData;
	_RESULT						m_Result;
};

#endif
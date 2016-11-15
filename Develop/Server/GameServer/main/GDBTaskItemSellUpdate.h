#ifndef _GDBTASK_ITEM_SELL_UPDATE_H
#define _GDBTASK_ITEM_SELL_UPDATE_H


#include "GDBTaskNPCShop.h"
#include "MMemPool.h"


class GEntityPlayer;

class GDBTaskItemSellUpdate : public GDBTaskNPCShopTrade, public MMemPool<GDBTaskItemSellUpdate>
{
public :
	GDBTaskItemSellUpdate(const MUID& uidReqPlayer);
	~GDBTaskItemSellUpdate();

	enum
	{
		ITEM_SELL = 0
		,
	};

	bool Input(const GDBT_NPC_SHOP_TRADE_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;	

	
protected :
	class Completer
	{
	public :
		Completer(GDBT_NPC_SHOP_TRADE_DATA& Data) : m_Data(Data) {}
		~Completer() {}

		void Do();

	private :
		void UpdateItem(GEntityPlayer* pPlayer);
		void IncreaseMoney(GEntityPlayer* pPlayer);

	private :
		GDBT_NPC_SHOP_TRADE_DATA& m_Data;
	};


protected :
	GDBT_NPC_SHOP_TRADE_DATA m_Data;
};


#endif
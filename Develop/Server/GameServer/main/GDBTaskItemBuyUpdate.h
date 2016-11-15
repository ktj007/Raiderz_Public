#ifndef _GDBTASK_ITEM_BUY_UPDAATE_H
#define _GDBTASK_ITEM_BUY_UPDAATE_H


#include "GDBTaskNPCShop.h"
#include "MMemPool.h"


class GDBTaskItemBuyUpdate : public GDBTaskNPCShopTrade, public MMemPool<GDBTaskItemBuyUpdate>
{
public :
	GDBTaskItemBuyUpdate(const MUID& uidReqPlayer);
	~GDBTaskItemBuyUpdate();

	enum
	{
		ITEM_BUY = 0
		,
	};

	bool Input(const GDBT_NPC_SHOP_TRADE_DATA& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;
		
protected :
	class Completer
	{
	public :
		Completer(GDBT_NPC_SHOP_TRADE_DATA& Data);

		void Do();

	private :
		void UpdateItem(GEntityPlayer* pPlayer);		

	protected :
		GDBT_NPC_SHOP_TRADE_DATA& m_Data;		
	};

private :
	GDBT_NPC_SHOP_TRADE_DATA m_Data;
};

#endif
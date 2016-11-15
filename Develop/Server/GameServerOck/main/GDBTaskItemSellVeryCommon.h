#ifndef _GDBTASK_ITEM_SELL_VERYCOMMON_H
#define _GDBTASK_ITEM_SELL_VERYCOMMON_H


#include "GDBTaskNPCShop.h"
#include "MMemPool.h"


class GEntityPlayer;


class GDBTaskItemSellVeryCommon : public GDBTaskNPCShopTrade, public MMemPool<GDBTaskItemSellVeryCommon>
{
public :
	GDBTaskItemSellVeryCommon(const MUID& uidReqPlayer);
	~GDBTaskItemSellVeryCommon();

	enum
	{
		ITEM_SELL
	};


	bool Input(GDBT_SELL_ITEM_VERY_COMMON& data);

	void					OnExecute(mdb::MDatabase& rfDB) override;
	mdb::MDB_THRTASK_RESULT	_OnCompleted() override;


protected :
	bool BeginCacheSync(GDBT_SELL_ITEM_VERY_COMMON& data);
	

protected :
	class Completer
	{
	public :
		Completer(GDBT_SELL_ITEM_VERY_COMMON& Data) : m_Data(Data) {}
		~Completer() {}

		void Do();


	private :
		void DeleteItem(GEntityPlayer* pPlayer);
		void IncreaseMoney(GEntityPlayer* pPlayer);


	private :
		GDBT_SELL_ITEM_VERY_COMMON& m_Data;
	};


protected :
	GDBT_SELL_ITEM_VERY_COMMON m_Data;
};


#endif
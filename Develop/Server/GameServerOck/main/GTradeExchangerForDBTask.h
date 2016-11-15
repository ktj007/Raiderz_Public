#pragma once

struct GDBT_TRADE_DATA;
struct GDBT_TRADE_ITEM;
struct GDBT_TRADE_RESULT;

class GItem;
class GEntityPlayer;
class GTRADE_CHAR;
class GTRADE_ITEM_LOG;

class GTradeExchangerForDBTask : public MTestMemPool<GTradeExchangerForDBTask>
{
public:
	void Exchange(GDBT_TRADE_DATA& data);

private:
	void UpdateMoney(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2, int nMoney1, int nMoney2);
	void TradeServerItem(GEntityPlayer* pPlayer, GEntityPlayer* pToPlayer, GTRADE_CHAR& TradeChar, GDBT_TRADE_RESULT& Res, GDBT_TRADE_RESULT& ToRes);
	void ChangeOwner(GEntityPlayer* pPlayer, GEntityPlayer* pToPlayer, GItem* pItem, int nSlotID, GDBT_TRADE_RESULT& Res, GDBT_TRADE_RESULT& ToRes );
	void DecreaseItemBySplit(GEntityPlayer* pPlayer, GItem* pItem, const int16 nTradeAmt, GDBT_TRADE_RESULT& Res );
	void InsertItemBySplit(GEntityPlayer* pPlayer, GEntityPlayer* pSender, const vector<GDBT_TRADE_ITEM>& vecDBInsertItemBySplit, GDBT_TRADE_RESULT& Res);	
};

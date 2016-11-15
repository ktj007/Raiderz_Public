#ifndef _GTRADE_EXCHANGER_H_
#define _GTRADE_EXCHANGER_H_

#include "GPlayerTrade.h"

class GEntityPlayer;
class GTradeItem;
class GTRADE_CHAR;

TEST_FORWARD_FT(TradeExchanger, Fixture, Exchange_InvalidUsagePeriod);
TEST_FORWARD_FT(TradeExchanger, Fixture, Exchange_InvalidExpirationPeriod);


class GTradeExchanger : public MTestMemPool<GTradeExchanger>
{
TEST_FRIEND_FT(TradeExchanger, Fixture, Exchange_InvalidUsagePeriod);
TEST_FRIEND_FT(TradeExchanger, Fixture, Exchange_InvalidExpirationPeriod);

public:	
	bool Exchange(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2);

private:
	bool Check(GEntityPlayer* pPlayer1, const vector<GTradeItem*>& vecP1GiveItem, int nP1GiveMoney, GEntityPlayer* pPlayer2, vector<GTradeItem*> vecP2GiveItem, int nP2GiveMoney);
	bool CheckMoney(int nP1OwnMoney, int nP1GiveMoney, int nP2OwnMoney, int nP2GiveMoney);
	bool CheckItem(GEntityPlayer* pPlayer, const vector<GTradeItem*>& vecTradeItem);	
	
	bool DB(GEntityPlayer* pPlayer1, const vector<GTradeItem*> &vecP1GiveItem, int nP1GiveMoney,
			GEntityPlayer* pPlayer2, const vector<GTradeItem*> &vecP2GiveItem, int nP2GiveMoney);
	void CancelReservedSlotID(GEntityPlayer* pPlayer, GTRADE_CHAR& TradeChar, size_t nSlotIDCount);
	bool MakeTradeItemInfo(GEntityPlayer* pPlayer
		, const vector<GTradeItem*>& vecP1GiveItem
		, GTRADE_CHAR& TradeCharFrom
		, GTRADE_CHAR& TradeCharTo
		, const size_t nReqSlotCnt);
};

#endif

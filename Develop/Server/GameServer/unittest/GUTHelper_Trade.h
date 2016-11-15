#pragma once

class GEntityPlayer;
class GItem;

class GUTHelper_Trade
{
public:
	static void SetRequestState(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2);
	static void SetTradeState(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2);
	static void SetConfirmState(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2);
		
	static void FullTradeItem(GEntityPlayer* pPlayer);
	static void AddTradeItem(GEntityPlayer* pPlayer, GItem* pItem, int nAmount = 0);
};

#pragma once

class GEntityPlayer;

class GTradeChecker : public MTestMemPool<GTradeChecker>
{
public:
	bool CancelInvalidTrade(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	bool CheckTrader(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	bool CheckDistance(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	bool CheckEnemy(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
};

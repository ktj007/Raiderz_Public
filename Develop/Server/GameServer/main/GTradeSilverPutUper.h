#ifndef _G_TRADE_MONEYR_PUTUPER_H_
#define _G_TRADE_MONEYR_PUTUPER_H_

class GEntityPlayer;

class GTradeMoneyPutUper : public MTestMemPool<GTradeMoneyPutUper>
{
private:
	bool Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney);

	bool CheckMoney(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney);
	void Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney);
	void Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney);

public:
	bool PutUp(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nMoney);

};

#endif
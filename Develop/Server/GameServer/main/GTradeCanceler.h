#ifndef _G_TRADE_CANCELER_H_
#define _G_TRADE_CANCELER_H_

class GEntityPlayer;

class GTradeCanceler : public MTestMemPool<GTradeCanceler>
{
private:
	bool Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	void Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);	

public:
	bool Cancel(GEntityPlayer* pReqPlayer);
};

#endif

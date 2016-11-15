#ifndef _GTRADE_CONFIRMER_H_
#define _GTRADE_CONFIRMER_H_

class GEntityPlayer;

class GTradeConfirmer : public MTestMemPool<GTradeConfirmer>
{
public:
	bool Confirm(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);

private:
	bool Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);	

	void Apply(GEntityPlayer* pReqPlayer);
	void Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);	
};

#endif

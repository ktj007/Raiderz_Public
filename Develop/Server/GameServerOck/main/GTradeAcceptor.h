#ifndef _GTRADE_ACCEPTER_H_
#define _GTRADE_ACCEPTER_H_

class GEntityPlayer;

class GTradeAcceptor : public MTestMemPool<GTradeAcceptor>
{
public:
	bool Accept(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept);

private:
	bool Check(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept);
	void Apply(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept);
	void Route(GEntityPlayer* pAcceptPlayer, GEntityPlayer* pTarPlayer, bool bAccept);
};


#endif

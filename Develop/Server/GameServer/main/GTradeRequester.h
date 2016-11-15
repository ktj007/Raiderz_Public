#ifndef _GTRADE_REQUESTER_H_
#define _GTRADE_REQUESTER_H_

class GEntityPlayer;

class GTradeRequester : public MTestMemPool<GTradeRequester>
{
private:
	bool Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	bool CheckSelfRequest(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	bool CheckTargetBusy( GEntityPlayer* pTarPlayer, GEntityPlayer* pReqPlayer );

	void Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	void Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);

public:
	bool Request(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	
};


#endif
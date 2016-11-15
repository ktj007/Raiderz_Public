#ifndef _GTRADE_ITEM_PUTDOWNER_H_
#define _GTRADE_ITEM_PUTDOWNER_H_

class GEntityPlayer;

class GTradeItemPutDowner : public MTestMemPool<GTradeItemPutDowner>
{
public:
	bool PutDown(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount);

private:
	bool Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount);

	bool CheckItemAmount(GEntityPlayer* pReqPlayer,  int nAmount, int nSlotID);
	void Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount);
	void Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount);
};

#endif

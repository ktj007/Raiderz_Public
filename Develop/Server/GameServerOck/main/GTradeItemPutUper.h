#ifndef _GTRADE_ITEM_PUTUPER_H_
#define _GTRADE_ITEM_PUTUPER_H_

class GEntityPlayer;
class GItem;
class GItemData;

class GTradeItemPutUper : public MTestMemPool<GTradeItemPutUper>
{
public:
	bool PutUp(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nSlotID, int nAmount);

private:
	bool Check(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, GItem* pItem, int nAmount);
	bool CheckTradeSpace(GEntityPlayer* pReqPlayer);
	bool CheckTradableItem(GEntityPlayer* pReqPlayer, GItem* pItem);
	bool CheckItemAmount(GEntityPlayer* pReqPlayer, GItem* pItem, int nAmount);
	bool CheckItemAddable(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, int nItemID, int nAmount);

	void Apply(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, GItem* pItem, int nAmount);
	void Route(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer, GItem* pItem, int nAmount);
};

#endif

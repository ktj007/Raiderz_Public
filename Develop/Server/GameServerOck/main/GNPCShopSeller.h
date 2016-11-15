#pragma once

class GNPCShopInfo;
class GItem;

class GNPCShopSeller : public MTestMemPool<GNPCShopSeller>
{
public:
	bool Sell(GEntityPlayer* pPlayer, int nNPCShopID, int nInvenSlotID, int nSellAmount);
	bool SellVeryCommon(GEntityPlayer* pPlayer, int nNPCShopID);

	void Sell_Route(GEntityPlayer* pPlayer);
	void SellVeryCommon_Route(GEntityPlayer* pPlayer);

private:
	bool Sell_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItem* pItem, int nSellAmount, int& outnSumOfSellPrice);
	bool Sell_CheckItemAmount(GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount);

	bool Sell_DB(GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount, int nSumOfSellPrice);
	bool Sell_DBDelete(GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount, int nSumOfSellPrice);
	bool Sell_DBDecrease(GEntityPlayer* pPlayer, GItem* pItem, int nSellAmount, int nSumOfSellPrice);	

	bool SellVeryCommon_Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, vector<GItem*> vecVeryCommonItem, int& outnSumOfSellPrice);
	bool SellVeryCommon_DB(GEntityPlayer* pPlayer, vector<GItem*>& vecItem, int nSumOfSellPrice);

	bool CheckSellabeItem(GItem* pItem, GEntityPlayer* pPlayer);	
	bool CheckMoney(GEntityPlayer* pPlayer, int nSumOfSellPrice);
	vector<GItem*> CollectSellableVeryCommonItem(GEntityPlayer* pPlayer);

	int CalcSumOfSellPrice(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItem* pItem, int nSellAmount);
	int CalcSumOfSellPrice(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, vector<GItem*> vecItem);
};

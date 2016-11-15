#pragma once

class GNPCShopInfo;
class GItemData;
class GItem;

class GNPCShopBuyer : public MTestMemPool<GNPCShopBuyer>
{
public:
	bool Buy(GEntityPlayer* pPlayer, int nNPCShopID, int nItemID, int nItemAmount);	
	void Route( GEntityPlayer* pPlayer);

private:
	bool Check(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItemData* pItemData, int nBuyAmount, int& outnSumOfBuyPrice);
	bool CheckItemAmount(GEntityPlayer* pPlayer, int nBuyAmount, int nMaxStackAmount);
	bool CheckShopCondition(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItemData* pItemData);
	bool CheckItemAddable(GEntityPlayer* pPlayer,  GItemData* pItemData, int nBuyAmount);
	bool CheckMoney(GEntityPlayer* pPlayer, int nSumOfBuyPrice);

	int CalcSumOfBuyPrice(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, GItemData* pItemData, int nBuyAmount);	

	bool DB(GEntityPlayer* pPlayer, GItemData* pItemData, int nBuyAmount, int nSumOfBuyPrice);	
	bool DB_Insert(GEntityPlayer* pPlayer, GItemData* pItemData, int nBuyAmount, int nRemainMoney, int nSumOfBuyPrice);
	bool DB_Increase(GEntityPlayer* pPlayer, GItem* pItem, int nBuyAmount, int nRemainMoney, int nSumOfBuyPrice);	
};

#pragma once

class GNPCShopInfo;

class GNPCShopShower : public MTestMemPool<GNPCShopShower>
{
public:
	bool Show(GEntityPlayer* pPlayer, int nNPCShopID);

private:
	void Route(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo);

	void CategorizeShopItem(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, 
		vector<int>& vecBuyableUsableItemID, vector<int>& vecBuyableUnusableItemID, vector<int>& vecUnbuyableItemID);
};

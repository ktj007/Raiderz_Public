#pragma once

class GNPCShopInfo;

class GUTHelper_NPCShop
{
public:
	static GNPCShopInfo* NewNPCShopInfo(int nID=INVALID_ID);
	
	static GItemData* AddBuyableUsableItem(GNPCShopInfo*  pNPCShopInfo, int nID=INVALID_ID);
	static GItemData* AddBuyableUnusableItem(GNPCShopInfo* pNPCShopInfo, int nID =INVALID_ID);
	static GItemData* AddUnbuyableItem(GNPCShopInfo* pNPCShopInfo, int nID=INVALID_ID);
};

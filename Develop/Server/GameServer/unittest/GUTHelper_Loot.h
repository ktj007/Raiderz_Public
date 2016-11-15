#pragma once

class GLootInfo;
struct LOOT_MONEY_INFO;
struct LOOT_ITEM_INFO;
struct LOOT_SOUL_INFO;

class GUTHelper_Loot
{
public:
	GLootInfo* NewLootInfo();
	LOOT_MONEY_INFO* NewLootMoneyInfo(GLootInfo* pLootInfo, float fRate=100.0f, int nMin=1, int nMax=1);
	LOOT_ITEM_INFO* NewLootItemInfo(GLootInfo* pLootInfo, float fRate=100.0f, int nMin=1, int nMax=1, int nItemID=0);
	LOOT_SOUL_INFO* NewLootSoulInfo(GLootInfo* pLootInfo, SOUL_TYPE nSoulType, float fRate=100.0f, int nMin=1, int nMax=1);
	void PrepareDropItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nItemID=INVALID_ID);
};

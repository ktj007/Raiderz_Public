#include "StdAfx.h"
#include "GUTHelper_Loot.h"
#include "GLootInfo.h"
#include "SUnitTestUtil.h"
#include "GLootInfoMgr.h"
#include "GItemManager.h"
#include "GUTHelper_Item.h"
#include "GTestForward.h"
#include "GHateTable.h"


GLootInfo* GUTHelper_Loot::NewLootInfo()
{
	GLootInfo* pLootInfo = new GLootInfo();
	pLootInfo->m_nID = SUnitTestUtil::NewID();

	gmgr.pLootInfoMgr->Insert(pLootInfo);

	return pLootInfo;
}

LOOT_MONEY_INFO* GUTHelper_Loot::NewLootMoneyInfo(GLootInfo* pLootInfo, float fRate, int nMin, int nMax)
{
	LOOT_MONEY_INFO* pLootMoneyInfo = new LOOT_MONEY_INFO();	
	pLootMoneyInfo->m_fRate = fRate;
	pLootMoneyInfo->m_nMin = nMin;
	pLootMoneyInfo->m_nMax = nMax;
	pLootInfo->m_vecLootMoneys.push_back(pLootMoneyInfo);

	return pLootMoneyInfo;
}

LOOT_ITEM_INFO* GUTHelper_Loot::NewLootItemInfo(GLootInfo* pLootInfo, float fRate, int nMin, int nMax, int nItemID)
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL == pItemData)
	{
		pItemData = GUTHelper_Item::NewItemData();
	}

	LOOT_ITEM_INFO* pLootItemInfo = new LOOT_ITEM_INFO();
	pLootItemInfo->m_nID = pItemData->m_nID;
	pLootItemInfo->m_fRate = fRate;
	pLootItemInfo->m_nMin = nMin;
	pLootItemInfo->m_nMax = nMax;

	pLootInfo->m_vecLootItems.push_back(pLootItemInfo);

	return pLootItemInfo;
}

LOOT_SOUL_INFO* GUTHelper_Loot::NewLootSoulInfo(GLootInfo* pLootInfo, SOUL_TYPE nSoulType, float fRate, int nMin, int nMax)
{
	LOOT_SOUL_INFO* pLootSoulInfo = new LOOT_SOUL_INFO();
	pLootSoulInfo->m_nType = nSoulType;
	pLootSoulInfo->m_fRate = fRate;
	pLootSoulInfo->m_nMin = nMin;
	pLootSoulInfo->m_nMax = nMax;

	pLootInfo->m_vecLootSouls.push_back(pLootSoulInfo);

	return pLootSoulInfo;
}

void GUTHelper_Loot::PrepareDropItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nItemID)
{
	GItemData* pItemData = GUTHelper_Item::NewItemData();

	GLootInfo* pLootInfo = test.loot.NewLootInfo();
	test.loot.NewLootItemInfo(pLootInfo);
	pNPC->GetNPCInfo()->m_pLootInfo = pLootInfo;
	pNPC->GetHateTable().AddPoint_FoundEnemy(pPlayer);	
}

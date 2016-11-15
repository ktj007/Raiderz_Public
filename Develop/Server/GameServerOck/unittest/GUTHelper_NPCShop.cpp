#include "StdAfx.h"
#include "GUTHelper_NPCShop.h"
#include "SUnitTestUtil.h"
#include "GNPCShopInfo.h"
#include "GNPCShopInfoMgr.h"
#include "GTestForward.h"
#include "GConditionsInfo.h"


GNPCShopInfo* GUTHelper_NPCShop::NewNPCShopInfo(int nID/*=INVALID_ID*/)
{
	if (nID == INVALID_ID)	nID = SUnitTestUtil::NewID();

	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nID);
	if (NULL != pNPCShopInfo) return pNPCShopInfo;

	pNPCShopInfo = new GNPCShopInfo();
	pNPCShopInfo->m_nID = nID;
	pNPCShopInfo->m_pDialogInfo = test.dialog.NewDialogInfo();

	gmgr.pNPCShopInfoMgr->Insert(pNPCShopInfo);

	return pNPCShopInfo;
}

GItemData* GUTHelper_NPCShop::AddBuyableUsableItem( GNPCShopInfo* pNPCShopInfo, int nID/*=INVALID_ID*/ )
{
	GItemData* pItemData = test.item.NewItemData(nID);
	pNPCShopInfo->m_vecNormalItem.push_back(pItemData);

	return pItemData;
}

GItemData* GUTHelper_NPCShop::AddBuyableUnusableItem( GNPCShopInfo* pNPCShopInfo, int nID/*=INVALID_ID*/ )
{
	GItemData* pItemData = test.item.NewItemData(nID);
	pNPCShopInfo->m_vecNormalItem.push_back(pItemData);	

	GConditionsInfo* pFailConditionsInfo = test.condition.NewCondI(test.condition.NewCondLForFail(), true);
	pItemData->m_nConditionsInfoID = pFailConditionsInfo->m_nID;

	return pItemData;
}

GItemData* GUTHelper_NPCShop::AddUnbuyableItem(GNPCShopInfo* pNPCShopInfo, int nID)
{
	GItemData* pItemData = test.item.NewItemData(nID);
	GConditionsInfo* pFailConditionsInfo = test.condition.NewCondI(test.condition.NewCondLForFail());

	pNPCShopInfo->m_mapConditionsItem.insert(map<GItemData*, GConditionsInfo*>::value_type(pItemData, pFailConditionsInfo));

	return pItemData;
}


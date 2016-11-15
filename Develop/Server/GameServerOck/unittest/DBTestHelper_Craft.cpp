#include "stdafx.h"
#include "DBTestHelper_Craft.h"
#include "DBTestHelper.h"
#include "GConfig.h"

DBT_CRAFT_CHAR DBTestHelper_Craft::CreateCraftTestCharacter(bool bCreateEmptyItemSlot)
{
	DBT_CRAFT_CHAR cc;

	cc.nAID			= 1;
	cc.nCID			= DBTestHelper::InsertCharacter(cc.nAID, GConfig::m_nMyWorldID);
	cc.nCharPtm		= 123;
	cc.nXP			= 12412;
	cc.nMoney		= 90;
	cc.nModMoney	= 10;
	cc.nActorID		= 4214;

	DBTestHelper::InsertItemInfo(cc.ProdItem.nItemID, L"test_prod_item");

	DBT_RECP_ITEM ritem_1, ritem_2;

	ritem_1.nItemID		= DBTestHelper::InsertItemInfo(2, L"test_recp_item_1");
	ritem_1.nSlotID		= 0;
	ritem_1.nStackAmt	= 10;
	ritem_1.nRecpAmt	= 10;
	ritem_1.nIUID		= DBTestHelper::InsertItem(cc.nAID, cc.nCID, GConfig::m_nMyWorldID, ritem_1.nItemID, ritem_1.nStackAmt, SLOTTYPE_INVENTORY, ritem_1.nSlotID);

	ritem_2.nItemID		= DBTestHelper::InsertItemInfo(3, L"test_recp_item_2");
	ritem_2.nSlotID		= 1;
	ritem_2.nStackAmt	= 10;
	ritem_2.nRecpAmt	= 1;
	ritem_2.nIUID		= DBTestHelper::InsertItem(cc.nAID, cc.nCID, GConfig::m_nMyWorldID, ritem_2.nItemID, ritem_2.nStackAmt, SLOTTYPE_INVENTORY, ritem_2.nSlotID);

	cc.ProdItem.nSlotID				= 10;
	cc.ProdItem.nStackAmt			= 1;
	cc.ProdItem.nDura				= 100;
	cc.ProdItem.nColor				= 5;
	cc.ProdItem.bClaimed			= false;
	cc.ProdItem.bPeriodItem			= true;
	cc.ProdItem.nUsagePeriod		= 3600;
	cc.ProdItem.strExpiDt			= L"2010-9-1 12:4:2";

	if (bCreateEmptyItemSlot)
		DBTestHelper::InsertEmptyItemSlot(cc.nAID, cc.nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, cc.ProdItem.nSlotID);

	cc.vRecpItem.push_back(ritem_1);
	cc.vRecpItem.push_back(ritem_2);

	return cc;
}

wstring DBTestHelper_Craft::MakeRecpInfoToString( DBT_RECP_ITEM& ri, const int16 nRecpAmt )
{
	wchar_t szRecp[1024] = {0,};
	swprintf_s(szRecp, L"%d.%d.%d,", SLOTTYPE_INVENTORY, ri.nSlotID, ri.nStackAmt - nRecpAmt);
	return szRecp;
}

#include "stdafx.h"
#include "GUTHelper_Item.h"
#include "GDBTaskItem.h"
#include "SUnitTestUtil.h"



SUITE(TestItemMoveData)
{
	void SetData(GItem* pItem)
	{
		pItem->m_nSlotType = SLOTTYPE_INVENTORY;
		pItem->m_nSlotID = 3;
		pItem->m_nIUID = 4;
		pItem->m_pItemData->m_nID = 5;
		pItem->m_nSoulCount = 7;
		pItem->m_nDurability = 8;
		pItem->m_nEnchants[0] = 11111; // 강화 1회
		pItem->m_nDyedColor = 11;
		pItem->m_bBind = true;;
		pItem->m_UsagePeriod.Set(true, true, 10, 1);
		pItem->m_strExpiDt = L"2010-1-1 1:0";
	}

	void CHECK_SAME_DATA(GDBT_ITEM& im, GItem* pItem)
	{
		CHECK_EQUAL(im.m_nSlotType		, pItem->m_nSlotType);
		CHECK_EQUAL(im.m_nSlotID		, pItem->m_nSlotID);
		CHECK_EQUAL(im.m_nIUID			, pItem->m_nIUID);
		CHECK_EQUAL(im.m_nItemID		, pItem->m_pItemData->m_nID);
		CHECK_EQUAL(im.m_nStackAmt		, pItem->GetAmount());
		CHECK_EQUAL(im.m_nSoulCnt		, pItem->m_nSoulCount);
		CHECK_EQUAL(im.m_nDura			, pItem->m_nDurability);
		CHECK_EQUAL(im.m_nMaxDura		, pItem->m_nDurability);	// 서버에 아직 구현이 안되어 있어서...
		CHECK_EQUAL(im.m_nEnchCnt		, 1);
		CHECK_EQUAL(im.m_nColor			, pItem->m_nDyedColor);
		CHECK_EQUAL(im.m_bClaimed		, pItem->m_bBind);
		CHECK_EQUAL(im.m_nCharPtm		, pItem->m_UsagePeriod.GetUsageStartCharPlayTimeSec());
		CHECK_EQUAL(im.m_bPeriodItem	, pItem->m_UsagePeriod.IsPeriod() || pItem->m_ExpirationPeriod.IsPeriod());
		CHECK_EQUAL(im.m_nUsagePeriod	, pItem->m_UsagePeriod.GetPeriodSec());

		const wstring strExpiDt = wstring(L"'") + pItem->m_strExpiDt + wstring(L"'");
		CHECK(im.m_strExpiDt == strExpiDt);	
	}

	TEST(TestItemMoveData_Build)
	{
		GItem* pItem = GUTHelper_Item::NewItem(INVALID_ID, 9);
		SetData(pItem);

		GDBT_ITEM im;

		im.Build(SUnitTestUtil::NewCID(), pItem);

		CHECK_SAME_DATA(im, pItem);

		SAFE_DELETE(pItem);
	}
	
}
#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GItemDyer.h"
#include "GConditionsInfo.h"
#include "AStlUtil.h"
#include "GItemHolder.h"
#include "GPlayerDoing.h"


SUITE(ItemDyer)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_vecColor.push_back(1);
			m_vecColor.push_back(200);
			m_vecColor.push_back(32);

			m_pDyeItem = test.item.GainDyeItem(m_pPlayer, m_vecColor);
			m_pTargetItem = test.item.GainArmorItem(m_pPlayer);

			m_pTrader = test.player.NewPlayer(m_pField);

			m_pPlayer->GetDoing().ChangeDoing(CD_DYEING);
		}

		vector<int>		m_vecColor;
		GItem*			m_pDyeItem;
		GItem*			m_pTargetItem;
		GEntityPlayer*	m_pTrader;

		GItemDyer	m_ItemDyer;
	};

	TEST_FIXTURE(Fixture, CheckDyeItemType)
	{
		m_pDyeItem->m_pItemData->m_ItemType = ITEMTYPE_GATHER;

		CHECK_EQUAL(false, m_ItemDyer.Dye(m_pPlayer, m_pTargetItem->m_nSlotType, m_pTargetItem->m_nSlotID, m_pDyeItem->m_nSlotID));
	}

	TEST_FIXTURE(Fixture, CheckDyeItemCondition)
	{
		GConditionsInfo* pConditionsInfo = test.condition.NewCondI(test.condition.NewCondLForFail(), true);
		m_pDyeItem->m_pItemData->m_nConditionsInfoID = pConditionsInfo->m_nID;

		CHECK_EQUAL(false, m_ItemDyer.Dye(m_pPlayer, m_pTargetItem->m_nSlotType, m_pTargetItem->m_nSlotID, m_pDyeItem->m_nSlotID));
	}

	TEST_FIXTURE(Fixture, CheckTargetItemActable)
	{
		m_pPlayer->GetDoing().EndDoing();

		test.trade.SetTradeState(m_pPlayer, m_pTrader);
		test.trade.AddTradeItem(m_pPlayer, m_pTargetItem, 1);

		CHECK_EQUAL(false, m_ItemDyer.Dye(m_pPlayer, m_pTargetItem->m_nSlotType, m_pTargetItem->m_nSlotID, m_pDyeItem->m_nSlotID));
	}

	TEST_FIXTURE(Fixture, CheckTargetItemDyeable)
	{
		m_pTargetItem->m_pItemData->m_bDyeable = false;

		CHECK_EQUAL(false, m_ItemDyer.Dye(m_pPlayer, m_pTargetItem->m_nSlotType, m_pTargetItem->m_nSlotID, m_pDyeItem->m_nSlotID));
	}

	TEST_FIXTURE(Fixture, CheckNowDyeing)
	{
		m_pPlayer->GetDoing().ChangeDoing(CD_NORMAL);
		CHECK_EQUAL(false, m_ItemDyer.Dye(m_pPlayer, m_pTargetItem->m_nSlotType, m_pTargetItem->m_nSlotID, m_pDyeItem->m_nSlotID));
	}

	TEST_FIXTURE(Fixture, Dye)
	{
		int nDyeItemID = m_pDyeItem->m_pItemData->m_nID;		

		CHECK_EQUAL(true, m_ItemDyer.Dye(m_pPlayer, m_pTargetItem->m_nSlotType, m_pTargetItem->m_nSlotID, m_pDyeItem->m_nSlotID));
		CHECK_EQUAL(true, IsExist(m_pDyeItem->m_pItemData->m_vecDyeColor, m_pTargetItem->m_nDyedColor));

		CHECK_EQUAL(false, m_pPlayer->GetItemHolder()->IsExist(SLOTTYPE_INVENTORY, nDyeItemID));
	}
}
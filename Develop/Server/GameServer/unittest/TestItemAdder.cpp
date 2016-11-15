#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GItemManager.h"
#include "GItemAdder.h"
#include "GPlayerQuest.h"
#include "GSystem.h"
#include "GItemHolder.h"
#include "GQuestInfo.h"

SUITE(ItemAdder)
{
	struct FItemAdder : public FBaseGame
	{
		FItemAdder()
		{
			m_pItemData = test.item.NewItemData();
			m_nItemID = m_pItemData->m_nID;
		}

		GItemAdder		m_ItemAdder;
		GItemData*		m_pItemData;
		int				m_nItemID;
	};

	// 추가하려는 아이템의 수량이 잘못된 경우
	TEST_FIXTURE(FItemAdder, CheckItemAddable_CheckAmount)
	{
		int nAmount;

		nAmount = 0;
		CHECK_EQUAL(false, m_ItemAdder.CheckItemAddable(m_pPlayer, CHECK_ADDABLE_ITEM(m_nItemID, nAmount)));

		nAmount = -1;
		CHECK_EQUAL(false, m_ItemAdder.CheckItemAddable(m_pPlayer, CHECK_ADDABLE_ITEM(m_nItemID, nAmount)));

		nAmount = m_pItemData->m_nStackAmount + 1;
		CHECK_EQUAL(false, m_ItemAdder.CheckItemAddable(m_pPlayer, CHECK_ADDABLE_ITEM(m_nItemID, nAmount)));
	}

	// 유니크 아이템을 주었을때 2개이상인 경우
	TEST_FIXTURE(FItemAdder, CheckItemAddable_CheckUnique)
	{
		m_pItemData->m_bUnique = true;

		CHECK_EQUAL(false, m_ItemAdder.CheckItemAddable(m_pPlayer, CHECK_ADDABLE_ITEM(m_nItemID, 2)));
		
		GItem* pItem = test.item.GiveNewItem(m_pPlayer, m_pItemData->m_nID, 1);
		CHECK_EQUAL(false, m_ItemAdder.CheckItemAddable(m_pPlayer, CHECK_ADDABLE_ITEM(m_nItemID, 1)));
	}

	// 인벤토리 슬롯이 부족한 경우
	TEST_FIXTURE(FItemAdder, CheckItemAddable_CheckEmptySlot)
	{
		test.item.FullItemToInven(m_pPlayer);

		CHECK_EQUAL(false, m_ItemAdder.CheckItemAddable(m_pPlayer, CHECK_ADDABLE_ITEM(m_nItemID, 1)));
	}

	// 아이템 추가
	TEST_FIXTURE(FItemAdder, Insert)
	{
		int64	nIUID = SUnitTestUtil::NewIUID();
		int		nItemID = m_pItemData->m_nID;
		int		nInsertAmount = 10;
		int		nAddExpiMin = 10;
		int		nUsagePeriod = 10;
		int		nDurability = 100;
		int		nColor = 5;

		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();
		
		vector<GItem*> vecBeforeItem;
		pItemHolder->CollectItemByID(SLOTTYPE_INVENTORY, nItemID, vecBeforeItem);
		int nBeforeItemBundleAmount = vecBeforeItem.size();

		int nSlotID = pItemHolder->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0);
		CHECK_EQUAL(true, m_ItemAdder.AddForDBTask(m_pPlayer->GetUID(), ADD_ITEM(SLOTTYPE_INVENTORY, nSlotID, nInsertAmount, nInsertAmount, nIUID, nItemID, nAddExpiMin, nUsagePeriod, nDurability, nColor)));
		GItem* pItem = m_pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nSlotID);
		CHECK_EQUAL(nIUID, pItem->m_nIUID);
		CHECK_EQUAL(nItemID, pItem->m_pItemData->m_nID);
		CHECK_EQUAL(nInsertAmount, pItem->GetAmount());
		CHECK_EQUAL((gsys.pSystem->GetNowTime() / 1000) + nUsagePeriod, pItem->m_UsagePeriod.GetPeriodSec());
		CHECK_EQUAL(m_pPlayer->GetPlayerInfo()->nPlayTimeSec, pItem->m_UsagePeriod.GetUsageStartCharPlayTimeSec());
		CHECK_EQUAL((gsys.pSystem->GetNowTime() / 1000) + nAddExpiMin * 60, pItem->m_ExpirationPeriod.GetPeriodSec());
		CHECK_EQUAL(m_pPlayer->GetPlayerInfo()->nPlayTimeSec, pItem->m_ExpirationPeriod.GetUsageStartCharPlayTimeSec());

		vector<GItem*> vecAfterItem;
		pItemHolder->CollectItemByID(SLOTTYPE_INVENTORY, nItemID, vecAfterItem);
		CHECK_EQUAL(nBeforeItemBundleAmount + 1, vecAfterItem.size());
	}

	// 아이템 수량 증가
	TEST_FIXTURE(FItemAdder, Increase)
	{
		GItem* pItem = test.item.GiveNewItem(m_pPlayer, m_pItemData->m_nID, 1);

		int nBeforeItemAmount = pItem->GetAmount();

		CHECK_EQUAL(true, m_ItemAdder.AddForDBTask(m_pPlayer->GetUID(), ADD_ITEM(SLOTTYPE_INVENTORY, pItem->m_nSlotID, pItem->GetAmount() + 10, 10)));
		CHECK_EQUAL(nBeforeItemAmount + 10, pItem->GetAmount());
	}
}
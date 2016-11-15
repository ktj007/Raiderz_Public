#include "stdafx.h"
#include "FBaseGame.h"
#include "GItemRemover.h"
#include "GTestForward.h"
#include "GItemHolder.h"


SUITE(ItemRemover)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{

		}

		GItemRemover	m_ItemRemover;
	};

	TEST_FIXTURE(Fixture, Check)
	{
		GItem* pItem = test.item.GiveNewItem(m_pPlayer);

		CHECK_EQUAL(false, m_ItemRemover.Check(m_pPlayer, pItem, -1));
		CHECK_EQUAL(false, m_ItemRemover.Check(m_pPlayer, pItem, pItem->GetAmount()+1));
	}

	TEST_FIXTURE(Fixture, Delete)
	{
		GItem* pItem = test.item.GiveNewItem(m_pPlayer);
		int nItemID = pItem->GetID();
		SH_ITEM_SLOT_TYPE nSlotType = pItem->m_nSlotType;
		int nSlotID = pItem->m_nSlotID;

		CHECK_EQUAL(true, m_ItemRemover.Remove(m_pPlayer, pItem->GetID(), pItem->GetAmount()));
		CHECK_EQUAL(false, m_pPlayer->GetItemHolder()->IsExist(SLOTTYPE_INVENTORY, nItemID));

		ASSERT_EQUAL(MC_ITEM_REMOVE, m_pLink->GetCommandID(0));

		vector<TD_ITEM_DELETE>		vecTDItemDelete;
		vector<TD_ITEM_DECREASE>	vecTDItemDecrease;
		m_pLink->GetCommand(0).GetBlob(vecTDItemDelete, 0);
		m_pLink->GetCommand(0).GetBlob(vecTDItemDecrease, 1);
		ASSERT_EQUAL(1, vecTDItemDelete.size());
		ASSERT_EQUAL(0, vecTDItemDecrease.size());

		TD_ITEM_DELETE tdItemDelete = vecTDItemDelete.front();		
		CHECK_EQUAL(nSlotType, tdItemDelete.m_nSlotType);
		CHECK_EQUAL(nSlotID, tdItemDelete.m_nSlotID);
	}

	TEST_FIXTURE(Fixture, Decrease)
	{
		GItem* pItem = test.item.GiveNewItem(m_pPlayer, INVALID_ID, 5);
		int nDecreaseAmount = pItem->GetAmount() - 1;
		int nItemID = pItem->GetID();
		SH_ITEM_SLOT_TYPE nSlotType = pItem->m_nSlotType;
		int nSlotID = pItem->m_nSlotID;

		CHECK_EQUAL(true, m_ItemRemover.Remove(m_pPlayer, pItem->GetID(), nDecreaseAmount));
		CHECK_EQUAL(true, m_pPlayer->GetItemHolder()->IsExist(SLOTTYPE_INVENTORY, nItemID));

		ASSERT_EQUAL(MC_ITEM_REMOVE, m_pLink->GetCommandID(0));

		vector<TD_ITEM_DELETE>		vecTDItemDelete;
		vector<TD_ITEM_DECREASE>	vecTDItemDecrease;
		m_pLink->GetCommand(0).GetBlob(vecTDItemDelete, 0);
		m_pLink->GetCommand(0).GetBlob(vecTDItemDecrease, 1);
		ASSERT_EQUAL(0, vecTDItemDelete.size());
		ASSERT_EQUAL(1, vecTDItemDecrease.size());

		TD_ITEM_DECREASE tdItemDecrease = vecTDItemDecrease.front();		
		CHECK_EQUAL(nSlotType, tdItemDecrease.m_nSlotType);
		CHECK_EQUAL(nSlotID, tdItemDecrease.m_nSlotID);
		CHECK_EQUAL(nDecreaseAmount, tdItemDecrease.m_nAmount);
	}
}
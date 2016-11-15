#include "stdafx.h"
#include "GItemHolder.h"
#include "GInventory.h"
#include "GEquipmentSlot.h"
#include "GUnitTestUtil.h"
#include "GUTHelper.h"
#include "GTestForward.h"
#include "FBaseGame.h"

SUITE(ItemHolderTest)
{	
	struct ItemHolderTest : public FBaseGame
	{			
		GItemHolder* m_pItemHolder;
		GItemData* m_pItemData;
		GUTHelper m_helper;

		ItemHolderTest()
		{
			m_pItemHolder = m_pPlayer->GetItemHolder();
		}

		~ItemHolderTest()	{}

		GItem* PushInvenItem(int id, int slotID, int amount)
		{
			GItem* pItem = m_helper.MakeItem(id, amount);
			m_pItemHolder->PushItem(SLOTTYPE_INVENTORY, slotID, pItem);

			return pItem;
		}
	};

	TEST_FIXTURE(ItemHolderTest, CollectItemByQuantity)
	{
		// arrange
		int nItemID = GUnitTestUtil::NewID();
		m_pItemData = m_helper.NewItemData(nItemID);
		m_pItemData->m_nStackAmount	= 100;

		GItem* pItem1 = PushInvenItem(nItemID, 0, 10);
		GItem* pItem2 = PushInvenItem(nItemID, 1, 5);

		int nStackAmt = 11;
		GITEM_STACK_AMT_VEC vec;

		// act
		int nRemainAmt = m_pItemHolder->CollectItemUntilAmount(SLOTTYPE_INVENTORY, nItemID, nStackAmt, vec);

		if (nRemainAmt != 0)
		{
			nRemainAmt = m_pItemHolder->CollectItemUntilAmount(SLOTTYPE_INVENTORY, nItemID, nRemainAmt, vec);
		}

		CHECK_EQUAL(0, nRemainAmt);

		// assert
		CHECK_EQUAL(2, vec.size());

		CHECK_EQUAL(pItem1->m_nIUID, vec[0].nIUID);
		CHECK_EQUAL(SLOTTYPE_INVENTORY, vec[0].nSlotType);
		CHECK_EQUAL(pItem1->m_nSlotID, vec[0].nSlotID);
		CHECK_EQUAL(10, vec[0].nModStackAmt);
		CHECK_EQUAL(0, vec[0].nToStackAmt);

		CHECK_EQUAL(pItem2->m_nIUID, vec[1].nIUID);
		CHECK_EQUAL(SLOTTYPE_INVENTORY, vec[1].nSlotType);
		CHECK_EQUAL(pItem2->m_nSlotID, vec[1].nSlotID);
		CHECK_EQUAL(1, vec[1].nModStackAmt);
		CHECK_EQUAL(4, vec[1].nToStackAmt);
	}
	
	TEST_FIXTURE(ItemHolderTest, IsUnarmed)
	{
		CHECK_EQUAL(true, m_pItemHolder->GetEquipment().IsUnarmed());

		test.item.EquipNewWeapon(m_pPlayer, WEAPON_1H_SLASH);
		CHECK_EQUAL(false, m_pItemHolder->GetEquipment().IsUnarmed());
	}
}

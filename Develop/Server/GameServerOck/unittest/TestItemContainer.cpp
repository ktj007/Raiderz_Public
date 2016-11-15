#include "stdafx.h"
#include "GInventory.h"
#include "TestItemManager.h"
#include "GUTHelper.h"
#include "GUnitTestUtil.h"

SUITE(InventoryTest)
{
	struct Fixture
	{
		GUTHelper m_helper;
		GInventory m_inventory;
		GEntityPlayer m_dummyPlayer;
		GItem* m_pItem;
		GItem* m_pItem2;
		map<MUID, GItem*> mapTestItem;

		Fixture(): m_inventory(&m_dummyPlayer)
		{
			m_pItem = MakeTestItem();
			m_pItem2 = MakeTestItem();
		}

		~Fixture()
		{
			DeleteTestItem();
		}

		GItem* MakeTestItem(void)
		{
			GItem* pItem = m_helper.MakeItem();
			mapTestItem.insert(make_pair(pItem->GetUID(), pItem));
			return pItem;
		}

		void DeleteTestItem(void)
		{
			vector<GItem*> vecAllItem;
			m_inventory.CollectAllItem(vecAllItem);

			for each(GItem* pItem in vecAllItem)
			{
				mapTestItem.erase(pItem->GetUID());
			}

			for each(pair<MUID, GItem*> testItem in mapTestItem)
			{
				delete testItem.second;
			}
		}
	};

	TEST_FIXTURE(Fixture, TestPushItem)
	{
		// arrange
		int slotID = 0;
		
		// act
		CHECK(m_inventory.PushItem(slotID, m_pItem));

		// assert
		CHECK_EQUAL(m_inventory.GetItemCount(), 1);
		CHECK_EQUAL(m_inventory.PopItem(slotID), m_pItem);
	}

	TEST_FIXTURE(Fixture, TestPopItem)
	{
		// arrange
		int slotID = 0;
		CHECK(m_inventory.PushItem(slotID, m_pItem));
		
		// act
		GItem* pPopItem = m_inventory.PopItem(slotID);

		// assert
		CHECK_EQUAL(m_inventory.GetItemCount(), 0);
		CHECK_EQUAL(pPopItem, m_pItem);
	}

	TEST_FIXTURE(Fixture, TestFindEmptySlot)
	{
		// arrange
		int firstSlotID = 0;
		CHECK(m_inventory.PushItem(firstSlotID, m_pItem));

		int secondSlotID = 2;
		CHECK(m_inventory.PushItem(secondSlotID, m_pItem2));
		
		// act
		int nFirstEmptySlotID = m_inventory.ReserveEmptySlot(0);
		int nSecondEmptySlotID = m_inventory.ReserveEmptySlot(nFirstEmptySlotID + 1);

		// assert
		CHECK_EQUAL(nFirstEmptySlotID, firstSlotID + 1);
		CHECK_EQUAL(nSecondEmptySlotID, secondSlotID + 1);
	}
}

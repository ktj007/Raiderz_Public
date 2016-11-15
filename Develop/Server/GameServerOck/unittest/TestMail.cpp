#include "stdafx.h"
#include "GMail.h"
#include "GTestWrapper.h"
#include "GUTHelper_Item.h"
#include "GItemManager.h"
#include "SUnitTestUtil.h"


SUITE(Mail)
{
	const int64 TEST_MAIL_UID = 555;

	struct Fixture
	{
		Fixture()
			: m_Mail(TEST_MAIL_UID)
		{
		}
		~Fixture()
		{
		}

		GItem* NewAppendedItem(int nSlotID)
		{
			GItem* pItem = m_helperItem.NewItem();
			pItem->m_pItemData->m_nID = SUnitTestUtil::NewID();
			pItem->m_nSlotID = nSlotID;
			pItem->m_nSlotType = SLOTTYPE_MAIL;
			return pItem;
		}

		GMail	m_Mail;
		GUTHelper_Item m_helperItem;

		GTestMgrWrapper<GItemManager>	m_itemManager;
	};

	void CHECK_AllAppendedItemSlotIsNULL(GMail& refMail)
	{
		CHECK((GItem*)NULL == refMail.GetAppendedItem(0));
		CHECK((GItem*)NULL == refMail.GetAppendedItem(1));
		CHECK((GItem*)NULL == refMail.GetAppendedItem(2));
		CHECK((GItem*)NULL == refMail.GetAppendedItem(3));
		CHECK((GItem*)NULL == refMail.GetAppendedItem(4));
	}

	TEST_FIXTURE(Fixture, TestMail_AddAppendedItemForDBTask)
	{
		/// arrange
		GItem* pItem = NewAppendedItem(0);

		/// act
		m_Mail.AddAppendedItemForDBTask(pItem);

		/// assert
		CHECK(pItem == m_Mail.GetAppendedItem(0));
		CHECK((GItem*)NULL == m_Mail.GetAppendedItem(1));
		CHECK((GItem*)NULL == m_Mail.GetAppendedItem(2));
		CHECK((GItem*)NULL == m_Mail.GetAppendedItem(3));
		CHECK((GItem*)NULL == m_Mail.GetAppendedItem(4));
	}

	TEST_FIXTURE(Fixture, TestMail_AddAppendedItemForDBTask_AllSlot)
	{
		/// arrange
		GItem* pItem1 = NewAppendedItem(0);
		GItem* pItem2 = NewAppendedItem(1);
		GItem* pItem3 = NewAppendedItem(2);
		GItem* pItem4 = NewAppendedItem(3);
		GItem* pItem5 = NewAppendedItem(4);

		/// act
		m_Mail.AddAppendedItemForDBTask(pItem1);
		m_Mail.AddAppendedItemForDBTask(pItem2);
		m_Mail.AddAppendedItemForDBTask(pItem3);
		m_Mail.AddAppendedItemForDBTask(pItem4);
		m_Mail.AddAppendedItemForDBTask(pItem5);

		/// assert
		CHECK(pItem1 == m_Mail.GetAppendedItem(0));
		CHECK(pItem2 == m_Mail.GetAppendedItem(1));
		CHECK(pItem3 == m_Mail.GetAppendedItem(2));
		CHECK(pItem4 == m_Mail.GetAppendedItem(3));
		CHECK(pItem5 == m_Mail.GetAppendedItem(4));
	}

	TEST_FIXTURE(Fixture, TestMail_AddAppendedItemForDBTask_Failure_OverTopBoundary)
	{
		/// arrange
		GItem* pItem = NewAppendedItem(MAX_MAIL_APPENDED_ITEM_COUNT);

		/// act
		bool ret = m_Mail.AddAppendedItemForDBTask(pItem);

		/// assert
		CHECK_EQUAL(false, ret);
		CHECK_AllAppendedItemSlotIsNULL(m_Mail);
	}

	TEST_FIXTURE(Fixture, TestMail_AddAppendedItemForDBTask_Failure_BelowBottomBoundary)
	{
		/// arrange
		GItem* pItem = NewAppendedItem(-1);

		/// act
		bool ret = m_Mail.AddAppendedItemForDBTask(pItem);

		/// assert
		CHECK_EQUAL(false, ret);
		CHECK_AllAppendedItemSlotIsNULL(m_Mail);
	}

	TEST_FIXTURE(Fixture, TestMail_ClearAppendedItems)
	{
		/// arrange
		GItem* pItem = NewAppendedItem(0);

		m_Mail.AddAppendedItemForDBTask(pItem);

		/// act
		m_Mail.ClearAppendedItems();

		/// assert
		CHECK_AllAppendedItemSlotIsNULL(m_Mail);
	}

	TEST_FIXTURE(Fixture, TestMail_ClearAppendedItems_AllSlot)
	{
		/// arrange
		GItem* pItem1 = NewAppendedItem(0);
		GItem* pItem2 = NewAppendedItem(1);
		GItem* pItem3 = NewAppendedItem(2);
		GItem* pItem4 = NewAppendedItem(3);
		GItem* pItem5 = NewAppendedItem(4);

		m_Mail.AddAppendedItemForDBTask(pItem1);
		m_Mail.AddAppendedItemForDBTask(pItem2);
		m_Mail.AddAppendedItemForDBTask(pItem3);
		m_Mail.AddAppendedItemForDBTask(pItem4);
		m_Mail.AddAppendedItemForDBTask(pItem5);

		/// act
		m_Mail.ClearAppendedItems();

		/// assert
		CHECK_AllAppendedItemSlotIsNULL(m_Mail);
	}

	TEST_FIXTURE(Fixture, TestMail_GetNewThumbnailItemID_Return_Zero)
	{
		// arrange
		GItem* pItem1 = NewAppendedItem(0);
		m_Mail.AddAppendedItemForDBTask(pItem1);

		int nNewThumbnailItemID;

		// act
		nNewThumbnailItemID = m_Mail.GetNewThumbnailItemID(pItem1->m_nSlotID);

		// assert
		CHECK_EQUAL(0, nNewThumbnailItemID);
	}

	TEST_FIXTURE(Fixture, TestMail_GetNewThumbnailItemID_Return_SecondSlot_ItemID)
	{
		// arrange
		GItem* pItem1 = NewAppendedItem(0);
		GItem* pItem2 = NewAppendedItem(1);

		m_Mail.AddAppendedItemForDBTask(pItem1);
		m_Mail.AddAppendedItemForDBTask(pItem2);

		int nNewThumbnailItemID;

		// act
		nNewThumbnailItemID = m_Mail.GetNewThumbnailItemID(pItem1->m_nSlotID);

		// assert
		CHECK_EQUAL(pItem2->m_pItemData->m_nID, nNewThumbnailItemID);
	}

}
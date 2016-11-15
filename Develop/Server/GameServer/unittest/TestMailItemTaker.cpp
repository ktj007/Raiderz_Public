#include "stdafx.h"
#include "FForCommandTest.h"
#include "GMailSystem.h"
#include "GMailItemTaker.h"
#include "GDBManager.h"
#include "GEntityPlayer.h"
#include "GUTHelper_Player.h"
#include "GUTHelper_Item.h"
#include "GItem.h"
#include "GMail.h"
#include "GPlayerMailBox.h"
#include "GTestForward.h"
#include "GCLientMailRouter.h"
#include "GItemHolder.h"


SUITE(MailItemTaker)
{
	struct MockMailDBManager: public GDBManager
	{
		MOCK_METHOD1(MailGetAppendedItem, bool(const GDBT_MAIL_GET_ITEM& data));
	};
	
	struct MockClientMailRouter : public GClientMailRouter
	{
		MOCK_METHOD4(TakeAppendedItemRes, void(GEntityPlayer* pPlayer, MUID uidMail, uint8 nSlotID, int nNewThumbnailItemID));
	};
	
	struct Fixture: public FForCommandTest
	{
		GTestSysWrapper2<GDBManager, MockMailDBManager> m_DBManagerWrapper;

		MockMailDBManager* m_pDBManager;
		MockClientMailRouter* m_pClientMailRouter;

		GMailItemTaker	m_mailItemTaker;

		GMail* m_pMail;
		GItem* m_pAppendedItem;

		Fixture()
		{
			m_pDBManager = m_DBManagerWrapper.Get();

			m_pClientMailRouter = new MockClientMailRouter();
			m_mailItemTaker.SetClientRouter(m_pClientMailRouter);

			InitMail();
		}
		~Fixture()
		{
			SAFE_DELETE(m_pClientMailRouter);
		}

		void InitMail()
		{
			MUID uidMail = SUnitTestUtil::NewUID();
			GMail* pNewMail = new GMail(uidMail.Value);

			m_pPlayer->GetMailbox().AddMail(pNewMail);

			InitAppendedItem(pNewMail, 0);

			m_pMail = pNewMail;
		}
		void InitAppendedItem(GMail* pMail, int nSlotID)
		{
			GItem* pItem = GUTHelper_Item::NewItem();
			pItem->m_nSlotID = nSlotID;
			pItem->m_nSlotType = SLOTTYPE_MAIL;
			
			pMail->AddAppendedItemForDBTask(pItem);

			m_pAppendedItem = pItem;
		}
	};

	TEST_FIXTURE(Fixture, TestMailItemTaker_Fixture)
	{
		CHECK_EQUAL(1, m_pPlayer->GetMailbox().GetSize());
		CHECK(m_pMail->GetAppendedItem(0) == m_pAppendedItem);
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_Take_Success)
	{
		// arrange
		GDBT_MAIL_GET_ITEM data;
		data.m_uidPlayer			= m_pPlayer->GetUID();
		data.m_nAID					= m_pPlayer->GetAID();
		data.m_nCID					= m_pPlayer->GetCID();
		data.m_nMailUID				= m_pMail->GetUID();
		data.m_nMailSlotID			= m_pAppendedItem->m_nSlotID;
		data.m_nIUID				= m_pAppendedItem->m_nIUID;
		data.m_nItemID				= m_pAppendedItem->m_pItemData->m_nID;
		data.m_InvenSlotID			= 0;	///< First SlotID
		data.m_nNewMailDefaultItemID= 0;
		data.m_nCharPlayTime		= m_pPlayer->GetPlayerInfo()->nPlayTimeSec;
		data.m_nDeltaCharPtm		= m_pPlayer->GetPlayerInfo()->GetDeltaPlayTime();
		data.m_nLevel				= m_pPlayer->GetLevel();
		data.m_nModStackAmt			= m_pAppendedItem->GetAmount();
		data.m_nMoney				= m_pPlayer->GetMoney();
		
		// arrange (=assert)
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedItem(data))
					.Times(1);
		
		// act
		m_mailItemTaker.Take(m_pPlayer, m_pMail->GetUID(), m_pAppendedItem->m_nSlotID);
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_Take_Failure_InvalidSlotID)
	{
		// arrange
		int nInvalidSlotID = 1;

		// arrange (=assert)
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedItem(_))
			.Times(0);

		// act
		bool bRet = m_mailItemTaker.Take(m_pPlayer, m_pMail->GetUID(), nInvalidSlotID);

		// assert
		CHECK_EQUAL(false, bRet);
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_Take_Failure_InventoryIsFull)
	{
		// arrange
		test.item.FullItemToInven(m_pPlayer, 0);

		// arrange (=assert)
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedItem(_))
			.Times(0);

		// act
		bool bRet = m_mailItemTaker.Take(m_pPlayer, m_pMail->GetUID(), m_pAppendedItem->m_nSlotID);

		// assert
		CHECK_EQUAL(false, bRet);

		ASSERT_EQUAL(MC_MSG_RES, m_pLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_SYSTEM_FEW_INVENTORY, m_pLink->GetParam<int>(0,4));
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_Take_Failure_InventoryIsFull_AbleToMerge)
	{
		// arrange
		test.item.FullItemToInven(m_pPlayer, 1);
		m_pAppendedItem->m_pItemData->m_nStackAmount = 10;	///< 10개 까지 겹쳐질 수 있게 설정.
		m_pPlayer->GetItemHolder()->SetAmount(m_pAppendedItem->m_nSlotType, m_pAppendedItem->m_nSlotID, 1);		
		test.item.GiveNewItem(m_pPlayer, m_pAppendedItem->m_pItemData->m_nID);	///< 하나 가지고 있게 설정.

		// arrange (=assert)
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedItem(_))
			.Times(0);

		// act
		bool bRet = m_mailItemTaker.Take(m_pPlayer, m_pMail->GetUID(), m_pAppendedItem->m_nSlotID);

		// assert
		CHECK_EQUAL(false, bRet);

		ASSERT_EQUAL(MC_MSG_RES, m_pLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_SYSTEM_FEW_INVENTORY, m_pLink->GetParam<int>(0,4));
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_Take_Failure_UniqueItem)
	{
		// arrange
		m_pAppendedItem->m_pItemData->m_bUnique = true;	///< 유니크 아이템으로 설정.
		test.item.GiveNewItem(m_pPlayer, m_pAppendedItem->m_pItemData->m_nID);	///< 하나 가지고 있게 설정.

		// arrange (=assert)
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedItem(_))
			.Times(0);

		// act
		bool bRet = m_mailItemTaker.Take(m_pPlayer, m_pMail->GetUID(), m_pAppendedItem->m_nSlotID);

		// assert
		CHECK_EQUAL(false, bRet);

		ASSERT_EQUAL(MC_MSG_RES, m_pLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_SYSTEM_CANT_HAVE_TWO_UNIQUE_ITEM, m_pLink->GetParam<int>(0,4));
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_Take_Failure_OverMaxStackAmount)
	{
		// arrange
		m_pAppendedItem->m_pItemData->m_nStackAmount = 10;	///< 최대 스텍킹 개수를 10개로 설정.		
		m_pAppendedItem->m_nAmount = 20;	///< 수량을 최대개수 넘치게 설정.

		// arrange (=assert)
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetAppendedItem(_))
			.Times(0);

		// act
		bool bRet = m_mailItemTaker.Take(m_pPlayer, m_pMail->GetUID(), m_pAppendedItem->m_nSlotID);

		// assert
		CHECK_EQUAL(false, bRet);

		ASSERT_EQUAL(MC_MSG_RES, m_pLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_ITEM_NOT_ADD_OVER_MAXSTACK, m_pLink->GetParam<int>(0,4));
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_DelTakenItemForDBTask_Success)
	{
		// arrange
		int nThumbnailItemID = SUnitTestUtil::NewID();

		// arrange (=assert)
		EXPECT_CALL(*m_pClientMailRouter, TakeAppendedItemRes(m_pPlayer, (MUID)m_pMail->GetUID(), (uint8)m_pAppendedItem->m_nSlotID, nThumbnailItemID));	//pPlayer, nMailUID, nMailSlotID

		//// act
		m_mailItemTaker.DelTakenItemForDBTask(m_pPlayer, m_pMail->GetUID(), m_pAppendedItem->m_nSlotID, nThumbnailItemID);
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_DelTakenItemForDBTask_Failure_InvalidMailUID)
	{
		// arrange
		int nThumbnailItemID = SUnitTestUtil::NewID();
		int64 nInvalidMailUID = -1;

		// arrange (=assert)
		EXPECT_CALL(*m_pClientMailRouter, TakeAppendedItemRes(m_pPlayer, (MUID)m_pMail->GetUID(), (uint8)m_pAppendedItem->m_nSlotID, nThumbnailItemID))
			.Times(0);	//pPlayer, nMailUID, nMailSlotID

		//// act
		m_mailItemTaker.DelTakenItemForDBTask(m_pPlayer, nInvalidMailUID, m_pAppendedItem->m_nSlotID, nThumbnailItemID);
	}

	TEST_FIXTURE(Fixture, TestMailItemTaker_DelTakenItemForDBTask_Failure_InvalidMailSlotID)
	{
		// arrange
		int nThumbnailItemID = SUnitTestUtil::NewID();
		int nInvalidMailItemSlotID = -1;

		// arrange (=assert)
		EXPECT_CALL(*m_pClientMailRouter, TakeAppendedItemRes(m_pPlayer, (MUID)m_pMail->GetUID(), (uint8)m_pAppendedItem->m_nSlotID, nThumbnailItemID))
			.Times(0);	//pPlayer, nMailUID, nMailSlotID

		//// act
		m_mailItemTaker.DelTakenItemForDBTask(m_pPlayer, m_pMail->GetUID(), nInvalidMailItemSlotID, nThumbnailItemID);
	}

}

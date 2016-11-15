#include "stdafx.h"
#include "FForCommandTest.h"
#include "GMailSystem.h"
#include "GMailItemTaker.h"
#include "GMailMoneyTaker.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "CCommandResultTable.h"
#include "GClientMailRouter.h"
#include "GUnitTestUtil.h"
#include "GAppServerMailRouter.h"
#include "FBaseMail.h"

SUITE(MailCommand)
{
	struct MockMailItemTaker : public GMailItemTaker
	{
		MOCK_METHOD3(Take, bool(GEntityPlayer* pPlayer, MUID uidMail, int nSlotID));
	};

	struct MockMailMoneyTaker : public GMailMoneyTaker
	{
		MOCK_METHOD2(TakeAppendedMoney, bool(GEntityPlayer* pPlayer, MUID uidMail));
	};

	struct MockMailSystem: public GMailSystem
	{
		MOCK_METHOD3(PostMail, bool(GEntityPlayer* pPlayer, const TD_MAIL_POST_REQ_INFO* pInfo, wstring strMailText));
		MOCK_METHOD2(UpdateMailBox, void(GEntityPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo));
		MOCK_METHOD2(RetryTransNewMail, void(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo));
		MOCK_METHOD2(ReadMail, void(GEntityPlayer* pPlayer, MUID uidMail));
		MOCK_METHOD2(PutUpItem, void(GEntityPlayer* pPlaeyr, const TD_MAIL_RESERVE_ITEM* pItem));
		MOCK_METHOD2(PutDownItem, void(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem));
	};
	
	struct TestMailCommand: public FForCommandTest, public FBaseMail
	{
		GTestSysWrapper2<GMailSystem, MockMailSystem> m_MailSystemWrapper;
		MockMailSystem* m_pMailSystem;		
		GPlayerObject* m_pPlayerObject;

		MockMailItemTaker*	m_pMailItemTaker;
		MockMailMoneyTaker*	m_pMailMoneyTaker;

		TestMailCommand()
		{
			m_pMailSystem = m_MailSystemWrapper.Get();

			m_pMailItemTaker = new MockMailItemTaker();
			m_pMailSystem->SetMailItemTaker(m_pMailItemTaker);

			m_pMailMoneyTaker = new MockMailMoneyTaker();
			m_pMailSystem->SetMailMoneyTaker(m_pMailMoneyTaker);

			// 기본 InWorld 상태로 시작
			MUID uidPlayer = m_pPlayer->GetUID();
			m_pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
			m_pPlayerObject->OnStartGame();

			m_pPlayerObject->GetEntity()->DisableICheck();
		}

		MCommandResult RecvPostMailReq(TD_MAIL_POST_REQ_INFO& info, wstring strMailText)
		{
			return m_pLink->OnRecv(MC_MAIL_POST_REQ, 2, NEW_SINGLE_BLOB(info), NEW_WSTR(strMailText.c_str()));
		}

		MCommandResult RecvTransNewMailReq(CID nCID, TD_MAIL_MAILBOX_MAIL_INFO& info)
		{
			return m_pAppNetAgent->OnRecv(MPC_MAIL_TRANS_NEW_MAIL, 2, NEW_INT(nCID), NEW_SINGLE_BLOB(info));
		}

		MCommandResult RecvTakeAppendedItem(MUID uidMail, uint8 nSlotID)
		{
			return m_pLink->OnRecv(MC_MAIL_TAKE_APPENDED_ITEM_REQ, 2, NEW_UID(uidMail), NEW_UCHAR(nSlotID));
		}

		MCommandResult RecvTakeAppendedMoney(MUID uidMail)
		{
			return m_pLink->OnRecv(MC_MAIL_TAKE_APPENDED_MONEY_REQ, 1, NEW_UID(uidMail));
		}

		MCommandResult RecvAppendPutUpItemReq(TD_MAIL_RESERVE_ITEM& item)
		{
			return m_pLink->OnRecv(MC_MAIL_APPEND_PUTUP_ITEM_REQ, 1, NEW_SINGLE_BLOB(item));
		}

		MCommandResult RecvAppendPutDownItemReq(TD_MAIL_RESERVE_ITEM& item)
		{
			return m_pLink->OnRecv(MC_MAIL_APPEND_PUTDOWN_ITEM_REQ, 1, NEW_SINGLE_BLOB(item));
		}
	};

	TEST_FIXTURE(TestMailCommand, CommandRouter_MailboxIsOpened)
	{
		// arrange
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);

		vector<TD_MAIL_MAILBOX_MAIL_INFO> vecTD_MAIL_MAILBOX_MAIL_INFO;
		vecTD_MAIL_MAILBOX_MAIL_INFO.push_back(info);

		// act
		GClientMailRouter router;
		router.MailboxIsOpened(m_pPlayer, vecTD_MAIL_MAILBOX_MAIL_INFO);

		// assert
		CHECK_EQUAL(MC_MAIL_MAILBOX_IS_OPENED, m_pLink->GetCommandID(0));
		MCommandParameter* pParam = m_pLink->GetCommand(0).GetParameter(0);
		ASSERT_CHECK(pParam != NULL);
		CHECK(info == *(TD_MAIL_MAILBOX_MAIL_INFO*)pParam->GetPointer());
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_MailboxIsClosed)
	{
		// act
		GClientMailRouter router;
		router.MailboxIsClosed(m_pPlayer);

		// assert
		CHECK_EQUAL(MC_MAIL_MAILBOX_IS_CLOSED, m_pLink->GetCommandID(0));
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_PostMailReq)
	{	
		// arrange
		TD_MAIL_POST_REQ_INFO info;
		wcsncpy_s(info.strReceiverName, L"TestReceiverName", _TRUNCATE);
		wcsncpy_s(info.strMailTitle, L"TestMailTitle", _TRUNCATE);		
		info.nAppendedMoney = 100;

		wstring strMailText = L"TestMailText";
		

		// arrange mock(=assert)
		using testing::Pointee;			// 포인터가 가르키는 값비교
		EXPECT_CALL(*m_pMailSystem, PostMail(m_pPlayer, Pointee(info), strMailText));
		

		// act		
		MCommandResult nCommandResult = RecvPostMailReq(info, strMailText);
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_PostMailRes)
	{
		// arrange
		CCommandResultTable nResult = CR_SUCCESS;

		// act
		GClientMailRouter router;
		router.MailPostRes(m_pPlayer, nResult);

		// assert
		CHECK_EQUAL(MC_MAIL_POST_RES, m_pLink->GetCommandID(0));
		CHECK_EQUAL(nResult, m_pLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_RouteNewMailToReceiver)
	{
		// arrange
		CID nCID = GUnitTestUtil::NewCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// act
		GAppServerMailRouter router;
		router.RouteNewMailToReceiver(nCID, &info);


		// assert
		CHECK_EQUAL(MPC_MAIL_ROUTE_NEW_MAIL_TO_RECEIVER, m_pAppNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(nCID, m_pAppNetAgent->GetSendParam<int>(0, 0));		

		MCommandParameter* pParam = m_pAppNetAgent->GetSendCommand(0).GetParameter(1);
		ASSERT_CHECK(pParam != NULL);
		CHECK(info == *(TD_MAIL_MAILBOX_MAIL_INFO*)pParam->GetPointer());
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_TransNewMailReq_Success)
	{
		// arrange
		CID nCID = m_pPlayer->GetCID();
		
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)
		using testing::Pointee;			// 포인터가 가르키는 값비교
		EXPECT_CALL(*m_pMailSystem, UpdateMailBox(m_pPlayer, Pointee(info)));


		// act
		MCommandResult nCommandResult = RecvTransNewMailReq(nCID, info);
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_TransNewMailReq_UnkownPlayer)
	{
		// arrange
		CID nUnkownCID = GUnitTestUtil::NewCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		EXPECT_CALL(*m_pMailSystem, RetryTransNewMail(nUnkownCID, Pointee(info)))
			.Times(1);				// 재시도 요청

		using testing::_;
		EXPECT_CALL(*m_pMailSystem, UpdateMailBox(_, _))
			.Times(0);				// 통보하지 않는다.


		// act
		MCommandResult nCommandResult = RecvTransNewMailReq(nUnkownCID, info);
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_TransNewMailReq_NotInWorld)
	{
		// arrange
		CID nCID = m_pPlayer->GetCID();
		m_pPlayerObject->OnLogout();		// InWorld 외 다른 상태
		
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		EXPECT_CALL(*m_pMailSystem, RetryTransNewMail(nCID, Pointee(info)))
			.Times(1);				// 재시도 요청

		using testing::_;
		EXPECT_CALL(*m_pMailSystem, UpdateMailBox(_, _))
			.Times(0);				// 통보하지 않는다.


		// act
		MCommandResult nCommandResult = RecvTransNewMailReq(nCID, info);
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}
	
	TEST_FIXTURE(TestMailCommand, CommandRouter_TransNewMailRes)
	{
		// arrange
		CID nCID = GUnitTestUtil::NewCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// act
		GAppServerMailRouter router;
		router.RetryTransNewMail(nCID, &info);

		
		// assert
		CHECK_EQUAL(MPC_MAIL_RETRY_TRANS_NEW_MAIL, m_pAppNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(nCID, m_pAppNetAgent->GetSendParam<int>(0, 0));

		MCommandParameter* pParam = m_pAppNetAgent->GetSendCommand(0).GetParameter(1);
		ASSERT_CHECK(pParam != NULL);
		CHECK(info == *(TD_MAIL_MAILBOX_MAIL_INFO*)pParam->GetPointer());
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_NotifyMail)
	{
		// arrange
		bool bIsNew = true;

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);
		vector<TD_MAIL_MAILBOX_MAIL_INFO> vecInfo;
		vecInfo.push_back(info);

		
		// act
		GClientMailRouter router;
		router.NotifyMail(m_pPlayer, bIsNew, vecInfo);

	
		// assert
		ASSERT_EQUAL(MC_MAIL_NOTIFY_MAIL, m_pLink->GetCommandID(0));

		CHECK_EQUAL(bIsNew, m_pLink->GetParam<bool>(0, 0));

		MCommandParameter* pParam = m_pLink->GetCommand(0).GetParameter(1);
		ASSERT_CHECK(pParam != NULL);
		CHECK(info == *(TD_MAIL_MAILBOX_MAIL_INFO*)pParam->GetPointer());
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_MailRead)
	{
		// arrange
		MUID uidMail = GUnitTestUtil::NewUID();


		// arrange mock(=assert)
		EXPECT_CALL(*m_pMailSystem, ReadMail(m_pPlayer, uidMail));


		// act
		m_pLink->OnRecv(MC_MAIL_READ_REQ, 1, NEW_UID(uidMail));
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_MailRead)
	{
		// arrange
		TD_MAIL_CONTENT_INFO info;
		info.uidMail = GUnitTestUtil::NewUID();
		info.nAppendedMoney = 100;

		wstring strMailText = L"TestMailText";

		vector<TD_ITEM> vecAppendedItem;
		TD_ITEM item;
		vecAppendedItem.push_back(item);

		
		// act
		GClientMailRouter router;
		router.MailReadRes(m_pPlayer, &info, strMailText, vecAppendedItem);


		// assert
		ASSERT_EQUAL(MC_MAIL_READ_RES, m_pLink->GetCommandID(0));

		MCommandParameter* pParam = m_pLink->GetCommand(0).GetParameter(0);
		ASSERT_CHECK(pParam != NULL);
		CHECK(info == *(TD_MAIL_CONTENT_INFO*)pParam->GetPointer());

		CHECK(strMailText == m_pLink->GetParam<const wchar_t*>(0, 1));

		vector<TD_ITEM> vecSendData;
		ASSERT_CHECK(m_pLink->GetCommand(0).GetBlob(vecSendData, 2));
		CHECK(vecAppendedItem == vecSendData);
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_TakeAppendedItemReq)
	{
		// arrange
		MUID uidMail	= GUnitTestUtil::NewUID();
		int nSlotID		= 1;

		// arrange mock(=assert)
		EXPECT_CALL(*m_pMailItemTaker, Take(m_pPlayer, uidMail, nSlotID));

		// act
		MCommandResult nCommandResult = RecvTakeAppendedItem(uidMail, nSlotID);

		// assert
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}
	TEST_FIXTURE(TestMailCommand, CommandHandler_TakeAppendedItemRes)
	{
		// arrange
		MUID uidMail			= GUnitTestUtil::NewUID();
		int nSlotID				= 0;
		int nNewThumbnailItemID = GUnitTestUtil::NewID();

		// act
		GClientMailRouter router;
		router.TakeAppendedItemRes(m_pPlayer, uidMail, nSlotID, nNewThumbnailItemID);

		// assert
		ASSERT_EQUAL(MC_MAIL_TAKE_APPENDED_ITEM_RES, m_pLink->GetCommandID(0));
		CHECK_EQUAL(uidMail,			 m_pLink->GetParam<MUID>(0,0));
		CHECK_EQUAL(nSlotID,			 m_pLink->GetParam<uint8>(0,1));
		CHECK_EQUAL(nNewThumbnailItemID, m_pLink->GetParam<int>(0,2));
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_TakeAppendedMoneyReq)
	{
		// arrange
		MUID uidMail	= GUnitTestUtil::NewUID();

		// arrange mock(=assert)
		EXPECT_CALL(*m_pMailMoneyTaker, TakeAppendedMoney(m_pPlayer, uidMail));

		// act
		MCommandResult nCommandResult = RecvTakeAppendedMoney(uidMail);

		// assert
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}
	TEST_FIXTURE(TestMailCommand, CommandHandler_TakeAppendedMoneyRes)
	{
		// arrange
		MUID uidMail	= GUnitTestUtil::NewUID();

		// act
		GClientMailRouter router;
		router.TakeAppendedMoneyRes(m_pPlayer, uidMail);

		// assert
		ASSERT_EQUAL(MC_MAIL_TAKE_APPENDED_MONEY_RES, m_pLink->GetCommandID(0));
		CHECK_EQUAL(uidMail, m_pLink->GetParam<MUID>(0, 0));
	}

	TEST_FIXTURE(TestMailCommand, CommandHander_PutUpItemReq)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// arrange mock(=assert)
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		EXPECT_CALL(*m_pMailSystem, PutUpItem(m_pPlayer, Pointee(item)));
		

		// act
		MCommandResult nCommandResult = RecvAppendPutUpItemReq(item);
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_PutDownItemReq)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// arrange mock(=assert)
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		EXPECT_CALL(*m_pMailSystem, PutDownItem(m_pPlayer, Pointee(item)));


		// act
		MCommandResult nCommandResult = RecvAppendPutDownItemReq(item);
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_PutUpItemRes)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);		


		// act
		GClientMailRouter router;
		router.PutUpItemRes(m_pPlayer, &item);


		// assert
		ASSERT_EQUAL(MC_MAIL_APPEND_PUTUP_ITEM_RES, m_pLink->GetCommandID(0));

		MCommandParameter* pParam = m_pLink->GetCommand(0).GetParameter(0);
		ASSERT_CHECK(pParam != NULL);
		CHECK(item == *(TD_MAIL_RESERVE_ITEM*)pParam->GetPointer());
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_PutDownItemRes)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// act
		GClientMailRouter router;
		router.PutDownItemRes(m_pPlayer, &item);


		// assert
		ASSERT_EQUAL(MC_MAIL_APPEND_PUTDOWN_ITEM_RES, m_pLink->GetCommandID(0));

		MCommandParameter* pParam = m_pLink->GetCommand(0).GetParameter(0);
		ASSERT_CHECK(pParam != NULL);
		CHECK(item == *(TD_MAIL_RESERVE_ITEM*)pParam->GetPointer());
	}
}

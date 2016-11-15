#include "stdafx.h"
#include "GTestWrapper.h"
#include "GMailSystem.h"
#include "GDBTaskData.h"
#include "FBasePlayer.h"
#include "GUnitTestUtil.h"
#include "GEntityPlayer.h"
#include "GClientMailRouter.h"
#include "GUTHelper_Player.h"
#include "GAppServerMailRouter.h"
#include "GPlayerMailBox.h"
#include "GDBTaskMailWrite.h"
#include "GMail.h"
#include "FBaseMail.h"
#include "GMailItemPutUper.h"
#include "GMailItemPutDowner.h"
#include "GPlayerMailReserver.h"
#include "GUTHelper_Item.h"
#include "MockLink.h"
#include "GUTHelper_Network.h"
#include "CCommandTable.h"


SUITE(MailSystem)
{
	struct MockMailDBManager: public GDBManager
	{
		MOCK_METHOD2(MailGetList, bool(const GDBT_MAIL_GET_MAILBOX_LIST& refParam, bool bOpenMaibox));
		MOCK_METHOD2(MailCheckReceiver, bool(const GEntityPlayer* pPlayer, const GDBT_MAIL_WRITE& data));
		MOCK_METHOD3(MailGetContent, bool(const MUID& uidPlayer, int64 nMUID, bool bHasText));		
	};

	struct MockClientMailRouter: public GClientMailRouter
	{
		MOCK_METHOD2(MailboxIsOpened, void(GEntityPlayer* pPlayer, const vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecTD_MAIL_MAILBOX_MAIL_INFO));
		MOCK_METHOD2(MailPostRes, void(GEntityPlayer* pPlayer, CCommandResultTable nResult));
		MOCK_METHOD3(NotifyMail, void(GEntityPlayer* pPlayer, bool bIsNew, const vector<TD_MAIL_MAILBOX_MAIL_INFO>& vecTD_MAIL_MAILBOX_MAIL_INFO));
		MOCK_METHOD2(PutUpItemRes, void(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem));
		MOCK_METHOD2(PutDownItemRes, void(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem));
	};

	struct MockServerMailRouter: public GAppServerMailRouter
	{
		MOCK_METHOD2(RetryTransNewMail, void(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo));
	};

	struct MockMailSystem: public GMailSystem
	{
		MOCK_METHOD2(RouteNewMailToReceiver, void(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo));
		MOCK_METHOD2(RouteMailPostRes, void(GEntityPlayer* pPlayer, CCommandResultTable nResult));
	};

	struct MockMailItemPutUper: public GMailItemPutUper
	{
		MOCK_METHOD2(Do, bool(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem));
	};

	struct MockMailItemPutDowner: public GMailItemPutDowner
	{
		MOCK_METHOD2(Do, bool(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem));
	};


	const int64 TEST_MAIL_UID_TOP = 100000;
	const int64 TEST_MAIL_UID_BOTTOM = 1;

	struct TestMailSystem: public FBaseMail
	{		
		GTestSysWrapper2<GMailSystem, MockMailSystem> m_MailSystemWrapper;
		GTestSysWrapper2<GDBManager, MockMailDBManager> m_DBManagerWrapper;

		MockMailSystem* m_pMailSystem;
		MockMailDBManager* m_pDBManager;
		GEntityPlayer* m_pPlayer;
		MockLink* m_pLink;

		MockClientMailRouter* m_pClientMailRouter;
		MockServerMailRouter* m_pServerMailRouter;

		MockMailItemPutUper* m_pMailItemPutUper;
		MockMailItemPutDowner* m_pMailItemPutDowner;


		TestMailSystem()
		{
			m_pMailSystem = m_MailSystemWrapper.Get();
			m_pDBManager = m_DBManagerWrapper.Get();

			GUTHelper_Player helper;
			m_pPlayer = helper.NewPlayer();

			m_pPlayer->GetPlayerInfo()->SetMoney(1000);

			GUTHelper_Network networkHelper;
			m_pLink = networkHelper.NewLink(m_pPlayer);

			InitMailbox();

			m_pClientMailRouter = new MockClientMailRouter();
			m_pMailSystem->SetClientRouter(m_pClientMailRouter);

			m_pServerMailRouter = new MockServerMailRouter();
			m_pMailSystem->SetServerRouter(m_pServerMailRouter);

			m_pMailItemPutUper = new MockMailItemPutUper();
			m_pMailSystem->SetMailItemPutUper(m_pMailItemPutUper);

			m_pMailItemPutDowner = new MockMailItemPutDowner();
			m_pMailSystem->SetMailItemPutDowner(m_pMailItemPutDowner);
		}

		void InitMailbox()
		{
			m_pPlayer->GetMailbox().GetDBMailUID().Init(TEST_MAIL_UID_TOP, TEST_MAIL_UID_BOTTOM);
		}

		void InitData_PostMail(GEntityPlayer* pPlayer, TD_MAIL_POST_REQ_INFO& info, wstring& strMailText, GDBT_MAIL_WRITE& mail)
		{
			// TD_MAIL_POST_REQ_INFO&
			wcsncpy_s(info.strReceiverName, L"TestReceiverName", _TRUNCATE);
			wcsncpy_s(info.strMailTitle, L"TestMailTitle", _TRUNCATE);
			info.nAppendedMoney = 100;

			// wstirng&
			strMailText = L"TestContent";

			// GDBT_MAIL_WRITE&
			mail.m_nSenderAID = pPlayer->GetAID();
			mail.m_nSenderCID = pPlayer->GetCID();
			mail.m_strSenderName = pPlayer->GetName();
			mail.m_nSenderCharacterPlaySeconds = pPlayer->GetPlayerInfo()->nPlayTimeSec;
			mail.m_nDeltaCharPtm = pPlayer->GetPlayerInfo()->GetDeltaPlayTime();
			mail.m_nLevel = pPlayer->GetLevel();

			mail.m_strReceiverName = info.strReceiverName;

			mail.m_nMailType = MT_PLAYER;
			mail.m_strTitle = info.strMailTitle;	
			mail.m_hasText = !strMailText.empty();
			mail.m_strText = strMailText;
			mail.m_nRemainDeleteSeconds = MAX_MAIL_DELETE_SECOND;


			GPlayerMailReserver& reserver = pPlayer->GetMailReserver();

			GUTHelper_Item::GiveNewItem(pPlayer, 100, 5);
			GUTHelper_Item::GiveNewItem(pPlayer, 200, 5);
			GUTHelper_Item::GiveNewItem(pPlayer, 300, 5);
			GUTHelper_Item::GiveNewItem(pPlayer, 400, 5);
			GUTHelper_Item::GiveNewItem(pPlayer, 500, 5);

			reserver.PutUpItem(0, 1);
			reserver.PutUpItem(1, 2);
			reserver.PutUpItem(2, 3);
			reserver.PutUpItem(3, 4);
			reserver.PutUpItem(4, 5);

			mail.m_nDefaultItemID = reserver.GetDefaultItemID();
			mail.m_nAppendedItemCount = reserver.GetAppendedSlotCount();
			reserver.MakeDBT(mail.m_appendedItemSlot);


			pPlayer->GetPlayerInfo()->SetMoney(1000);			
			mail.m_nDeltaMoney = 100;			
			int nPostage = m_pMailSystem->CalcPostage(reserver.GetAppendedSlotCount());
			mail.m_nMoney = pPlayer->GetPlayerInfo()->GetMoney() - mail.m_nDeltaMoney - nPostage;
		}

		GMail* AddTestMail(GEntityPlayer* pPlayer, bool bHasText=false)
		{
			int64 nMailUID = 999;
			GMail* pNewMail = pPlayer->GetMailbox().NewMail(nMailUID);
			pNewMail->Init(pPlayer->GetCID(), MT_PLAYER, false, L"", L"", bHasText, 0, 0, 60);
			pPlayer->GetMailbox().AddMail(pNewMail);
			return pNewMail;
		}

		bool CheckRouteFailure(CCommandResultTable nResult)
		{
			MCommand& cmd = m_pLink->GetCommand(0);

			if (MC_MSG_RES != cmd.GetID())
			{
				return false;
			}

			if (5 != cmd.GetParameterCount())
			{
				return false;
			}

			int nSendResult;
			if (!cmd.GetParameter(&nSendResult, 4, MPT_INT))
			{
				return false;
			}

			if (nSendResult != nResult)
			{
				return false;
			}

			return true;
		}
	};

	TEST_FIXTURE(TestMailSystem, OpenMailbox_ExistMailAtDB_Should_Call_MailGetList)
	{
		// arrange mock(=assert)
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();
		refMailbox.GetDBMailUID().Init(10, 0);
		CHECK(0 < refMailbox.GetRemainSize());

		GDBT_MAIL_GET_MAILBOX_LIST dbParam(m_pPlayer->GetUID(),
			m_pPlayer->GetAID(),
			(int64)m_pPlayer->GetCID(),
			m_pPlayer->GetMailbox().GetDBMailUID().GetTop(),
			m_pPlayer->GetMailbox().GetDBMailUID().GetBottom(),
			m_pPlayer->GetMailbox().GetRemainSize());
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetList(dbParam, true))
			.Times(1);

		// act
		m_pMailSystem->OpenMailbox(m_pPlayer);
	}

	TEST_FIXTURE(TestMailSystem, OpenMailbox_NoMail_Should_Call_MailboxIsOpened)
	{
		// arrange mock(=assert)
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();
		refMailbox.GetDBMailUID().Init(0, 0);

		vector<TD_MAIL_MAILBOX_MAIL_INFO> vecData;
		using testing::_;
		EXPECT_CALL(*m_pClientMailRouter, MailboxIsOpened(m_pPlayer, vecData))
			.Times(1);

		// act
		m_pMailSystem->OpenMailbox(m_pPlayer);
	}

	TEST_FIXTURE(TestMailSystem, RouteOpenMailbox_With_Param_Should_Call_MailboxIsOpened_With_MailData)
	{
		// arrange mock(=assert)
		AddTestMail(m_pPlayer);

		std::vector<TD_MAIL_MAILBOX_MAIL_INFO> vecMailList;
		m_pPlayer->GetMailbox().GetMailListTo_TD(vecMailList);
		CHECK_EQUAL(1, vecMailList.size());
		using testing::_;
		EXPECT_CALL(*m_pClientMailRouter, MailboxIsOpened(m_pPlayer, vecMailList))
			.Times(1);

		// act
		m_pMailSystem->RouteOpenMailbox(m_pPlayer, TEST_MAIL_UID_TOP, TEST_MAIL_UID_BOTTOM);
	}

	TEST_FIXTURE(TestMailSystem, RouteOpenMailbox_NoParam_Should_Call_Without_Data)
	{
		// arrange mock(=assert)
		GPlayerMailBox& refMailbox = m_pPlayer->GetMailbox();
		refMailbox.GetDBMailUID().Init(0, 0);

		AddTestMail(m_pPlayer);
		CHECK_EQUAL(1, m_pPlayer->GetMailbox().GetSize());

		std::vector<TD_MAIL_MAILBOX_MAIL_INFO> vecMailList;
		using testing::_;
		EXPECT_CALL(*m_pClientMailRouter, MailboxIsOpened(m_pPlayer, vecMailList))
			.Times(1);

		// act
		m_pMailSystem->RouteOpenMailbox(m_pPlayer);
	}

	TEST_FIXTURE(TestMailSystem, PostMail_Success)
	{
		// arrange
		MUID uidPlayer = m_pPlayer->GetUID();

		TD_MAIL_POST_REQ_INFO info;
		wstring strMailText;
		GDBT_MAIL_WRITE mail;

		InitData_PostMail(m_pPlayer, info, strMailText, mail); 


		// arrange mock(=assert)
		using testing::Return;
		EXPECT_CALL(*m_pDBManager, MailCheckReceiver(m_pPlayer, mail))
			.WillOnce(Return(true));		// SetStub(성공)

		using testing::_;
		EXPECT_CALL(*m_pClientMailRouter, MailPostRes(_, _))
			.Times(0);						// 성공 시, 라우팅하지 않는다. 라우팅은 DBTask에서 결과에따라 처리.

		// act
		CHECK(m_pMailSystem->PostMail(m_pPlayer, &info, strMailText));
	}

	TEST_FIXTURE(TestMailSystem, PostMail_Failure)
	{
		// arrange
		MUID uidPlayer = m_pPlayer->GetUID();

		TD_MAIL_POST_REQ_INFO info;
		wstring strMailText;
		GDBT_MAIL_WRITE mail;

		InitData_PostMail(m_pPlayer, info, strMailText, mail); 


		// arrange mock(=assert)
		using testing::Return;
		EXPECT_CALL(*m_pDBManager, MailCheckReceiver(m_pPlayer, mail))
			.WillOnce(Return(false));		// SetStub(실패)

		EXPECT_CALL(*m_pClientMailRouter, MailPostRes(m_pPlayer, CR_FAIL_MAIL_POST_DB_ERROR))
			.Times(1);						// 실패 시, 실패를 라우팅한다.

		// act
		CHECK(!m_pMailSystem->PostMail(m_pPlayer, &info, strMailText));
	}

	TEST_FIXTURE(TestMailSystem, UpdateMailBox)
	{
		// arrange
		CID nCID = m_pPlayer->GetCID();

		bool bIsNew = true;

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);
		vector<TD_MAIL_MAILBOX_MAIL_INFO> vecInfo;
		vecInfo.push_back(info);

		// arrange mock(=assert)
		using testing::_;
		EXPECT_CALL(*m_pClientMailRouter, NotifyMail(m_pPlayer, bIsNew, vecInfo));


		// act
		m_pMailSystem->UpdateMailBox(m_pPlayer, &info);


		// assert
		vecInfo.clear();

		GPlayerMailBox& mailBox = m_pPlayer->GetMailbox();		
		mailBox.GetMailListTo_TD(vecInfo);

		ASSERT_EQUAL(1, vecInfo.size());
		CHECK(info == vecInfo[0]);
	}

	TEST_FIXTURE(TestMailSystem, ReadMail)
	{
		// arrange
		MUID uidPlayer = m_pPlayer->GetUID();
		bool bHasText = true;

		GMail* pMail = AddTestMail(m_pPlayer, bHasText);

		// arrange mock (=assert)
		EXPECT_CALL(*m_pDBManager, MailGetContent(uidPlayer, pMail->GetUID(), bHasText));

		// act
		m_pMailSystem->ReadMail(m_pPlayer, pMail->GetUID());
	}

	TEST_FIXTURE(TestMailSystem, ReadMail_AlreadyFilledContent_Should_JustReturn)
	{
		// arrange
		MUID uidPlayer = m_pPlayer->GetUID();
		bool bHasText = true;

		GMail* pMail = AddTestMail(m_pPlayer, bHasText);
		pMail->SetFilledContentForDBTask();

		// arrange mock (=assert)
		using testing::_;
		EXPECT_CALL(*m_pDBManager, MailGetContent(_, _, _))
			.Times(0);						// 성공 시, DB 초라 하지 않는다.

		// act
		m_pMailSystem->ReadMail(m_pPlayer, pMail->GetUID());
	}

	TEST_FIXTURE(TestMailSystem, PutUpItem_Success)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// arrange mock(=assert)
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		using testing::Return;		// set stub
		EXPECT_CALL(*m_pMailItemPutUper, Do(m_pPlayer, Pointee(item)))
			.WillOnce(Return(true));

		EXPECT_CALL(*m_pClientMailRouter, PutUpItemRes(m_pPlayer, Pointee(item)));


		// act
		m_pMailSystem->PutUpItem(m_pPlayer, &item);
	}

	TEST_FIXTURE(TestMailSystem, PutUpItem_Failure)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// arrange mock(=assert)
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		using testing::Return;		// set stub
		EXPECT_CALL(*m_pMailItemPutUper, Do(m_pPlayer, Pointee(item)))
			.WillOnce(Return(false));		

		EXPECT_CALL(*m_pClientMailRouter, PutUpItemRes(m_pPlayer, Pointee(item)))
			.Times(0);


		// act
		m_pMailSystem->PutUpItem(m_pPlayer, &item);
		CHECK(CheckRouteFailure(CR_FAIL));
	}

	TEST_FIXTURE(TestMailSystem, PutDownItem_Success)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// arrange mock(=assert)		
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		using testing::Return;		// set stub
		EXPECT_CALL(*m_pMailItemPutDowner, Do(m_pPlayer, Pointee(item)))
			.WillOnce(Return(true));

		EXPECT_CALL(*m_pClientMailRouter, PutDownItemRes(m_pPlayer, Pointee(item)));


		// act
		m_pMailSystem->PutDownItem(m_pPlayer, &item);
	}

	TEST_FIXTURE(TestMailSystem, PutDownItem_Failure)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// arrange mock(=assert)		
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		using testing::Return;		// set stub
		EXPECT_CALL(*m_pMailItemPutDowner, Do(m_pPlayer, Pointee(item)))
			.WillOnce(Return(false));		

		EXPECT_CALL(*m_pClientMailRouter, PutDownItemRes(m_pPlayer, Pointee(item)))
			.Times(0);


		// act
		m_pMailSystem->PutDownItem(m_pPlayer, &item);
		CHECK(CheckRouteFailure(CR_FAIL));		
	}
}

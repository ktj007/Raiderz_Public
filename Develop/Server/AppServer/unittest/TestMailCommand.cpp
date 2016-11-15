#include "stdafx.h"
#include "PPostOfficeSystem.h"
#include "PFixtureHelper.h"
#include "MockLink.h"
#include "PTestHelper.h"
#include "SUnitTestUtil.h"
#include "CTransData.h"
#include "CCommandTable_App.h"
#include "PGameServerObject.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"
#include "SProxyPlayer.h"
#include "CCommandResultTable.h"
#include "PCmdRouter_Mail.h"
#include "MTest.h"


SUITE(MailCommand)
{
	struct MockPostOfficeSystem: public PPostOfficeSystem 
	{
		MOCK_METHOD2(RouteNewMailToReceiver, void(const SProxyPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo));
		MOCK_METHOD2(RetryNewMailToReceiver, void(const SProxyPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo));
	};

	struct TestMailCommand: public FBaseMail
	{	
		PTestSysWrapper2<PPostOfficeSystem, MockPostOfficeSystem> m_pPostOfficeSystemWrapper;
		MockPostOfficeSystem* m_pPostOfficeSystem;

		TestMailCommand()
		{
			m_pPostOfficeSystem = m_pPostOfficeSystemWrapper.Get();
		}
	};

	TEST_FIXTURE(TestMailCommand, CommandHandler_RouteNewMailToReceiver)
	{
		// arrange
		CID nCID = m_pProxyPlayer->GetCID();
		
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)		
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		EXPECT_CALL(*m_pPostOfficeSystem, RouteNewMailToReceiver(m_pProxyPlayer, Pointee(info)));
		
		

		// act
		MCommandResult nCommandResult = m_pLink->OnRecv(MPC_MAIL_ROUTE_NEW_MAIL_TO_RECEIVER, 2, NEW_INT(nCID), NEW_SINGLE_BLOB(info));		
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}

	TEST_FIXTURE(TestMailCommand, CommandHandler_RouteTransNewMail)
	{
		// arrange
		CID nCID = m_pProxyPlayer->GetCID();
		
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)
		using testing::Pointee;		// 포인터가 가르키는 값 비교
		EXPECT_CALL(*m_pPostOfficeSystem, RetryNewMailToReceiver(m_pProxyPlayer, Pointee(info)));


		// act
		MCommandResult nCommandResult = m_pLink->OnRecv(MPC_MAIL_RETRY_TRANS_NEW_MAIL, 2, NEW_INT(nCID), NEW_SINGLE_BLOB(info));
		CHECK_EQUAL(CR_TRUE, nCommandResult);
	}

	TEST_FIXTURE(TestMailCommand, CommandRouter_TransNewMailReq)
	{
		// arrange
		MUID uidGameServer = m_pGameServerObject->GetUID();
		CID nCID = m_pProxyPlayer->GetCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// act
		PCmdRouter_Mail router;		
		router.TransNewMailReq(uidGameServer, nCID, &info);


		// assert
		CHECK_EQUAL(MPC_MAIL_TRANS_NEW_MAIL, m_pLink->GetCommandID(0));
		CHECK_EQUAL(nCID, m_pLink->GetParam<int>(0, 0));
		
		
		MCommandParameter* pParam = m_pLink->GetCommand(0).GetParameter(1);
		ASSERT_CHECK(pParam != NULL);
		CHECK(info == *(TD_MAIL_MAILBOX_MAIL_INFO*)pParam->GetPointer());		
	}
}

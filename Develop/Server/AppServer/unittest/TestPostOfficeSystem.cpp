#include "stdafx.h"
#include "PPostOfficeSystem.h"
#include "PTestWrapper.h"
#include "PFixtureHelper.h"
#include "PCmdRouter_Mail.h"
#include "SUnitTestUtil.h"
#include "SProxyPlayer.h"
#include "MTest.h"
#include "PBounceMail.h"
#include "PSystem.h"

SUITE(PostOfficeSystem)
{
	struct MockMailRouter: public PCmdRouter_Mail
	{
		MOCK_METHOD3(TransNewMailReq, void(MUID uidGameServer, CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo));
	};

	struct MockSystem: public PSystem
	{
		MOCK_METHOD0(GetNowTime, uint32(void));
	};

	struct MockPostOfficeSystem: public PPostOfficeSystem
	{
		queue<PBounceMail*>& GetBouceMailQueue()
		{
			return m_queueBounceMail;
		}
	};

	struct TestPostOfficeSystem: public FBaseMail
	{
		PTestSysWrapper2<PPostOfficeSystem, MockPostOfficeSystem> m_PostOfficeSystemWrapper;
		PTestSysWrapper2<PSystem, MockSystem> m_SystemWrapper;
		MockPostOfficeSystem* m_pPostOfficeSystem;
		MockSystem* m_pSystem;
		MockMailRouter* m_pMailRouter;

		TestPostOfficeSystem()
		{
			m_pPostOfficeSystem = m_PostOfficeSystemWrapper.Get();
			m_pSystem = m_SystemWrapper.Get();

			m_pMailRouter = new MockMailRouter();
			m_pPostOfficeSystem->SetNewRouter(m_pMailRouter);
		}
	};


	TEST_FIXTURE(TestPostOfficeSystem, RouteNewMailToReceiver_Success)
	{
		// arrange
		MUID uidGameServer = m_pGameServerObject->GetUID();		
		CID nCID = m_pProxyPlayer->GetCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)
		using testing::Pointee;			// 포인터가 가르키는 값 비교
		EXPECT_CALL(*m_pMailRouter, TransNewMailReq(uidGameServer, nCID, Pointee(info)))
			.Times(1);			// 성공 시, 게임서버에 통보한다.


		// act
		m_pPostOfficeSystem->RouteNewMailToReceiver(m_pProxyPlayer, &info);		
	}

	TEST_FIXTURE(TestPostOfficeSystem, RouteNewMailToReceiver_NotFindGameServer)
	{
		// arrange		
		CID nCID = m_pProxyPlayer->GetCID();

		int nUnkownGameServerID = SUnitTestUtil::NewID();
		m_pProxyPlayer->SetGameServerID(nUnkownGameServerID);
		
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)
		using testing::_;
		EXPECT_CALL(*m_pMailRouter, TransNewMailReq(_, _, _))
			.Times(0);			// 게임서버를 찾을 수 없다면, 통보하지 않는다.


		// act
		m_pPostOfficeSystem->RouteNewMailToReceiver(m_pProxyPlayer, &info);		
	}

	TEST_FIXTURE(TestPostOfficeSystem, RetryNewMailToReceiver)
	{
		// arrange 
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);

		queue<PBounceMail*>& queueBouceMail = m_pPostOfficeSystem->GetBouceMailQueue();
		ASSERT_EQUAL(0, queueBouceMail.size());
		

		// act		
		m_pPostOfficeSystem->RetryNewMailToReceiver(m_pProxyPlayer, &info);

		
		// assert		
		ASSERT_EQUAL(1, queueBouceMail.size());

		PBounceMail* pMail = queueBouceMail.front();
		CHECK_EQUAL(m_pProxyPlayer->GetCID(), pMail->GetCID());		

		TD_MAIL_MAILBOX_MAIL_INFO bounceMailInfo;
		pMail->Export(&bounceMailInfo);

		CHECK(info == bounceMailInfo);
	}

	TEST_FIXTURE(TestPostOfficeSystem, Update)
	{
		// arrange
		MUID uidGameServer = m_pGameServerObject->GetUID();
		
		CID nCID = m_pProxyPlayer->GetCID();
		
		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);

		
		// arrange mock(=assert)
		using testing::Return;
		EXPECT_CALL(*m_pSystem, GetNowTime())
			.WillOnce(Return(10000))				// SetStub, 반송메일 등록 시간 용
			.WillOnce(Return(40000));				// SetStub, 반송 시간 비교 용

		using testing::Pointee;						// 포인터가 가리키는 값 비교
		EXPECT_CALL(*m_pMailRouter, TransNewMailReq(uidGameServer, nCID, Pointee(info)));

		
		// act		
		m_pPostOfficeSystem->RetryNewMailToReceiver(m_pProxyPlayer, &info);

		m_pPostOfficeSystem->Update();


		// assert
		queue<PBounceMail*>& queueBouceMail = m_pPostOfficeSystem->GetBouceMailQueue();
		CHECK(queueBouceMail.empty());
	}

	TEST_FIXTURE(TestPostOfficeSystem, Update_NoTime)
	{
		// arrange
		MUID uidGameServer = m_pGameServerObject->GetUID();

		CID nCID = m_pProxyPlayer->GetCID();

		TD_MAIL_MAILBOX_MAIL_INFO info;
		InitMailInfo(info);


		// arrange mock(=assert)
		using testing::Return;
		EXPECT_CALL(*m_pSystem, GetNowTime())
			.WillOnce(Return(10000))				// SetStub, 반송메일 등록 시간 용
			.WillOnce(Return(30000));				// SetStub, 재발송 시간 비교 용

		using testing::_;
		EXPECT_CALL(*m_pMailRouter, TransNewMailReq(_, _, _))
			.Times(0);								// 시간이 안되면, 재전송하지 않는다.


		// act		
		m_pPostOfficeSystem->RetryNewMailToReceiver(m_pProxyPlayer, &info);

		m_pPostOfficeSystem->Update();


		// assert
		queue<PBounceMail*>& queueBouceMail = m_pPostOfficeSystem->GetBouceMailQueue();
		CHECK_EQUAL(1, queueBouceMail.size());
	}
};

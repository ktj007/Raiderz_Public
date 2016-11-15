#include "stdafx.h"
#include "FBaseGame.h"
#include "GReserveCmdManager.h"
#include "GUnitTestUtil.h"

using testing::Return;

SUITE(ReserveCmdManager)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{
			m_nCID = SUnitTestUtil::NewCID();
		}

		~Fixture()
		{

		}

		int m_nCID;

		GTestMgrWrapper<GReserveCmdManager> m_wrapperReserveCmdManager;
	};

	TEST_FIXTURE(Fixture, Reserve)
	{
		gmgr.pReserveCmdManager->Reserve(m_nCID, new MCommand());

		GReserveCmd* pReserveCmd = gmgr.pReserveCmdManager->GetReserveCmd(m_nCID);
		ASSERT_CHECK(NULL != pReserveCmd);
	}

	TEST_FIXTURE(Fixture, RunReserveCmd)
	{
		class MockCommand : public MCommand
		{
		public:
			MOCK_METHOD0(Run, MCommandResult ());
		};

		MockCommand* pMockCmd = new MockCommand();

		EXPECT_CALL(*pMockCmd, Run()).Times(1).WillRepeatedly(Return(CR_TRUE));

		gmgr.pReserveCmdManager->Reserve(m_nCID, pMockCmd);
		gmgr.pReserveCmdManager->RunReserveCmd(m_nCID);
	}

	TEST_FIXTURE(Fixture, DeleteReserveCmd)
	{
		gmgr.pReserveCmdManager->Reserve(m_nCID, new MCommand());
		gmgr.pReserveCmdManager->DeleteReserveCmd(m_nCID);

		CHECK(NULL == gmgr.pReserveCmdManager->GetReserveCmd(m_nCID));
	}

	// 예약시간 초과시 예약된 커맨드 삭제
	TEST_FIXTURE(Fixture, ReserveTimeOver)
	{
		GTestSysWrapper2<GSystem, GUnitTestUtil::FakeSystem>	FakeSystemWrapper;

		gmgr.pReserveCmdManager->Reserve(m_nCID, new MCommand());

		gsys.pSystem->Sleep((TIMEOUT_TICK_MOVE_SERVER / 1000) - 1);
		gmgr.pReserveCmdManager->Update();
		CHECK(NULL != gmgr.pReserveCmdManager->GetReserveCmd(m_nCID));

		gsys.pSystem->Sleep(1);
		gmgr.pReserveCmdManager->Update();
		CHECK(NULL == gmgr.pReserveCmdManager->GetReserveCmd(m_nCID));
	}
}
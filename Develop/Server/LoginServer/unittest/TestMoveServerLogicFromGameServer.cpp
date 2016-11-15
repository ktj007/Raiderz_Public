#include "stdafx.h"
#include "LMoveServerLogicFromGameServer.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "LPlayerAcceptManager.h"
#include "FBasePlayer.h"
#include "LFixtureHelper.h"
#include "LUnitTestUtil.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "MockLink.h"
#include "CCommandResultTable.h"
#include "CCommandTable_Login.h"


SUITE(MoveServerLogicFromGameServer)
{
	struct Fixture : public FBasePlayer, FBaseMockLink
	{
		Fixture()
		{
			m_uidTestSession = LUnitTestUtil::NewUID();
			m_pPlayerLink = NewLink(m_uidTestSession);
		}
		~Fixture()
		{

		}

		MUID							m_uidTestSession;
		MockLink*						m_pPlayerLink;

		LMoveServerLogicFromGameServer	m_MoveServeLogicFromGS;
		LTestMgrWrapper<LPlayerAcceptManager>	m_wrprPlayerAcceptManager;
	};

	TEST_FIXTURE(Fixture, TestMoveServerLogicFromGameServer_Success)
	{
		/// Arrange
		MUID uidConnectionKey = LUnitTestUtil::NewUID();
		AID nAID = LUnitTestUtil::NewAID();
		wstring strName = LUnitTestUtil::NewName();
		MUID uidPlayer = LUnitTestUtil::NewUID();


		LPlayerAcceptor_FromGameServer* pAccepter = new LPlayerAcceptor_FromGameServer();
		pAccepter->ReserveAccept(uidConnectionKey, nAID, strName, uidPlayer);

		gmgr.pPlayerAcceptManager->Add(pAccepter);

		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());
		CHECK_EQUAL(0, gmgr.pPlayerObjectManager->GetClientsCount());

		/// Act
		bool bRet = m_MoveServeLogicFromGS.MoveLoginServer(m_uidTestSession, uidConnectionKey);

		/// Assert
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());
		CHECK_EQUAL(1, gmgr.pPlayerObjectManager->GetClientsCount());

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_RESPONSE_MOVE_LOGIN_SERVER, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(CR_SUCCESS, m_pPlayerLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestMoveServerLogicFromGameServer_Failure_NotExistAccepter)
	{
		/// Arrange
		MUID uidConnectionKey = LUnitTestUtil::NewUID();

		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());
		CHECK_EQUAL(0, gmgr.pPlayerObjectManager->GetClientsCount());

		/// Act
		bool bRet = m_MoveServeLogicFromGS.MoveLoginServer(m_uidTestSession, uidConnectionKey);

		/// Assert
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());
		CHECK_EQUAL(0, gmgr.pPlayerObjectManager->GetClientsCount());

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_RESPONSE_MOVE_LOGIN_SERVER, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_LOGIN, m_pPlayerLink->GetParam<int>(0, 0));
	}

}
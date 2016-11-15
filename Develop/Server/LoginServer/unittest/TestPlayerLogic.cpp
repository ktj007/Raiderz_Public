#include "stdafx.h"
#include "FBasePlayer.h"
#include "LFixtureHelper.h"
#include "LPlayerLogic.h"
#include "MockLink.h"
#include "CCommandTable.h"
#include "CCommandTable_Master.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "LPlayerAcceptManager.h"
#include "FBaseMasterClient.h"


SUITE(PlayerLogic)
{
	struct Fixture : public FBasePlayer, FBaseMockLink, FBaseMasterClient
	{
		Fixture() {}
		~Fixture() {}

		LPlayerLogic m_logic;

		LTestMgrWrapper<LPlayerAcceptManager> m_wrprPlayerAcceptManager;
	};

	TEST_FIXTURE(Fixture, TestPlayerLogic_DuplicatedPlayerLogin_Success)
	{
		MUID uidPlayer = LTestHelper::NewUID();
		MUID uidConnectionKey = MUID::ZERO;

		NewTestPlayerObject(uidPlayer);
		MockLink* pLink = NewLink(uidPlayer);

		CHECK_EQUAL(false, pLink->IsCallHardDisconnect());

		m_logic.DuplicatedPlayerLogin(uidPlayer, uidConnectionKey);

		CHECK_EQUAL(true, pLink->IsCallHardDisconnect());
		ASSERT_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_DUPLICATED_PLAYER_LOGIN, pLink->GetCommandID(0));
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_DuplicatedPlayerLogin_Failure_NotExistPlayer)
	{
		MUID uidNotExistPlayer = LTestHelper::NewUID();
		MUID uidConnectionKey = MUID::ZERO;

		MockLink* pLink = NewLink(uidNotExistPlayer);

		m_logic.DuplicatedPlayerLogin(uidNotExistPlayer, uidConnectionKey);

		ASSERT_EQUAL(0, pLink->GetCommandCount());
	}

	TEST_FIXTURE(Fixture, TestPlayerLogic_DuplicatedPlayerLogin_Success_Acceptor)
	{
		MUID uidConnectionKey			= LTestHelper::NewUID();
		AID nAID						= LTestHelper::NewID();
		wstring strUserID				= LTestHelper::NewName();
		MUID uidPlayerFromGameServer	= LTestHelper::NewUID();
		
		LPlayerAcceptor_FromGameServer* pAcceptor = new LPlayerAcceptor_FromGameServer();
		pAcceptor->ReserveAccept(uidConnectionKey, nAID, strUserID, uidPlayerFromGameServer);
		
		gmgr.pPlayerAcceptManager->Add(pAcceptor);
		
		MUID uidNotExistPlayer = LTestHelper::NewUID();
		m_logic.DuplicatedPlayerLogin(uidNotExistPlayer, uidConnectionKey);

		ASSERT_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MLC_PLAYER_DELETE_PLAYER, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(nAID, m_pNetAgent->GetSendParam<int>(0, 0));
		CHECK_EQUAL(uidPlayerFromGameServer, m_pNetAgent->GetSendParam<MUID>(0, 1));
	}

}
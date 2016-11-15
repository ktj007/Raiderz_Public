#include "stdafx.h"
#include "FBaseMasterClient.h"
#include "LCmdRouter_MoveServer.h"
#include "LTestHelper.h"
#include "CCommandTable_Master.h"
#include "LFixtureHelper.h"
#include "CCommandTable_Login.h"
#include "MockLink.h"
#include "CCommandResultTable.h"


SUITE(CmdRouter_MoveServer)
{
	struct FCmdRouter_MoveServer :public FBaseMasterClient, FBaseMockLink
	{
		FCmdRouter_MoveServer()
		{
			m_uidPlayer = LTestHelper::NewUID();
			m_pPlayerLink = LTestHelper::NewLink(m_uidPlayer);
		}
		~FCmdRouter_MoveServer()
		{
		}

		MUID		m_uidPlayer;
		MockLink*	m_pPlayerLink;
	};

	TEST_FIXTURE(FCmdRouter_MoveServer, TestFCmdRouter_MoveServer_RouteReadyEnterLoginServerRes)
	{
		MUID uidRequesterGameServer = LTestHelper::NewUID();
		MUID uidRequesterPlayer = LTestHelper::NewUID();
		int nResult = CR_SUCCESS;
		MUID uidConnectionKey = LTestHelper::NewUID();

		LCmdRouter_MoveServer::RouteReadyEnterLoginServerRes(uidRequesterGameServer, uidRequesterPlayer, nResult, uidConnectionKey);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MLC_COMM_READY_ENTER_LOGINSERVER_RES, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(uidRequesterGameServer, m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(uidRequesterPlayer, m_pNetAgent->GetSendParam<MUID>(0, 1));
		CHECK_EQUAL(nResult, m_pNetAgent->GetSendParam<int>(0, 2));
		CHECK_EQUAL(uidConnectionKey, m_pNetAgent->GetSendParam<MUID>(0, 3));
	}

	TEST_FIXTURE(FCmdRouter_MoveServer, TestFCmdRouter_MoveServer_RouteResponseMoveLoginServer)
	{
		int nResult = CR_SUCCESS;
		MUID uidAllocUID = LTestHelper::NewUID();

		LCmdRouter_MoveServer::RouteResponseMoveLoginServer(m_uidPlayer, nResult, uidAllocUID);

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_RESPONSE_MOVE_LOGIN_SERVER, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(nResult, m_pPlayerLink->GetParam<int>(0, 0));
		CHECK_EQUAL(uidAllocUID, m_pPlayerLink->GetParam<MUID>(0, 1));
	}

}
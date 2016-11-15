#include "stdafx.h"
#include "LFixtureHelper.h"
#include "LTestWrapper.h"
#include "MockLink.h"
#include "CCommandTable_Login.h"
#include "CCommandTable_Master.h"
#include "FBaseMasterClient.h"
#include "LGlobal.h"
#include "LPlayerAcceptManager.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "CCommandResultTable.h"


SUITE(CmdHandler_MoveServer)
{
	struct FCmdHandler_MoveServer :public FBaseMockLink
	{
		FCmdHandler_MoveServer()
		{
			m_uidPlayer = LTestHelper::NewUID();
			m_pMockLink = NewLink(m_uidPlayer);
		}
		~FCmdHandler_MoveServer()
		{

		}

		MUID		m_uidPlayer;
		MockLink*	m_pMockLink;
	};

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_OnRecv_MC_COMM_REQUEST_MOVE_LOGIN_SERVER)
	{
		MUID uidConnectionKey = LTestHelper::NewUID();
		int nCommandVersion = LTestHelper::NewID();

		MCommandResult result = m_pMockLink->OnRecv(MC_COMM_REQUEST_MOVE_LOGIN_SERVER
			, 2
			, NEW_UID(uidConnectionKey)
			, NEW_INT(nCommandVersion)
			);

		CHECK_EQUAL(CR_TRUE, result);
	}

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_OnRecv_MC_COMM_REQUEST_MOVE_LOGIN_SERVER_ON_PMANG)
	{
		MUID uidConnectionKey = LTestHelper::NewUID();
		int nCommandVersion = LTestHelper::NewID();
		wstring strStatIndex = L"TestStatIndex";
		int nPCCafeID = LTestHelper::NewID();

		MCommandResult result = m_pMockLink->OnRecv(MC_COMM_REQUEST_MOVE_LOGIN_SERVER_ON_PMANG
			, 4
			, NEW_UID(uidConnectionKey)
			, NEW_INT(nCommandVersion)
			, NEW_WSTR(strStatIndex.c_str())
			, NEW_INT(nPCCafeID)
			);

		CHECK_EQUAL(CR_TRUE, result);
	}

	struct FCmdHandler_MasterServer_MoveServer : public FBaseMasterClient
	{
		FCmdHandler_MasterServer_MoveServer()
		{
		}
		~FCmdHandler_MasterServer_MoveServer()
		{

		}

		MCommandResult OnRecv_MLC_COMM_READY_ENTER_LOGINSERVER_REQ(MUID uidRequestGameServer, MUID uidRequestPlayer, MUID uidConnectionKey, AID nAID, wstring strUserID)
		{
			return m_pNetAgent->OnRecv(MLC_COMM_READY_ENTER_LOGINSERVER_REQ
									, 5
									, NEW_UID(uidRequestGameServer)
									, NEW_UID(uidRequestPlayer)
									, NEW_UID(uidConnectionKey)
									, NEW_INT(nAID)
									, NEW_WSTR(strUserID.c_str())
									);
		}
		MCommandResult OnRecv_MLC_COMM_CANCEL_MOVE_LOGINSERVER(MUID uidConnectionKey)
		{
			return m_pNetAgent->OnRecv(MLC_COMM_CANCEL_MOVE_LOGINSERVER
									, 1
									, NEW_UID(uidConnectionKey)
									);
		}

		LTestMgrWrapper<LPlayerAcceptManager> m_wrapperPlayerAcceptManager;
	};

	TEST_FIXTURE(FCmdHandler_MasterServer_MoveServer, TestCmdHandler_MasterServer_MoveServer_OnReadyEnterLoginServerReq)
	{
		MUID	uidRequesterGameServer = LTestHelper::NewUID();
		MUID	uidRequesterPlayer = LTestHelper::NewUID();
		MUID	uidConnectionKey = LTestHelper::NewUID();
		AID		nAID = LTestHelper::NewID();
		wstring	strUserID = LTestHelper::NewName();

		MCommandResult result = OnRecv_MLC_COMM_READY_ENTER_LOGINSERVER_REQ(uidRequesterGameServer, uidRequesterPlayer, uidConnectionKey, nAID, strUserID.c_str());
		
		CHECK_EQUAL(CR_TRUE, result);

		CHECK((LPlayerAcceptor_FromGameServer*)NULL !=  gmgr.pPlayerAcceptManager->FindAcceptor(uidConnectionKey));
		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MLC_COMM_READY_ENTER_LOGINSERVER_RES, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(uidRequesterGameServer, m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(uidRequesterPlayer, m_pNetAgent->GetSendParam<MUID>(0, 1));
		CHECK_EQUAL(CR_SUCCESS, m_pNetAgent->GetSendParam<int>(0, 2));
		CHECK_EQUAL(uidConnectionKey, m_pNetAgent->GetSendParam<MUID>(0, 3));
	}

	TEST_FIXTURE(FCmdHandler_MasterServer_MoveServer, TestCmdHandler_MasterServer_MoveServer_OnCancelMoveLoginServer)
	{
		MUID	uidConnectionKey = LTestHelper::NewUID();
		AID		nAID = LTestHelper::NewID();
		wstring	strUserID = LTestHelper::NewName();
		MUID uidPlayer = LTestHelper::NewUID();

		LPlayerAcceptor_FromGameServer* pAcceptor = new LPlayerAcceptor_FromGameServer();
		pAcceptor->ReserveAccept(uidConnectionKey, nAID, strUserID, uidPlayer);
		gmgr.pPlayerAcceptManager->Add(pAcceptor);
		CHECK_EQUAL(1, gmgr.pPlayerAcceptManager->GetSize());

		MCommandResult result = OnRecv_MLC_COMM_CANCEL_MOVE_LOGINSERVER(uidConnectionKey);

		CHECK_EQUAL(CR_TRUE, result);
		CHECK_EQUAL(0, gmgr.pPlayerAcceptManager->GetSize());
	}

}

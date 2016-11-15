#include "stdafx.h"
#include "FBaseNetClient.h"
#include "GUnitTestUtil.h"
#include "CCommandTable.h"
#include "CCommandTable_Master.h"
#include "CCommandTable_Login.h"
#include "CCommandResultTable.h"
#include "FBasePlayer.h"
#include "MockField.h"
#include "GUTHelper_Field.h"
#include "GEntityPlayer.h"
#include "GPlayerObject.h"
#include "FBaseMockLink.h"
#include "MockLink.h"


SUITE(CmdHandler_MoveServer)
{
	struct FCmdHandler_MoveServer :public FBaseNetClient, FBasePlayer, FBaseMockLink
	{
		FCmdHandler_MoveServer()
		{
			MockField* pField = GUTHelper_Field::DefaultMockField();
			m_pEntityPlayer = NewEntityPlayer(pField);
			m_PlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_pEntityPlayer->GetUID());

			m_pPlayerLink = NewLink(m_pEntityPlayer);

			m_pNetAgent->ResetSendCommands();
		}
		~FCmdHandler_MoveServer()
		{

		}

		MCommandResult RecvCommand_ReserveMoveLoginServerRes(const MUID& uidRequestPlayer, int nResult, const MUID& uidConnectionKey)
		{
			return m_pNetAgent->OnRecv(MMC_COMM_RESERVE_MOVE_LOGINSERVER_RES,
				3,
				NEW_UID(uidRequestPlayer),
				NEW_INT(nResult),
				NEW_UID(uidConnectionKey));
		}

		GPlayerObject*	m_PlayerObject;
		GEntityPlayer*	m_pEntityPlayer;
		MockLink*		m_pPlayerLink;
	};
	
	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_RESERVE_MOVE_LOGINSERVER_RES_CheckParam)
	{
		MUID uidRequesterPlayer = GUnitTestUtil::NewUID();
		int nResult = CR_SUCCESS;
		MUID uidConnectionKey = GUnitTestUtil::NewUID();
		MCommandResult result = RecvCommand_ReserveMoveLoginServerRes(uidRequesterPlayer, nResult, uidConnectionKey);

		CHECK_EQUAL(CR_TRUE, result);
	}

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_RESERVE_MOVE_LOGINSERVER_RES_Success)
	{
		m_PlayerObject->OnReadyMoveLoginServer();
		CHECK_EQUAL(true, m_PlayerObject->IsReadyMoveLoginServer());

		int nResult = CR_SUCCESS;
		MUID uidConnectionKey = GUnitTestUtil::NewUID();
		RecvCommand_ReserveMoveLoginServerRes(m_PlayerObject->GetUID(), nResult, uidConnectionKey);

		CHECK_EQUAL(false, m_PlayerObject->IsReadyMoveLoginServer());
		CHECK_EQUAL(true, m_PlayerObject->IsMoveLoginServer());
	}

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_RESERVE_MOVE_LOGINSERVER_RES_Failure_Not_CR_SUCCESS)
	{
		m_PlayerObject->OnReadyMoveLoginServer();

		int nResult = CR_FAIL;
		MUID uidConnectionKey = GUnitTestUtil::NewUID();
		RecvCommand_ReserveMoveLoginServerRes(m_PlayerObject->GetUID(), nResult, uidConnectionKey);

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_COMM_CANCEL_MOVE_LOGINSERVER, m_pNetAgent->GetSendCommandID(0));

		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(nResult, m_pPlayerLink->GetParam<int>(0, 0));
	}

	TEST_FIXTURE(FCmdHandler_MoveServer, TestCmdHandler_MoveServer_RESERVE_MOVE_LOGINSERVER_RES_Failure_Not_PlayerObject_IsReadyMoveLoginServer)
	{
		m_PlayerObject->OnStartGame();

		int nResult = CR_SUCCESS;
		MUID uidConnectionKey = GUnitTestUtil::NewUID();
		RecvCommand_ReserveMoveLoginServerRes(m_PlayerObject->GetUID(), nResult, uidConnectionKey);

		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(0, m_pPlayerLink->GetCommandCount());
	}

}

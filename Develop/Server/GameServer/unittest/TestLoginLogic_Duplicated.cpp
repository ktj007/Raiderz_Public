#include "stdafx.h"
#include "FBasePlayer.h"
#include "FBaseNetClient.h"
#include "FBaseMockLink.h"
#include "GLoginLogic.h"
#include "MockField.h"
#include "GUTHelper_Field.h"
#include "GEntityPlayer.h"
#include "CCommandTable.h"
#include "GPlayerAcceptManager.h"
#include "GPlayerAcceptor_FromGameServer.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GUnitTestUtil.h"
#include "CCommandTable_Master.h"
#include "GUTHelper_Login.h"
#include "GPlayerObject.h"


SUITE(LoginLogic_Duplicated)
{
	struct Fixture : public FBasePlayer, FBaseNetClient, FBaseMockLink
	{
		Fixture()
		{
		}
		~Fixture()
		{
		}

		GLoginLogic m_logic;
	};

	TEST_FIXTURE(Fixture, TestLoginLogic_Duplicated_Success_ExistPlayer)
 	{
		MockField* pMockField = GUTHelper_Field::NewMockField();
		GEntityPlayer* pEntityPlayer = NewEntityPlayer(pMockField);
		GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(pEntityPlayer->GetUID());
		MUID uidConnectionKey = MUID::ZERO;

		MockLink* pLink = NewLink(pEntityPlayer);

		bool bRet = m_logic.DuplicatedLogin(pEntityPlayer->GetUID(), uidConnectionKey);

		CHECK_EQUAL(true, bRet);
		ASSERT_EQUAL(MC_COMM_DUPLICATED_PLAYER_LOGIN, pLink->GetCommandID(0));
		CHECK_EQUAL(true, pLink->IsCallHardDisconnect());

		pMockField->Destroy();
		SAFE_DELETE(pMockField);
	}

	TEST_FIXTURE(Fixture, TestLoginLogic_Duplicated_Success_ExistAcceptorFromLoginServer)
	{
		// Arrange
		GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
		GPlayerAcceptor_FromLoginServer* pAcceptor = new GPlayerAcceptor_FromLoginServer();
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo;
		tdLoginAccountInfo.nAID = GUnitTestUtil::NewAID();
		tdLoginAccountInfo.nCID = GUnitTestUtil::NewCID();
		MUID uidConnectionKey = GUnitTestUtil::NewUID();
		MUID uidPlayer = GUnitTestUtil::NewUID();
		pAcceptor->ReserveLogin(&tdLoginAccountInfo, uidConnectionKey, uidPlayer);
		pAcceptManager->Add(pAcceptor, GPlayerAcceptor::FROM_LOGIN_SERVER);
		
		CHECK_EQUAL(1, pAcceptManager->GetSize(GPlayerAcceptor::FROM_LOGIN_SERVER));

		// Act
		bool bRet = m_logic.DuplicatedLogin(uidPlayer, uidConnectionKey);
		
		// Assert
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(0, pAcceptManager->GetSize(GPlayerAcceptor::FROM_LOGIN_SERVER));

		ASSERT_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_PLAYER_DELETE_PLAYER,	m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(tdLoginAccountInfo.nAID,	m_pNetAgent->GetSendParam<int>(0, 0));
		CHECK_EQUAL(uidPlayer,					m_pNetAgent->GetSendParam<MUID>(0, 1));
	}

	TEST_FIXTURE(Fixture, TestLoginLogic_Duplicated_Success_ExistAcceptorFromGameServer)
	{
		// Arrange
		MockField* pMockField = GUTHelper_Field::NewMockField();
		GEntityPlayer* pEntityPlayer = NewEntityPlayer(pMockField);
		GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(pEntityPlayer->GetUID());

		GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
		GPlayerAcceptor_FromGameServer* pAcceptor = new GPlayerAcceptor_FromGameServer();
		MUID uidConnectionKey = GUnitTestUtil::NewUID();
		MUID uidGameServer = GUnitTestUtil::NewUID();
		MCommand* pCommand = GUTHelper_Login::NewCommand_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER(pEntityPlayer, uidConnectionKey, uidGameServer, m_pNetAgent->GetUID());
		pAcceptor->ReserveLogin(pCommand);
		pAcceptManager->Add(pAcceptor, GPlayerAcceptor::FROM_GAME_SERVER);

		CHECK_EQUAL(1, pAcceptManager->GetSize(GPlayerAcceptor::FROM_GAME_SERVER));

		AID nAID = (AID)pPlayerObject->GetAccountInfo().nAID;
		MUID uidPlayer = pPlayerObject->GetUID();

		gmgr.pPlayerObjectManager->DeletePlayer(uidPlayer);

		m_pNetAgent->ResetSendCommands();

		// Act
		bool bRet = m_logic.DuplicatedLogin(pEntityPlayer->GetUID(), uidConnectionKey);

		// Assert
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(0, pAcceptManager->GetSize(GPlayerAcceptor::FROM_GAME_SERVER));

		ASSERT_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_PLAYER_DELETE_PLAYER,	m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(nAID,						m_pNetAgent->GetSendParam<int>(0, 0));
		CHECK_EQUAL(uidPlayer,					m_pNetAgent->GetSendParam<MUID>(0, 1));

		// Clear
		SAFE_DELETE(pCommand);

		pMockField->Destroy();
		SAFE_DELETE(pMockField);
	}

}
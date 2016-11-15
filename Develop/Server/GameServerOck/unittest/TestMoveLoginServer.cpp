#include "stdafx.h"
#include "FBasePlayer.h"
#include "GAsyncTask.h"
#include "GUTHelper_Player.h"
#include "GEntityPlayer.h"
#include "GServer.h"
#include "GPlayerSystem.h"
#include "GPlayerObject.h"
#include "FBaseMockLink.h"
#include "GPlayerDoing.h"
#include "CCommandTable.h"
#include "CCommandResultTable.h"
#include "GUnitTestUtil.h"
#include "FBaseNetClient.h"
#include "CCommandTable_Master.h"
#include "GUTHelper_Field.h"
#include "GUTHelper_NPC.h"
#include "GEntityNPC.h"
#include "MockField.h"
#include "GActorHateTracker.h"
#include "GFieldInfoMgr.h"
#include "GFieldMgr.h"
#include "GPlayerField.h"
#include "GPlayerFieldShared.h"


SUITE(MoveLoginServer)
{
	struct Fixture : public FBasePlayer, FBaseMockLink, FBaseNetClient
	{
		Fixture()
		{
			InitAsyncTaskManager();

			m_pEntityPlayer = m_helper.NewPlayer();
			m_pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_pEntityPlayer->GetUID());
			m_pPlayerObject->OnStartGame();

			m_pPlayerLink = NewLink(m_pEntityPlayer);
		}
		~Fixture()
		{
			InitAsyncTaskManager();
		}

		void InitAsyncTaskManager()
		{
			m_pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();
			m_pAsyncTaskManager->Clear();
		}

		GUTHelper_Player	m_helper;
		GEntityPlayer*		m_pEntityPlayer;
		GPlayerObject*		m_pPlayerObject;
		MockLink*			m_pPlayerLink;
		GAsyncTaskManager*	m_pAsyncTaskManager;
	};

	namespace Start
	{
		TEST_FIXTURE(Fixture, TestMoveLoginServer_Success)
		{
			bool bRet = gsys.pPlayerSystem->MoveToSelectChar(m_pPlayerObject);
			CHECK_EQUAL(true, bRet);

			CHECK_EQUAL(1, m_pAsyncTaskManager->GetTaskCount());
			CHECK_EQUAL(POS_READY_MOVE_LOGIN_SERVER, m_pPlayerObject->GetState());
		}

		TEST_FIXTURE(Fixture, TestMoveLoginServer_Failure_InBusy)
		{
			m_pEntityPlayer->GetDoing().ChangeDoing(CD_INTERACTING);

			bool bRet = gsys.pPlayerSystem->MoveToSelectChar(m_pPlayerObject);
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pAsyncTaskManager->GetTaskCount());

			CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_MSG_RES, m_pPlayerLink->GetCommandID(0));
			CHECK_EQUAL(MT_ADVICE, m_pPlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(CR_MOVE_TO_SELECT_CHAR_BUSY, m_pPlayerLink->GetParam<int>(0, 4));
		}
		TEST_FIXTURE(Fixture, TestMoveLoginServer_Failure_InState_ReadyMoveLoginServer)
		{
			m_pPlayerObject->OnReadyMoveLoginServer();

			bool bRet = gsys.pPlayerSystem->MoveToSelectChar(m_pPlayerObject);
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pAsyncTaskManager->GetTaskCount());

			CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_MSG_RES, m_pPlayerLink->GetCommandID(0));
			CHECK_EQUAL(MT_SYSTEM, m_pPlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(CR_MOVE_TO_SELECT_CHAR_INVALID_STATE, m_pPlayerLink->GetParam<int>(0, 4));
		}

		TEST_FIXTURE(Fixture, TestMoveLoginServer_Failure_InState_MoveLoginServer)
		{
			m_pPlayerObject->OnMoveLoginServer();

			bool bRet = gsys.pPlayerSystem->MoveToSelectChar(m_pPlayerObject);
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pAsyncTaskManager->GetTaskCount());

			CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_MSG_RES, m_pPlayerLink->GetCommandID(0));
			CHECK_EQUAL(MT_SYSTEM, m_pPlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(CR_MOVE_TO_SELECT_CHAR_INVALID_STATE, m_pPlayerLink->GetParam<int>(0, 4));
		}

		TEST_FIXTURE(Fixture, TestMoveLoginServer_Failure_InNotPOS_INWORLD)
		{
			m_pPlayerObject->OnEndLogin();

			bool bRet = gsys.pPlayerSystem->MoveToSelectChar(m_pPlayerObject);
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pAsyncTaskManager->GetTaskCount());

			CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_MSG_RES, m_pPlayerLink->GetCommandID(0));
			CHECK_EQUAL(MT_SYSTEM, m_pPlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(CR_MOVE_TO_SELECT_CHAR_INVALID_STATE, m_pPlayerLink->GetParam<int>(0, 4));
		}

		TEST_FIXTURE(Fixture, TestMoveLoginServer_Failure_IsNowCombat)
		{
			MockField* pMockField = GUTHelper_Field::NewMockField();
			GEntityNPC* pMockNPC = GUTHelper_NPC::NewMockNPC<GEntityNPC>(pMockField);

			GActorHateTracker& refHateTracker = m_pEntityPlayer->GetActorHateTracker();
			reinterpret_cast<GActorObserver*>(&refHateTracker)->OnAddHateTarget(pMockNPC);

			CHECK_EQUAL(false, refHateTracker.IsEmpty());
			CHECK_EQUAL(true, m_pEntityPlayer->IsNowCombat());

			bool bRet = gsys.pPlayerSystem->MoveToSelectChar(m_pPlayerObject);
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pAsyncTaskManager->GetTaskCount());

			CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_MSG_RES, m_pPlayerLink->GetCommandID(0));
			CHECK_EQUAL(MT_ADVICE, m_pPlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(CR_MOVE_TO_SELECT_CHAR_DONT_DURING_COMBAT, m_pPlayerLink->GetParam<int>(0, 4));

			pMockField->Destroy();
			SAFE_DELETE(pMockField);
		}

		TEST_FIXTURE(Fixture, TestMoveLoginServer_Failure_WhenPrepareChannel)
		{
			int nChannelID = GUnitTestUtil::NewID();
			m_pEntityPlayer->GetPlayerField().GetFieldShared().PrepareChangeChannel(nChannelID);
			m_pPlayerLink->ResetCommands();

			CHECK_EQUAL(true, m_pEntityPlayer->GetPlayerField().GetFieldShared().IsPreparedChangeChannel());

			bool bRet = gsys.pPlayerSystem->MoveToSelectChar(m_pPlayerObject);
			CHECK_EQUAL(false, bRet);
			CHECK_EQUAL(0, m_pAsyncTaskManager->GetTaskCount());

			CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
			CHECK_EQUAL(MC_MSG_RES, m_pPlayerLink->GetCommandID(0));
			CHECK_EQUAL(MT_SYSTEM, m_pPlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(CR_MOVE_TO_SELECT_CHAR_INVALID_STATE, m_pPlayerLink->GetParam<int>(0, 4));
		}
	}

	TEST_FIXTURE(Fixture, TestMoveLoginServer_PlayerObject_Method_SetMoveLoginServerConnectionKey_Success)
	{
		// Arrange
		m_pPlayerObject->OnReadyMoveLoginServer();
		CHECK_EQUAL(POS_READY_MOVE_LOGIN_SERVER, m_pPlayerObject->GetState());
		MUID uidConnectionKey = GUnitTestUtil::NewUID();

		// Execute
		m_pPlayerObject->SetMoveLoginServerConnectionKey(uidConnectionKey);

		// Assert
		CHECK_EQUAL(POS_MOVE_LOGIN_SERVER, m_pPlayerObject->GetState());
	}

	TEST_FIXTURE(Fixture, TestMoveLoginServer_PlayerObject_Method_CancelMoveLoginServer_Success)
	{
		// Arrange
		m_pPlayerObject->OnReadyMoveLoginServer();
		CHECK_EQUAL(POS_READY_MOVE_LOGIN_SERVER, m_pPlayerObject->GetState());
		
		// Execute
		m_pPlayerObject->CancelMoveLoginServer(CR_FAIL);

		// Assert
		CHECK_EQUAL(POS_INWORLD, m_pPlayerObject->GetState());
		
		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL, m_pPlayerLink->GetParam<int>(0, 0));

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		CHECK_EQUAL(MMC_COMM_CANCEL_MOVE_LOGINSERVER, m_pNetAgent->GetSendCommandID(0));
	}

	TEST_FIXTURE(Fixture, TestMoveLoginServer_PlayerObject_Method_CancelMoveLoginServer_Failure)
	{
		// Arrange
		CHECK_EQUAL(POS_INWORLD, m_pPlayerObject->GetState());

		// Execute
		m_pPlayerObject->CancelMoveLoginServer(CR_FAIL);

		// Assert
		CHECK_EQUAL(POS_INWORLD, m_pPlayerObject->GetState());

		CHECK_EQUAL(0, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());
	}

	TEST_FIXTURE(Fixture, TestMoveLoginServer_PlayerObject_Method_GoMoveLoginServer_Success)
	{
		// Arrange
		m_pPlayerObject->OnMoveLoginServer();
		CHECK_EQUAL(POS_MOVE_LOGIN_SERVER, m_pPlayerObject->GetState());

		// Execute
		m_pPlayerObject->GoMoveLoginServer(CR_SUCCESS);

		// Assert
		CHECK_EQUAL(1, m_pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_COMM_START_MOVE_TO_LOGIN_SERVER_RES, m_pPlayerLink->GetCommandID(0));
		CHECK_EQUAL(CR_SUCCESS, m_pPlayerLink->GetParam<int>(0, 0));
	}
	
}
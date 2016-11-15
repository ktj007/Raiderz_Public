#include "stdafx.h"
#include "GUTHelper.h"
#include "FForCommandTest.h"
#include "GServerGateRouter.h"
#include "GPlayerGameData.h"
#include "GGateInfo.h"
#include "CCommandResultTable.h"
#include "GUnitTestUtil.h"

SUITE(MoveGameServer)
{
	struct MockGatePlayerObject: public MockPlayerObject
	{
		enum MockGateState	{ NONE, READY, GETSET, GO, RETRY}	m_mockGateState;
		RESPONSE_GATE_TYPE m_responseGateType;
		GGateInfo m_gateInfo;

		MockGatePlayerObject(MUID& uid):MockPlayerObject(uid), m_mockGateState(NONE)	{}
		
		virtual void ReadyMoveGameServer(int nMoveTargetGameServerID, GGateInfo& refGateInfo) override
		{
			m_mockGateState = READY;
		}

		virtual void GetSetMoveGameServer(TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo) override
		{
			m_mockGateState = GETSET;
		}

		virtual void Gate(RESPONSE_GATE_TYPE responseGateType, const GGateInfo* pGateInfo) override
		{
			m_mockGateState = RETRY;
			m_responseGateType = responseGateType;
			m_gateInfo = *pGateInfo;	// shallow copy
		}
	};

	struct Fixture: public FForCommandTest
	{
		MUID m_uidRequestPlayer;
		MUID m_uidMockGatePlayer;
		GEntityPlayer* m_pEntityPlayerDummy;
		MockGatePlayerObject* m_pMockGatePlayerObject;
		
		Fixture(): m_pEntityPlayerDummy(NULL)
		{
			m_uidRequestPlayer = m_pPlayer->GetUID();
			InitMockGatePlayerObject();
		}
		
		~Fixture()
		{
		}

		void InitMockGatePlayerObject()
		{
			m_uidMockGatePlayer = SUnitTestUtil::NewUID();
			
			m_pMockGatePlayerObject = new MockGatePlayerObject(m_uidMockGatePlayer);
			gmgr.pPlayerObjectManager->AddPlayer(m_pMockGatePlayerObject);

			m_pEntityPlayerDummy = new GEntityPlayer;			
			m_pMockGatePlayerObject->Create(m_pEntityPlayerDummy);
		}

		TD_LOGIN_GAME_SERVER_INFO MakeLoginGameServerInfo(wstring strHostName, int nPort, MUID uidConnectionKey)
		{
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo;
			wcsncpy_s(LoginGameServerInfo.strHostName, strHostName.c_str(), _TRUNCATE);
			LoginGameServerInfo.nPort = nPort;
			LoginGameServerInfo.uidConnectionKey = uidConnectionKey;

			return LoginGameServerInfo;
		}

		TD_GATE_INFO MakeGateInfoSharedField(int nFieldID, int nChannelID)
		{
			vec3 vPos(100, 200, 300);
			vec3 vDir(0, 1, 0);

			TD_GATE_INFO GateInfo;
			GateInfo.nFieldID = nFieldID;
			GateInfo.vPosition = vPos;
			GateInfo.vDirection = vDir;
			GateInfo.bDynamicField = false;
			GateInfo.nChannelID = nChannelID;
			GateInfo.uidDynamicFieldGroup = MUID::ZERO;

			return GateInfo;
		}

		MCommandResult RecvCommand_ResponseReserveMoveGameServer(MUID uidRequestPlayer, int nResult, const TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo)
		{
			int nCount = 1;
			if (nResult != CR_SUCCESS || pLoginGameServerInfo == NULL)
			{
				nCount = 0;
			}

			return m_pNetAgent->OnRecv(MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER,
										3,
										NEW_UID(uidRequestPlayer),
										NEW_INT(nResult),
										NEW_BLOB(pLoginGameServerInfo, sizeof(TD_LOGIN_GAME_SERVER_INFO), nCount));
		}
		
		MCommandResult RecvCommand_GateMoveServer(MUID uidRequestPlayer, int nMoveTargetServerID, TD_GATE_INFO GateInfo)
		{
			return m_pNetAgent->OnRecv(MMC_GATE_MOVE_SERVER,
										3,
										NEW_UID(uidRequestPlayer),
										NEW_INT(nMoveTargetServerID),
										NEW_SINGLE_BLOB(&GateInfo, sizeof(GateInfo)));
		}

		MCommandResult RecvCommand_MMC_GATE_RES(MUID uidRequestPlayer, int nResponseGateType, TD_GATE_INFO GateInfo)
		{
			return m_pNetAgent->OnRecv(MMC_GATE_RES,
										3,
										NEW_UID(uidRequestPlayer),
										NEW_INT(nResponseGateType),
										NEW_BLOB(&GateInfo, sizeof(GateInfo), 1));
		}
	};

	TEST_FIXTURE(Fixture, TestCommandRouter_RequestReserveMoveGameServer) 
	{
		int nMoveTargetServerID = 999;
		GPlayerGameData gameData;
		GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
		pServerRouter->RequestReserveMoveGameServer(m_uidRequestPlayer, nMoveTargetServerID, &gameData);

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<int>(0, 0), nMoveTargetServerID);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 1), m_uidRequestPlayer);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MBlob*>(0, 2)->GetBlobCount(), 1);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MBlob*>(0, 3)->GetBlobCount(), 1);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MBlob*>(0, 4)->GetBlobCount(), 1);
		// 5번 param이하 vector<T>, BlobCount가 0 또는 1
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_GateAskSharedField)
	{		
		GGateInfo gateInfo;
		gateInfo.nFieldID = 100;
		gateInfo.vPosition = vec3(100, 200, 300);
		gateInfo.vDirection = vec3(0, 1, 0);
		gateInfo.bDynamicField = false;	
		gateInfo.nChannelID = 2;
		gateInfo.uidDynamicFieldGroup = SUnitTestUtil::NewUID();

		GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
		pServerRouter->AskGateSharedField(m_uidRequestPlayer, &gateInfo);

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GATE_ASK_SHARED_FIELD);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidRequestPlayer);
		
		TD_GATE_INFO* pInfo;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetSingleBlob(pInfo, 1));
		CHECK_EQUAL(pInfo->nFieldID, gateInfo.nFieldID);
		CHECK_EQUAL(pInfo->vPosition, gateInfo.vPosition);
		CHECK_EQUAL(pInfo->vDirection, gateInfo.vDirection);
		CHECK_EQUAL(pInfo->bDynamicField, gateInfo.bDynamicField);
		CHECK_EQUAL(pInfo->nChannelID, gateInfo.nChannelID);
		CHECK_EQUAL(pInfo->uidDynamicFieldGroup, gateInfo.uidDynamicFieldGroup);
	}

	TEST_FIXTURE(Fixture, TestCommandRouter_GateAskFieldGroup)
	{
		GGateInfo gateInfo;
		gateInfo.nFieldID = 100;
		gateInfo.vPosition = vec3(100, 200, 300);
		gateInfo.vDirection = vec3(0, 1, 0);
		gateInfo.bDynamicField = true;
		gateInfo.uidDynamicFieldGroup = SUnitTestUtil::NewUID();

		GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
		pServerRouter->AskGateDynamicField(m_uidRequestPlayer, &gateInfo, 1, DFT_TRIAL, false);

		ASSERT_EQUAL(m_pNetAgent->GetSendCommandID(0), MMC_GATE_ASK_DYNAMIC_FIELD);
		CHECK_EQUAL(m_pNetAgent->GetSendParam<MUID>(0, 0), m_uidRequestPlayer);

		TD_GATE_FIELD_GROUP_INFO* pGateFieldGroupInfo;
		ASSERT_CHECK(m_pNetAgent->GetSendCommand(0).GetSingleBlob(pGateFieldGroupInfo, 1));
		CHECK_EQUAL(pGateFieldGroupInfo->GateInfo.nFieldID, gateInfo.nFieldID);
		CHECK_EQUAL(pGateFieldGroupInfo->GateInfo.vPosition, gateInfo.vPosition);
		CHECK_EQUAL(pGateFieldGroupInfo->GateInfo.vDirection, gateInfo.vDirection);
		CHECK_EQUAL(pGateFieldGroupInfo->GateInfo.bDynamicField, gateInfo.bDynamicField);
		CHECK_EQUAL(pGateFieldGroupInfo->GateInfo.uidDynamicFieldGroup, gateInfo.uidDynamicFieldGroup);
		CHECK_EQUAL(pGateFieldGroupInfo->nFieldGroupID, 1);
		CHECK_EQUAL(pGateFieldGroupInfo->nType, (int8)DFT_TRIAL);
		CHECK_EQUAL(pGateFieldGroupInfo->isSingleType, false);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_ResponseReserveMoveGameServer) 
	{
		wstring strHostName = GUnitTestUtil::NewString();
		int nPort = 200;
		MUID uidConnectionKey = SUnitTestUtil::NewUID();		
		TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, nPort, uidConnectionKey);
		MCommandResult nResult = RecvCommand_ResponseReserveMoveGameServer(m_uidMockGatePlayer, CR_SUCCESS, &LoginGameServerInfo);

		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_mockGateState, MockGatePlayerObject::GETSET);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_ResponseReserveMoveGameServer_Failure)
	{
		wstring strHostName = GUnitTestUtil::NewString();
		int nPort = 200;
		MUID uidConnectionKey = SUnitTestUtil::NewUID();
		TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, nPort, uidConnectionKey);
		MCommandResult nResult = RecvCommand_ResponseReserveMoveGameServer(m_uidMockGatePlayer, CR_FAIL, &LoginGameServerInfo);

		CHECK_EQUAL(m_pMockGatePlayerObject->m_mockGateState, MockGatePlayerObject::NONE);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_ResponseReserveMoveGameServer_NoExistParameter)
	{
		MCommandResult nResult = RecvCommand_ResponseReserveMoveGameServer(m_uidMockGatePlayer, CR_SUCCESS, NULL);

		CHECK_EQUAL(m_pMockGatePlayerObject->m_mockGateState, MockGatePlayerObject::NONE);
	}
	
	TEST_FIXTURE(Fixture, TestCommandHandler_GateMoveServer)
	{
		int nMoveTargetServerID = 999;
		int nFieldID = 100;
		int nChannelID = 2;
		TD_GATE_INFO GateInfo = MakeGateInfoSharedField(nFieldID, nChannelID);
		MCommandResult nResult = RecvCommand_GateMoveServer(m_uidMockGatePlayer, nMoveTargetServerID, GateInfo);

		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_mockGateState, MockGatePlayerObject::READY);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_GateRetry) 
	{
		int nResponseGateType = RGT_EXECUTE;
		int nFieldID = 100;
		int nChannelID = 2;
		TD_GATE_INFO GateInfo = MakeGateInfoSharedField(nFieldID, nChannelID);
		MCommandResult nResult = RecvCommand_MMC_GATE_RES(m_uidMockGatePlayer, nResponseGateType, GateInfo);

		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_mockGateState, MockGatePlayerObject::RETRY);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_responseGateType, nResponseGateType);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_gateInfo.nFieldID, nFieldID);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_gateInfo.vPosition, GateInfo.vPosition);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_gateInfo.vDirection, GateInfo.vDirection);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_gateInfo.bDynamicField, false);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_gateInfo.nChannelID, nChannelID);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_gateInfo.uidDynamicFieldGroup, MUID::ZERO);


		// nResponseGateType 삼각측량
		nResponseGateType = RGT_REMAIN;
		RecvCommand_MMC_GATE_RES(m_uidMockGatePlayer, nResponseGateType, GateInfo);
		CHECK_EQUAL(m_pMockGatePlayerObject->m_responseGateType, nResponseGateType);
	}

}

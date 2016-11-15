#include "stdafx.h"
#include "ZGameServerInfo.h"
#include "ZGameServerInfoLoader.h"
#include "ZGameServerInfoManager.h"
#include "CCommandTable_Master.h"
#include "ZFixtureHelper.h"
#include "CCommandResultTable.h"
#include "ZServer.h"
#include "CTransData.h"
#include "STransData.h"
#include "STransData_M2G.h"
#include "ZGateCommandRouter.h"
#include "ZCommandCenter.h"
#include "ZGameServerObject.h"
#include "SCommandTable.h"
#include "ZTestHelper_CmdHandler.h"


#define PACKING_GAME_DATA_PARAM_COUNT		15

SUITE(MoveGameServer)
{	
	struct ServerStub: public MockServer
	{
		MUID m_uidReserverKeySub;

		virtual MUID NewReserverKey(void)					{ return m_uidReserverKeySub;				}
		void SetReserverKeyStub(MUID uidReserverKeyStub)	{ m_uidReserverKeySub = uidReserverKeyStub;	}		
	};

	struct Fixture: public FBaseServerField, public FBasePlayer
	{
		int m_nFromFieldID;
		int m_nToFieldID;
		MUID m_uidFromServer;
		MUID m_uidToServer;
		int m_nFromServerID;
		int m_nToServerID;
		MUID m_uidRequestPlayer;
		MockLink* m_pLinkToServer;
		MockLink* m_pLinkFromServer;
		ZTestSysWrapper2<ZServer, ServerStub>	m_ServerStub;

		Fixture()
		{
			m_nFromFieldID = 100;
			m_nToFieldID = 101;

			AddFieldInfo(m_nFromFieldID, L"FieldOfFromServer", L"", false, 2);
			AddFieldInfo(m_nToFieldID, L"FieldOfToServer", L"", false, 2);
			
			m_nFromServerID = 1;
			m_nToServerID = 2;

			m_uidFromServer = AddGameServerObject(m_nFromServerID, m_nFromFieldID)->GetUID();
			m_pLinkFromServer = ZTestHelper::NewLink(m_uidFromServer);
			m_pLinkFromServer->AddIgnoreID(MSC_PROXY_PLAYER_UPDATE);
			m_pLinkFromServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);

			m_uidToServer = AddGameServerObject(m_nToServerID, m_nToFieldID)->GetUID();			
			m_pLinkToServer = ZTestHelper::NewLink(m_uidToServer);
			m_pLinkToServer->AddIgnoreID(MSC_PROXY_PLAYER_UPDATE);
			m_pLinkToServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);

			m_uidRequestPlayer = ZTestHelper::NewUID();

			int nGameServerID = 1;
			int nAID = ZTestHelper::NewAID();
			wstring strUserID = ZTestHelper::NewName();
			int nCID = ZTestHelper::NewCID();
			ZPlayer* pPlayer = AddNewPlayerInWorld(nGameServerID, nAID, strUserID, nCID, m_uidRequestPlayer);
			pPlayer->SetFieldID(m_nFromFieldID + 1);
		}
		~Fixture()
		{
			ZTestHelper::ClearLinks();
		}

		vector<vector<int> > MakeDummyRelayData(int nDummyCount)
		{
			vector<vector<int> > vecDummyData;

			for(int i = 0; i < nDummyCount; i++)
			{
				vector<int> vecInt;
				vecInt.push_back(i);
				
				vecDummyData.push_back(vecInt);
			}

			return vecDummyData;
		}

		TD_LOGIN_GAME_SERVER_INFO MakeLoginGameServerInfo(wstring strHostName, int port, MUID uidConnectionKey)
		{
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo;
			wcsncpy_s(LoginGameServerInfo.strHostName, strHostName.c_str(), _TRUNCATE);
			LoginGameServerInfo.nPort = port;
			LoginGameServerInfo.uidConnectionKey = uidConnectionKey;
			
			return LoginGameServerInfo;
		}

		MCommandResult RecvCommand_RequestReserverMoveGameServer(int nToServer, MUID uidRequestPlayer, vector<vector<int> > vecRelayData)
		{		
			MCommand command;
			command.SetID(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER);
			command.SetReceiverUID(m_uidFromServer);
			command.SetSenderUID(m_pLinkFromServer->GetUID());

			command.AddParameter(NEW_INT(nToServer));
			command.AddParameter(NEW_UID(uidRequestPlayer));

			for(int i = 0; i < PACKING_GAME_DATA_PARAM_COUNT; i++)
			{
				command.AddParameter(NEW_BLOB(vecRelayData[i]));
			}
			
			return m_pLinkFromServer->OnRecvCommand(command);
		}

		MCommandResult RecvCommand_ResponseReadyMovePlayer_SuccessResult(MUID uidFromServer, MUID uidRequestPlayer, TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo)
		{
			return m_pLinkToServer->OnRecv(MMC_COMM_RESPONSE_READY_ENTER_MOVE_PLAYER, 
											4, 
											NEW_UID(uidFromServer), 
											NEW_UID(uidRequestPlayer), 
											NEW_INT(CR_SUCCESS), 
											NEW_BLOB(&LoginGameServerInfo, sizeof(LoginGameServerInfo), 1));
		}

		MCommandResult RecvCommand_ResponseReadyMovePlayer_FailureResult(MUID uidFromServer, MUID uidRequestPlayer)
		{
			return m_pLinkToServer->OnRecv(MMC_COMM_RESPONSE_READY_ENTER_MOVE_PLAYER, 
											4, 
											NEW_UID(uidFromServer), 
											NEW_UID(uidRequestPlayer), 
											NEW_INT(CR_FAIL), 
											NEW_BLOB());
		}

	};

	TEST(MoveGameServer_ConnectionKey)
	{
		set<MUID> setConnectionKey;
		for (int i = 0; i < 1000; ++i)
		{
			MUID uidConnectionKey = gsys.pServer->NewReserverKey();
			setConnectionKey.insert(uidConnectionKey);
			
			ASSERT_EQUAL(i+1, setConnectionKey.size());
		}
	}

	// 서버이동 예약 커멘드를 올바른 서버에 전달해야한다.
	TEST_FIXTURE(Fixture, TestCommandHandler_RequestReserverMoveGameServer)
	{
		MUID uidReserverKey = ZTestHelper::NewUID();
		m_ServerStub.Get()->SetReserverKeyStub(uidReserverKey);

		vector<vector<int>> vecRelayData = MakeDummyRelayData(PACKING_GAME_DATA_PARAM_COUNT);
		MCommandResult nResult = RecvCommand_RequestReserverMoveGameServer(m_nToServerID, m_uidRequestPlayer, vecRelayData);


		CHECK_EQUAL(nResult, CR_TRUE);
		ASSERT_EQUAL(m_pLinkToServer->GetCommandID(0), MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER);
		CHECK_EQUAL(m_pLinkToServer->GetParam<MUID>(0, 0), uidReserverKey);
		CHECK_EQUAL(m_pLinkToServer->GetParam<MUID>(0, 1), m_uidFromServer);
		CHECK_EQUAL(m_pLinkToServer->GetParam<MUID>(0, 2), m_uidRequestPlayer);

		for(int i = 0; i < PACKING_GAME_DATA_PARAM_COUNT; i++)
		{			
			vector<int> vecData;
			ASSERT_CHECK(m_pLinkToServer->GetCommand(0).GetBlob(vecData, i + 3));			
			CHECK(equal(vecRelayData[i].begin(), vecRelayData[i].end(), vecData.begin()));
		}
	}

	// 서버이동 예약 목표 서버가 존재하지 않는 경우, 실패 커멘드를 반환해야한다.
	TEST_FIXTURE(Fixture, TestCommandHandler_RequestReserverMoveGameServer_UnknownTargetServerFailure)
	{
		int nUnknownServerID = 9999;
		MCommandResult nResult = RecvCommand_RequestReserverMoveGameServer(nUnknownServerID, m_uidRequestPlayer, MakeDummyRelayData(PACKING_GAME_DATA_PARAM_COUNT));
		
		CHECK_EQUAL(nResult, CR_TRUE);		
		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), CR_FAIL);		
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 0);
	}

	// 서버이동 예약성공 응답 커멘드를 요청 서버에 전달해야한다.
	TEST_FIXTURE(Fixture, TestCommandHandler_ResponseReadyMovePlayer_SuccessResult)
	{
		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(m_uidRequestPlayer);
		pPlayer->SetState(PS_READY_MOVE_GAMESERVER);

		wstring strHostName = ZTestHelper::NewString();
		int port = 200;
		MUID uidConnectionKey = ZTestHelper::NewUID();
		TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);
		MCommandResult nResult = RecvCommand_ResponseReadyMovePlayer_SuccessResult(m_uidFromServer, m_uidRequestPlayer, LoginGameServerInfo);


		CHECK_EQUAL(nResult, CR_TRUE);
		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), CR_SUCCESS);		
		
		TD_LOGIN_GAME_SERVER_INFO* pInfo;
		int nCount;
		ASSERT_CHECK(m_pLinkFromServer->GetCommand(0).GetBlob(pInfo, nCount, 2));
		CHECK(!wcsncmp(pInfo->strHostName, LoginGameServerInfo.strHostName, MAX_HOSTNAME_LEN));
		CHECK_EQUAL(pInfo->nPort, LoginGameServerInfo.nPort);
		CHECK_EQUAL(pInfo->uidConnectionKey, LoginGameServerInfo.uidConnectionKey);
	}

	// 서버이동 예약실패 응답 커멘드를 요청 서버에 전달해야한다.
	TEST_FIXTURE(Fixture, TestCommandHandler_ResponseReadyMovePlayer_FailureResult)
	{
		int ip = 100;
		int port = 200;
		MUID uidConnectionKey = ZTestHelper::NewUID();
		MCommandResult nResult = RecvCommand_ResponseReadyMovePlayer_FailureResult(m_uidFromServer, m_uidRequestPlayer);

		CHECK_EQUAL(nResult, CR_TRUE);
		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), CR_FAIL);		
		CHECK(m_pLinkFromServer->GetParam<MBlob*>(0, 2) != NULL);
	}

	// 서버이동 예약 응답 커멘드를 받았으나, 요청 서버가 존제하지 않는다면 처리하지 말아야한다.
	TEST_FIXTURE(Fixture, TestCommandHandler_ResponseReadyMovePlayer_UnknownTargetServerFailure)
	{
		wstring strHostName = ZTestHelper::NewString();
		int port = 200;
		MUID uidConnectionKey = ZTestHelper::NewUID();
		MUID uidUnkownServer = ZTestHelper::NewUID();
		TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);
		MCommandResult nResult = RecvCommand_ResponseReadyMovePlayer_SuccessResult(uidUnkownServer, m_uidRequestPlayer, LoginGameServerInfo);

		CHECK_EQUAL(nResult, CR_TRUE);
		CHECK_EQUAL(m_pLinkFromServer->GetCommandID(0), 0);
	}

	// 필드이동 질의 커멘드를 받으면, 해당 필드를 가지고 있는 서버를 찾아 응답 커멘드를 보내줘야한다.
	TEST_FIXTURE(Fixture, TestCommandHandler_GateAsk)
	{
		int nChannelID = 1;
		TD_GATE_INFO GateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nToFieldID, nChannelID);
		MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkFromServer, m_uidRequestPlayer, GateInfo);
		
		CHECK_EQUAL(nResult, CR_TRUE);
		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_GATE_MOVE_SERVER);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), m_nToServerID);
	}

	// 필드이동 질의 커멘드를 받으면, 자신의 필드거나, 자신에게 필드 생성이 되었다면, Gate 응답 커멘드를 보내줘야한다.
	TEST_FIXTURE(Fixture, TestCommandHandler_GateAsk_Response_EXECUTE)
	{
		int nChannelID = 1;
		TD_GATE_INFO GateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFromFieldID, nChannelID);
		MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkFromServer, m_uidRequestPlayer, GateInfo);

		CHECK_EQUAL(nResult, CR_TRUE);
		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_GATE_RES);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), RGT_EXECUTE);
		
		TD_GATE_INFO* pInfo;
		int nCount;
		ASSERT_EQUAL(m_pLinkFromServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 1);
		ASSERT_CHECK(m_pLinkFromServer->GetCommand(0).GetBlob(pInfo, nCount, 2));
		CHECK_EQUAL(pInfo->nFieldID, m_nFromFieldID);
		CHECK_EQUAL(pInfo->nChannelID, nChannelID);
		CHECK_EQUAL(pInfo->bDynamicField, false);
		CHECK_EQUAL(pInfo->uidDynamicFieldGroup, MUID::ZERO);
	}

	// 필드이동 질의 커멘드를 받으면, 해당 필드가 존재하지 않을때 실패 응답.
	TEST_FIXTURE(Fixture, TestCommandHandler_GateAsk_UnkownFieldID)
	{
		int nUnkownFieldID = 300;
		int nChannelID = 1;
		TD_GATE_INFO GateInfo = ZTestHelper::MakeGateInfo_SharedField(nUnkownFieldID, nChannelID);
		MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MMC_GATE_ASK(m_pLinkFromServer, m_uidRequestPlayer, GateInfo);

		CHECK_EQUAL(nResult, CR_TRUE);
		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_GATE_RES);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), RGT_FAIL);
	}

	// 서버이동 예약 성공응답 커멘드 보내기 확인
	TEST_FIXTURE(Fixture, TestCommandRouter_ResponseReserverMoveGameServer)
	{
		wstring strHostName = ZTestHelper::NewString();
		int port = 200;
		MUID uidConnectionKey = ZTestHelper::NewUID();
		int nResult = CR_SUCCESS;
		TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);
		ZGateCommandRouter::RouteResponseReserveMoveGameServer(m_uidFromServer, m_uidRequestPlayer, nResult, &LoginGameServerInfo);

		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), nResult);		

		TD_LOGIN_GAME_SERVER_INFO* pInfo;
		int nCount;		
		ASSERT_EQUAL(m_pLinkFromServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 1);
		ASSERT_CHECK(m_pLinkFromServer->GetCommand(0).GetBlob(pInfo, nCount, 2));	
		CHECK(!wcsncmp(pInfo->strHostName, strHostName.c_str(), MAX_HOSTNAME_LEN));
		CHECK_EQUAL(pInfo->nPort, 200);
		CHECK_EQUAL(pInfo->uidConnectionKey, uidConnectionKey);
	}

	// 서버이동 예약 실패응답 커멘드 보내기 확인
	TEST_FIXTURE(Fixture, TestCommandRouter_ResponseReserverMoveGameServer_Failure)
	{
		wstring strHostName = ZTestHelper::NewString();
		int port = 200;
		MUID uidConnectionKey = ZTestHelper::NewUID();
		int nResult = CR_FAIL;
		TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);
		ZGateCommandRouter::RouteResponseReserveMoveGameServer(m_uidFromServer, m_uidRequestPlayer, nResult, &LoginGameServerInfo);

		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_COMM_RESPONSE_RESERVE_MOVE_GAME_SERVER);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), nResult);			
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 0);
	}

	// 서버이동 준비 요청 커멘드 보내기 확인
	TEST_FIXTURE(Fixture, TestCommandRouter_RequestReadyEnterMovePlayer)
	{		
		MUID uidReserverKey = ZTestHelper::NewUID();
		m_ServerStub.Get()->SetReserverKeyStub(uidReserverKey);

		auto_ptr<MCommand> pCommand(gsys.pCommandCenter->NewCommand(MMC_COMM_REQUEST_RESERVE_MOVE_GAME_SERVER));
		pCommand->AddParameter(NEW_INT(m_nToServerID));
		pCommand->AddParameter(NEW_UID(m_uidRequestPlayer));
		
		vector<vector<int>> vecRelayData = MakeDummyRelayData(PACKING_GAME_DATA_PARAM_COUNT);
		for(int i = 0; i < PACKING_GAME_DATA_PARAM_COUNT; i++)
		{
			pCommand->AddParameter(NEW_BLOB(vecRelayData[i]));
		}

		MUID uidConnectionKeyToSet = gsys.pServer->NewReserverKey();
		ZGateCommandRouter::RouteRequestReadyEnterMovePlayer(m_uidToServer, uidConnectionKeyToSet, m_uidRequestPlayer, m_uidFromServer, pCommand.get());
		

		ASSERT_EQUAL(m_pLinkToServer->GetCommandID(0), MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER);
		CHECK_EQUAL(m_pLinkToServer->GetParam<MUID>(0, 0), uidReserverKey);
		CHECK_EQUAL(m_pLinkToServer->GetParam<MUID>(0, 1), m_uidFromServer);
		CHECK_EQUAL(m_pLinkToServer->GetParam<MUID>(0, 2), m_uidRequestPlayer);

		for(int i = 0; i < PACKING_GAME_DATA_PARAM_COUNT; i++)
		{			
			vector<int> vecData;
			ASSERT_EQUAL(m_pLinkToServer->GetParam<MBlob*>(0, i + 3)->GetBlobCount(), 1);
			ASSERT_CHECK(m_pLinkToServer->GetCommand(0).GetBlob(vecData, i + 3));			
			CHECK(equal(vecRelayData[i].begin(), vecRelayData[i].end(), vecData.begin()));
		}
	}

	// 서버이동 지시 커멘드 보내기 확인
	TEST_FIXTURE(Fixture, TestCommandRouter_GateMoveServer)
	{
		int nChannelID = 1;
		TD_GATE_INFO tdGateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFromFieldID, nChannelID);

		ZGateCommandRouter::RouteGateMoveServer(m_uidFromServer, m_uidRequestPlayer, m_nToServerID, &tdGateInfo);

		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_GATE_MOVE_SERVER);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), m_nToServerID);

		TD_GATE_INFO* pInfo;
		CHECK_EQUAL(true, m_pLinkFromServer->GetCommand(0).GetSingleBlob(pInfo, 2));
		CHECK_EQUAL(pInfo->nFieldID, m_nFromFieldID);
		CHECK_EQUAL(pInfo->nChannelID, nChannelID);
	}

	// 서버이동 재시도 커멘드 보내기 확인
	TEST_FIXTURE(Fixture, TestCommandRouter_GateResponse_EXECUTE)
	{
		RESPONSE_GATE_TYPE nResponseGateType = RGT_EXECUTE;
		int nChannelID = 1;
		TD_GATE_INFO GateInfo = ZTestHelper::MakeGateInfo_SharedField(m_nFromFieldID, nChannelID);
		ZGateCommandRouter::RouteGateRes(m_uidFromServer, m_uidRequestPlayer, nResponseGateType, &GateInfo);

		ASSERT_EQUAL(m_pLinkFromServer->GetCommandID(0), MMC_GATE_RES);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<MUID>(0, 0), m_uidRequestPlayer);
		CHECK_EQUAL(m_pLinkFromServer->GetParam<int>(0, 1), nResponseGateType);
		
		TD_GATE_INFO* pInfo;
		int nCount;
		ASSERT_EQUAL(m_pLinkFromServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 1);
		ASSERT_CHECK(m_pLinkFromServer->GetCommand(0).GetBlob(pInfo, nCount, 2));
		CHECK_EQUAL(pInfo->nFieldID, m_nFromFieldID);
		CHECK_EQUAL(pInfo->nChannelID, nChannelID);
		CHECK_EQUAL(pInfo->bDynamicField, false);
		CHECK_EQUAL(pInfo->uidDynamicFieldGroup, MUID::ZERO);
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_CancelMoveGameServer_Success_State_READY_MOVE_GAMESERVER)
	{
		ZGameServerObject* pFromGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_nFromServerID);
		CHECK_EQUAL(1, pFromGameServer->GetPlayerMap().GetObjectCount());
		
		MUID uidConnectionKey = ZTestHelper::NewUID();
		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(m_uidRequestPlayer);
		pPlayer->SetState(PS_READY_MOVE_GAMESERVER);
		pPlayer->SetReservedConnectionKey(uidConnectionKey);
		pPlayer->SetReservedGameServerID(m_nToServerID);

		m_pLinkFromServer->OnRecv(MMC_COMM_CANCEL_MOVE_GAME_SERVER, 1, NEW_UID(m_uidRequestPlayer));

		CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());
		CHECK_EQUAL(1, pFromGameServer->GetPlayerMap().GetObjectCount());

		ASSERT_EQUAL(1, m_pLinkToServer->GetCommandCount());
		CHECK_EQUAL(MMC_COMM_CANCEL_RESERVED_MOVE_PLAYER, m_pLinkToServer->GetCommandID(0));
		CHECK_EQUAL(uidConnectionKey, m_pLinkToServer->GetParam<MUID>(0, 0));
	}

	TEST_FIXTURE(Fixture, TestCommandHandler_CancelMoveGameServer_Success_State_MOVE_GAMESERVER)
	{
		MUID uidConnectionKey = ZTestHelper::NewUID();

		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(m_uidRequestPlayer);
		pPlayer->SetState(PS_MOVE_GAMESERVER);
		pPlayer->SetReservedConnectionKey(uidConnectionKey);
		pPlayer->SetReservedGameServerID(m_nToServerID);

		ZGameServerObject* pFromGameServer = gmgr.pGameServerObjectManager->FindGameServer(m_nFromServerID);
		pFromGameServer->GetPlayerMap().Delete(pPlayer->GetAID());

		CHECK_EQUAL(0, pFromGameServer->GetPlayerMap().GetObjectCount());

		m_pLinkFromServer->OnRecv(MMC_COMM_CANCEL_MOVE_GAME_SERVER, 1, NEW_UID(m_uidRequestPlayer));

		CHECK_EQUAL(PS_IN_WORLD, pPlayer->GetState());
		CHECK_EQUAL(1, pFromGameServer->GetPlayerMap().GetObjectCount());

		ASSERT_EQUAL(1, m_pLinkToServer->GetCommandCount());
		CHECK_EQUAL(MMC_COMM_CANCEL_RESERVED_MOVE_PLAYER, m_pLinkToServer->GetCommandID(0));
		CHECK_EQUAL(uidConnectionKey, m_pLinkToServer->GetParam<MUID>(0, 0));
	}

}

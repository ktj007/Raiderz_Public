#include "stdafx.h"
#include "ZTestWrapper.h"
#include "ZTestHelper.h"
#include "ZFixtureHelper.h"
#include "MockLink.h"
#include "STransData.h"
#include "CTransData.h"
#include "ZCommandTable.h"
#include "ZGameServerObject.h"
#include "ZLoginCommandRouter.h"
#include "ZTestHelper_CmdHandler.h"
#include "ZSharedField.h"


SUITE(LoginGameServer)
{
	struct ServerStub: public MockServer
	{
		MUID m_uidReserverKeySub;

		virtual MUID NewReserverKey(void)					{ return m_uidReserverKeySub;				}
		void SetReserverKeyStub(MUID uidReserverKeyStub)	{ m_uidReserverKeySub = uidReserverKeyStub;	}		
	};

	struct Fixture: public FBaseServerField, public FBaseLoginServer, public FBasePlayer
	{
		MUID m_uidLoginServer;
		MUID m_uidGameServer;
		MUID m_uidGameServer2;
		MockLink* m_pLinkLoginServer;		
		MockLink* m_pLinkGameServer;
		MockLink* m_pLinkGameServer2;
		ZPlayer* m_pPlayer;
		int m_nFieldID10;
		int m_nFieldID20;
		ZTestSysWrapper2<ZServer, ServerStub>	m_ServerStub;

		Fixture()
		{
			int nGameServerID = 1;
			int nGameServerID2 = 2;
			int nLoginServerID = 10;

			m_nFieldID10 = 10;
			m_nFieldID20 = 20;
			InitDefaultGameServer(nGameServerID, m_nFieldID10, 1);
			InitDefaultGameServer(nGameServerID2, m_nFieldID20, 1);
			ZGameServerObject* pGameServerObject = gmgr.pGameServerObjectManager->FindGameServer(nGameServerID);
			ZGameServerObject* pGameServerObject2 = gmgr.pGameServerObjectManager->FindGameServer(nGameServerID2);

			ZFieldInfo* pFirstFieldInfo = AddFieldInfo(FIRST_ENTER_FIELD_ID);
			AddSharedField(pFirstFieldInfo, 1, nGameServerID);
			
			m_uidLoginServer = Connected(nLoginServerID);
			m_uidGameServer = pGameServerObject->GetUID();
			m_uidGameServer2 = pGameServerObject2->GetUID();
			
			m_pLinkLoginServer = ZTestHelper::NewLink(m_uidLoginServer);
			m_pLinkGameServer = ZTestHelper::NewLink(m_uidGameServer);
			m_pLinkGameServer->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);
			m_pLinkGameServer2  = ZTestHelper::NewLink(m_uidGameServer2);
			m_pLinkGameServer2->AddIgnoreID(MMC_FIELD_UPDATE_PROXY_FIELD_INFO);

			m_pPlayer = gmgr.pPlayerManager->NewPlayer(ZTestHelper::NewUID(), ZTestHelper::NewAID(), ZTestHelper::NewName());
			gmgr.pPlayerManager->Add(m_pPlayer);
		}

		~Fixture()
		{
			ZTestHelper::ClearLinks();
		}

		TD_LOGIN_GAME_SERVER_INFO MakeLoginGameServerInfo(wstring strHostName, int port, MUID uidConnectionKey)
		{
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo;
			wcsncpy_s(LoginGameServerInfo.strHostName, strHostName.c_str(), _TRUNCATE);
			LoginGameServerInfo.nPort = port;
			LoginGameServerInfo.uidConnectionKey = uidConnectionKey;
			
			return LoginGameServerInfo;
		}

		TD_LOGIN_FIELD_INFO MakeLoginFieldInfo(int nSharedFieldID, int nDynamicFieldID, MUID uidDynamicFieldGroup)
		{
			TD_LOGIN_FIELD_INFO LoginFieldInfo;
			LoginFieldInfo.nSharedFieldID = nSharedFieldID;
			LoginFieldInfo.nDynamicFieldID = nDynamicFieldID;
			LoginFieldInfo.uidDynamicFieldGroup = uidDynamicFieldGroup;

			return LoginFieldInfo;
		}

		TD_LOGIN_ACCOUNT_INFO MakeLoginAccountInfo(int nEnterKey, ZPlayer* pPlayer)
		{	
			TD_LOGIN_ACCOUNT_INFO LoginAccountInfo;
			LoginAccountInfo.nEnterKey = nEnterKey;
			LoginAccountInfo.nAID = pPlayer->GetAID();
			LoginAccountInfo.nCID = pPlayer->GetCID();
			
			wcsncpy_s(LoginAccountInfo.szUserID, pPlayer->GetPlayerName().c_str(), _TRUNCATE);

			return LoginAccountInfo;
		}

	};

	namespace TestCommandHandler
	{
		// 처리가 성공하면 필드 인원을 갱신하고 플레이어는 해당 필드 예약이 돼야합니다.
		TEST_FIXTURE(Fixture, TestCommandHandler_PlayerReserveDestField)
		{
			/// Fixtuer 설치
			ZSharedField* pField10 = gmgr.pFieldManager->FindSharedField(m_nFieldID10, 1);
			CHECK_EQUAL(0,			pField10->GetPlayerCount());
			CHECK_EQUAL(MUID::ZERO,	m_pPlayer->GetReservedFieldUID());

			MUID uidReserverKey = ZTestHelper::NewUID();
			m_ServerStub.Get()->SetReserverKeyStub(uidReserverKey);

			int nEnterKey = 200;
			TD_LOGIN_ACCOUNT_INFO LoginAccountInfo = MakeLoginAccountInfo(nEnterKey, m_pPlayer);
			TD_LOGIN_FIELD_INFO LoginFieldInfo = MakeLoginFieldInfo(m_nFieldID10, 0, MUID::ZERO);

			/// SUT 실행
			MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MLC_COMM_REQUEST_LOGIN_GAME_SERVER(m_pLinkLoginServer, m_pPlayer->GetUID(), LoginAccountInfo, LoginFieldInfo);

			/// 검증
			CHECK_EQUAL(1,					pField10->GetPlayerCount());
			CHECK_EQUAL(pField10->GetUID(),	m_pPlayer->GetReservedFieldUID());
		}

		// 최초 로그인일 경우
		TEST_FIXTURE(Fixture, TestCommandHandler_FirstLogin)
		{
			/// Fixtuer 설치
			MUID uidReserverKey = ZTestHelper::NewUID();
			m_ServerStub.Get()->SetReserverKeyStub(uidReserverKey);

			int nEnterKey = 200;
			TD_LOGIN_ACCOUNT_INFO LoginAccountInfo = MakeLoginAccountInfo(nEnterKey, m_pPlayer);
			int nFirstLoginFieldID = 0;
			TD_LOGIN_FIELD_INFO LoginFieldInfo = MakeLoginFieldInfo(nFirstLoginFieldID, 0, MUID::ZERO);

			/// SUT 실행
			MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MLC_COMM_REQUEST_LOGIN_GAME_SERVER(m_pLinkLoginServer, m_pPlayer->GetUID(), LoginAccountInfo, LoginFieldInfo);

			/// 검증
			ZField* pFirstEnterField = gmgr.pFieldManager->FindSharedField(FIRST_ENTER_FIELD_ID, PRIME_CHANNELID);
			ASSERT_CHECK(NULL != pFirstEnterField);
			CHECK_EQUAL(1,							pFirstEnterField->GetPlayerCount());
			CHECK_EQUAL(pFirstEnterField->GetUID(),	m_pPlayer->GetReservedFieldUID());
		}

		// 게임서버로 로그인 요청 커멘드를 받으면 해당 필드를 가진 게임서버에게 요청을 전달해야한다.
		TEST_FIXTURE(Fixture, TestCommandHandler_RequestLoginGameServer)
		{
			/// Fixtuer 설치			
			MUID uidReserverKey = ZTestHelper::NewUID();
			m_ServerStub.Get()->SetReserverKeyStub(uidReserverKey);

			int nEnterKey = 200;
			TD_LOGIN_ACCOUNT_INFO LoginAccountInfo = MakeLoginAccountInfo(nEnterKey, m_pPlayer);
			TD_LOGIN_FIELD_INFO LoginFieldInfo = MakeLoginFieldInfo(m_nFieldID10, 0, MUID::ZERO);

			/// SUT 실행
			MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MLC_COMM_REQUEST_LOGIN_GAME_SERVER(m_pLinkLoginServer, m_pPlayer->GetUID(), LoginAccountInfo, LoginFieldInfo);

			/// 검증
			CHECK_EQUAL(nResult, CR_TRUE);
			ASSERT_EQUAL(m_pLinkGameServer->GetCommandID(0), MMC_COMM_REQUEST_LOGIN_GAME_SERVER);
			CHECK_EQUAL(m_pLinkGameServer->GetParam<MUID>(0, 0), m_pLinkLoginServer->GetUID());
			CHECK_EQUAL(m_pLinkGameServer->GetParam<MUID>(0, 1), m_pPlayer->GetUID());
			CHECK_EQUAL(m_pLinkGameServer->GetParam<MUID>(0, 2), uidReserverKey);

			TD_LOGIN_ACCOUNT_INFO* pInfo;
			ASSERT_CHECK(m_pLinkGameServer->GetCommand(0).GetSingleBlob(pInfo, 3));
			CHECK_EQUAL(pInfo->nEnterKey, nEnterKey);
			CHECK_EQUAL(pInfo->nAID, m_pPlayer->GetAID());
			CHECK_EQUAL(pInfo->nCID, m_pPlayer->GetCID());
			
			CHECK(0 == wcscmp(pInfo->szUserID, m_pPlayer->GetPlayerName().c_str()));
		}

		// 게임서버로 로그인 요청 커멘드를 받으면 해당 필드를 가진 게임서버에게 요청을 전달해야한다.
		TEST_FIXTURE(Fixture, TestCommandHandler_RequestLoginGameServer_OrderServer)
		{
			MUID uidReserverKey = ZTestHelper::NewUID();
			m_ServerStub.Get()->SetReserverKeyStub(uidReserverKey);

			int nEnterKey = 200;
			int nDynamicFieldID = 101;
			MUID uidDynamicFieldGroup = MUID::ZERO;
			TD_LOGIN_ACCOUNT_INFO LoginAccountInfo = MakeLoginAccountInfo(nEnterKey, m_pPlayer);
			TD_LOGIN_FIELD_INFO LoginFieldInfo = MakeLoginFieldInfo(m_nFieldID20, nDynamicFieldID, uidDynamicFieldGroup);

			/// SUT 실행
			MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MLC_COMM_REQUEST_LOGIN_GAME_SERVER(m_pLinkLoginServer, m_pPlayer->GetUID(), LoginAccountInfo, LoginFieldInfo);

			/// 검증
			CHECK_EQUAL(nResult, CR_TRUE);
			ASSERT_EQUAL(m_pLinkGameServer2->GetCommandID(0), MMC_COMM_REQUEST_LOGIN_GAME_SERVER);
		}

		// 게임서버가 없을때, 로그인 요청 커멘드가 오면 실패 커멘드를 보내야한다.
		TEST_FIXTURE(Fixture, TestCommandHandler_RequestLoginGameServer_NoGameServer)
		{
			ZTestMgrWrapper<ZGameServerObjectManager>	NoGameServerObjectManager;
			MUID uidReserverKey = ZTestHelper::NewUID();
			m_ServerStub.Get()->SetReserverKeyStub(uidReserverKey);

			int nEnterKey = 200;
			int nDynamicFieldID = 101;
			MUID uidDynamicFieldGroup = ZTestHelper::NewUID();
			TD_LOGIN_ACCOUNT_INFO LoginAccountInfo = MakeLoginAccountInfo(nEnterKey, m_pPlayer);
			TD_LOGIN_FIELD_INFO LoginFieldInfo = MakeLoginFieldInfo(m_nFieldID10, nDynamicFieldID, uidDynamicFieldGroup);

			/// SUT 실행
			MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MLC_COMM_REQUEST_LOGIN_GAME_SERVER(m_pLinkLoginServer, m_pPlayer->GetUID(), LoginAccountInfo, LoginFieldInfo);

			/// 검증
			CHECK_EQUAL(nResult, CR_TRUE);
			ASSERT_EQUAL(m_pLinkLoginServer->GetCommandID(0), MLC_COMM_RESPONSE_LOGIN_GAME_SERVER);
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MUID>(0, 0), m_pPlayer->GetUID());
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<int>(0, 1), CR_FAIL_LOGIN_GAME_SERVER_NOT_FOUND);
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 0);
		}

		// 게임서버로부터 로그인 성공응답 커멘드를 받으면 로그인 서버에게 결과를 전달해야한다.
		TEST_FIXTURE(Fixture, TestCommandHandler_ResponseLoginGameServer)
		{
			wstring strHostName = ZTestHelper::NewString();
			int port = 200;
			MUID uidConnectionKey = ZTestHelper::NewUID();
			int nResultParameter = CR_SUCCESS;
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);

			m_pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);

			/// SUT 실행
			MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MMC_COMM_RESPONSE_LOGIN_GAME_SERVER(m_pLinkGameServer, m_pLinkLoginServer->GetUID(), m_pPlayer->GetUID(), nResultParameter, LoginGameServerInfo);

			/// 검증
			CHECK_EQUAL(nResult, CR_TRUE);
			ASSERT_EQUAL(m_pLinkLoginServer->GetCommandID(0), MLC_COMM_RESPONSE_LOGIN_GAME_SERVER);			
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MUID>(0, 0), m_pPlayer->GetUID());
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<int>(0, 1), nResult);

			TD_LOGIN_GAME_SERVER_INFO* pInfo;
			int nCount;
			ASSERT_CHECK(m_pLinkLoginServer->GetCommand(0).GetBlob(pInfo, nCount, 2));
			CHECK(!wcsncmp(pInfo->strHostName, strHostName.c_str(), MAX_HOSTNAME_LEN));
			CHECK_EQUAL(pInfo->nPort, port);
			CHECK_EQUAL(pInfo->uidConnectionKey, uidConnectionKey);

			CHECK_EQUAL(PS_MOVE_GAMESERVER, m_pPlayer->GetState());
		}

		// 게임서버로부터 로그인 성공응답 커멘드룰 받으면 로그인 서버에게 결과를 전달해야한다.
		TEST_FIXTURE(Fixture, TestCommandHandler_ResponseLoginGameServer_Failure)
		{
			wstring strHostName = ZTestHelper::NewString();
			int port = 200;
			MUID uidConnectionKey = ZTestHelper::NewUID();
			int nResultParameter = CR_FAIL;
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);

			m_pPlayer->SetState(PS_READY_LOGIN_GAMESERVER);

			/// SUT 실행
			MCommandResult nResult = ZTestHelper_CmdHandler::OnRecv_MMC_COMM_RESPONSE_LOGIN_GAME_SERVER(m_pLinkGameServer, m_pLinkLoginServer->GetUID(), m_pPlayer->GetUID(), nResultParameter, LoginGameServerInfo);

			/// 검증
			CHECK_EQUAL(nResult, CR_TRUE);
			ASSERT_EQUAL(m_pLinkLoginServer->GetCommandID(0), MLC_COMM_RESPONSE_LOGIN_GAME_SERVER);
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MUID>(0, 0), m_pPlayer->GetUID());
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<int>(0, 1), CR_FAIL);
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 0);

			CHECK_EQUAL(PS_SELECT_CHAR, m_pPlayer->GetState());
		}
	} ///< namespace TestCommandHandler

	namespace TestCommandRouter
	{
		// 게임서버로 로그인, 성공응답 커멘드 보내기
		TEST_FIXTURE(Fixture, TestCommandRouter_ResponseLoginGameServer)
		{
			wstring strHostName = ZTestHelper::NewString();
			int port = 200;
			MUID uidConnectionKey = ZTestHelper::NewUID();
			int nResult = CR_SUCCESS;
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);

			/// SUT 실행
			ZLoginCommandRouter::RouteResponseLoginGameServer(m_uidLoginServer, m_pPlayer->GetUID(), nResult, &LoginGameServerInfo);

			/// 검증
			ASSERT_EQUAL(m_pLinkLoginServer->GetCommandID(0), MLC_COMM_RESPONSE_LOGIN_GAME_SERVER);
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MUID>(0, 0), m_pPlayer->GetUID());
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<int>(0, 1), nResult);

			TD_LOGIN_GAME_SERVER_INFO* pInfo;
			int nCount;
			ASSERT_EQUAL(m_pLinkLoginServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 1);
			ASSERT_CHECK(m_pLinkLoginServer->GetCommand(0).GetBlob(pInfo, nCount, 2));
			CHECK(!wcsncmp(pInfo->strHostName, strHostName.c_str(), MAX_HOSTNAME_LEN));
			CHECK_EQUAL(pInfo->nPort, port);
			CHECK_EQUAL(pInfo->uidConnectionKey, uidConnectionKey);
		}

		// 게임서버로 로그인, 실패응답 커멘드 보내기 확인
		TEST_FIXTURE(Fixture, TestCommandRouter_ResponseLoginGameServer_Failure)
		{
			wstring strHostName = ZTestHelper::NewString();
			int port = 200;
			MUID uidConnectionKey = ZTestHelper::NewUID();
			int nResult = CR_FAIL;
			TD_LOGIN_GAME_SERVER_INFO LoginGameServerInfo = MakeLoginGameServerInfo(strHostName, port, uidConnectionKey);

			/// SUT 실행
			ZLoginCommandRouter::RouteResponseLoginGameServer(m_uidLoginServer, m_pPlayer->GetUID(), nResult, &LoginGameServerInfo);

			/// 검증
			ASSERT_EQUAL(m_pLinkLoginServer->GetCommandID(0), MLC_COMM_RESPONSE_LOGIN_GAME_SERVER);
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MUID>(0, 0), m_pPlayer->GetUID());
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<int>(0, 1), nResult);
			CHECK_EQUAL(m_pLinkLoginServer->GetParam<MBlob*>(0, 2)->GetBlobCount(), 0);
		}

		// 게임서버로 로그인, 요청 커멘드 보내기
		TEST_FIXTURE(Fixture, TestCommandRouter_RequestLoginGameServer)
		{
			MUID uidRequestServer = ZTestHelper::NewUID();
			MUID uidConnectionKey = ZTestHelper::NewUID();

			int nEnterKey = 200;
			TD_LOGIN_ACCOUNT_INFO LoginAccountInfo = MakeLoginAccountInfo(nEnterKey, m_pPlayer);

			/// SUT 실행
			ZLoginCommandRouter::RouteRequestLoginGameServer(m_uidGameServer, uidRequestServer, m_pPlayer->GetUID(), uidConnectionKey, &LoginAccountInfo);

			/// 검증
			ASSERT_EQUAL(m_pLinkGameServer->GetCommandID(0), MMC_COMM_REQUEST_LOGIN_GAME_SERVER);
			CHECK_EQUAL(m_pLinkGameServer->GetParam<MUID>(0, 0), uidRequestServer);
			CHECK_EQUAL(m_pLinkGameServer->GetParam<MUID>(0, 1), m_pPlayer->GetUID());
			CHECK_EQUAL(m_pLinkGameServer->GetParam<MUID>(0, 2), uidConnectionKey);

			TD_LOGIN_ACCOUNT_INFO* pInfo;
			ASSERT_CHECK(m_pLinkGameServer->GetCommand(0).GetSingleBlob(pInfo, 3));
			CHECK_EQUAL(pInfo->nEnterKey, nEnterKey);
			CHECK_EQUAL(pInfo->nAID, m_pPlayer->GetAID());
			CHECK_EQUAL(pInfo->nCID, m_pPlayer->GetCID());
			CHECK(0 == wcscmp(pInfo->szUserID, m_pPlayer->GetPlayerName().c_str()));
		}
	} ///< namespace TestCommandRouter
}

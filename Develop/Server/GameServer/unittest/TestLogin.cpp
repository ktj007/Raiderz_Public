#include "stdafx.h"
#include "TestLogin.h"
#include "GPlayerAcceptManager.h"
#include "GUTHelper_Login.h"
#include "GUTHelper_Field.h"
#include "GUTHelper_CmdHandler.h"
#include "CCommandResultTable.h"
#include "GLoginLogicMoveGameServer.h"
#include "GServer.h"
#include "GAsyncTask.h"
#include "GDBTaskCharData.h"

using namespace Login;

class MockDBTaskCharSerialize : public GDBTaskCharSerialize
{
private:
	MockLoginDBManager* m_pDBManager;
public:
	MockDBTaskCharSerialize(const MUID& uidReqPlayer, MockLoginDBManager* pDBManager) : GDBTaskCharSerialize(uidReqPlayer), m_pDBManager(pDBManager) { SetTaskSuccess(); }
	virtual ~MockDBTaskCharSerialize() {}
	virtual mdb::MDB_THRTASK_RESULT OnCompleted() override
	{
		m_Data.CharInfo.nCID = 2;
		m_Data.CharInfo.nGID = 0;
		m_Data.CharInfo.strName = L"birdkr";
		m_Data.CharInfo.nHP = 100;		

		m_Data.CharInfo.nSharedFieldID = m_pDBManager->GetExpectSharedFieldID();
		m_Data.CharInfo.nDynamicFieldID = m_pDBManager->GetExpectDynamicFieldID();

		return GDBTaskCharSerialize::OnCompleted();
	}
};

bool MockLoginDBManager::CharSerialize( const GDBT_CHAR_SERIALIZE& data )
{
	MockDBTaskCharSerialize task(data.m_uidPlayer, this);
	task.Input(data.m_nCID, data.m_nUIID, data.m_bReload);
	task.OnPrePost();
	task.OnCompleted();
	return true;
}


//////////////////////////////////////////////////////////////////////////

FLogin::FLogin()
{
	InitField();

	// -- link
	MUID uidLink = SUnitTestUtil::NewUID();
	m_pLink = NewLink(uidLink);
	m_pLink->AddIgnoreID(MC_FATIGUE_UPDATE);
	m_pLink->AddIgnoreID(MC_TIME_SYNCH);


	// -- login
	m_uidRequestServer = SUnitTestUtil::NewUID();
	m_uidRequestID = SUnitTestUtil::NewUID();
	m_uidConnectionKey = SUnitTestUtil::NewUID();

	m_nAID = 1;
	m_nCID = 2;
	m_nGID = 0;
}

FLogin::~FLogin()
{

}

void FLogin::InitField()
{
	// -- field
	m_nSharedFieldID = 1;
	m_nSharedFieldID2 = 2;

	m_nDynamicFieldID = 200;

	GFieldInfo* pSharedFieldInfo = AddFieldInfo(m_nSharedFieldID);
	pSharedFieldInfo->m_bDynamic = false;

	GFieldInfo* pSharedFieldInfo2 = AddFieldInfo(m_nSharedFieldID2);
	pSharedFieldInfo2->m_bDynamic = false;

	GFieldInfo* pDynamicFieldInfo = AddFieldInfo(m_nDynamicFieldID);
	pDynamicFieldInfo->m_bDynamic = true;

	FIELD_GROUP_INFO* pFieldGroupInfo = GUTHelper_Field::NewFieldGroupInfo(pDynamicFieldInfo);
	GDynamicFieldMaster* pDynamicFieldMaster = GUTHelper_Field::NewTrialFieldMaster(pFieldGroupInfo);

	m_uidDynamicFieldGroup = pDynamicFieldMaster->GetUID();

	GUTHelper_Field::NewSharedField(pSharedFieldInfo, 1);
	GUTHelper_Field::NewSharedField(pSharedFieldInfo2, 1);
}



TD_LOGIN_ACCOUNT_INFO FLogin::MakeParam_TD_LOGIN_ACCOUNT_INFO()
{
	TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo;

	wcsncpy_s(tdLoginAccountInfo.szUserID, L"birdkr", _TRUNCATE);
	tdLoginAccountInfo.nAID = m_nAID; 
	tdLoginAccountInfo.nCID = m_nCID;
	tdLoginAccountInfo.nEnterKey = 500;

	return tdLoginAccountInfo;
}

void FLogin::OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER( MUID uidRequestServer, MUID uidRequestID, MUID uidConnectionKey, TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo )
{
	m_pNetAgent->OnRecv(MMC_COMM_REQUEST_LOGIN_GAME_SERVER, 
		4, 
		NEW_UID(uidRequestServer),
		NEW_UID(uidRequestID),
		NEW_UID(uidConnectionKey),
		NEW_SINGLE_BLOB(pLoginAccountInfo, sizeof(TD_LOGIN_ACCOUNT_INFO))
		);
}

void FLogin::OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER( MUID uidConnectionKey )
{
	m_pLink->OnRecv(MC_COMM_REQUEST_LOGIN_GAME_SERVER, 2, 
		NEW_UID(uidConnectionKey),
		NEW_INT(SH_COMMAND_VERSION)
		);
}

void FLogin::OnRecv_MC_CHAR_GAMESTART( int nFieldID/*=-1*/ )
{
	m_pLink->OnRecv(MC_CHAR_GAMESTART, 2, 
		NEW_INT(nFieldID),
		NEW_VEC(vec3::ZERO)
		);
}

void FLogin::OnRecv_MMC_PLAYER_STATE_IN_WORLD_RES(CID nCID, int nResult)
{
	m_pNetAgent->OnRecv(MMC_PLAYER_STATE_IN_WORLD_RES, 2, 
		NEW_INT(nCID),
		NEW_INT(nResult)
		);
}

//////////////////////////////////////////////////////////////////////////


SUITE(Login)
{
	TEST_FIXTURE(FLogin, TestLogin_MMC_COMM_REQUEST_LOGIN_GAME_SERVER)
	{
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();

		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);

		CHECK_EQUAL(MMC_COMM_RESPONSE_LOGIN_GAME_SERVER, m_pNetAgent->GetSendCommandID(0));

		CHECK_EQUAL(m_uidRequestServer, m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(m_uidRequestID, m_pNetAgent->GetSendParam<MUID>(0, 1));
		CHECK_EQUAL((int)(CR_SUCCESS), m_pNetAgent->GetSendParam<int>(0, 2));

		TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo;
		m_pNetAgent->GetSendCommand(0).GetSingleBlob(pLoginGameServerInfo, 3);

		CHECK_EQUAL(m_uidConnectionKey, pLoginGameServerInfo->uidConnectionKey);
		CHECK_EQUAL(GConfig::m_nPort, pLoginGameServerInfo->nPort);
	}

	TEST_FIXTURE(FLogin, TestLogin_MC_COMM_REQUEST_LOGIN_GAME_SERVER__AcceptorSize)
	{
		GPlayerAcceptManager* pPlayerAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
		CHECK_EQUAL(0, pPlayerAcceptManager->GetSize(GPlayerAcceptor::FROM_LOGIN_SERVER));

		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);

		CHECK_EQUAL(1, pPlayerAcceptManager->GetSize(GPlayerAcceptor::FROM_LOGIN_SERVER));

		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidConnectionKey);

		CHECK_EQUAL(0, pPlayerAcceptManager->GetSize(GPlayerAcceptor::FROM_LOGIN_SERVER));
	}


	TEST_FIXTURE(FLogin, TestLogin_MC_COMM_REQUEST_LOGIN_GAME_SERVER__Success)
	{
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);

		// 로그인 하기 전의 값 체크
		CHECK_EQUAL(0, gmgr.pPlayerObjectManager->GetPlayersCount());

		// 클라이언트 로그인
		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidConnectionKey);

		// 로그인 응답 커맨드 체크
		CHECK_EQUAL(MC_COMM_RESPONSE_LOGIN_GAME_SERVER, m_pLink->GetCommandID(0));
		CHECK_EQUAL(CR_SUCCESS, m_pLink->GetParam<int>(0, 0));
		CHECK_EQUAL(m_pLink->GetUID(), m_pLink->GetParam<MUID>(0, 1));

		// 로그인 후 값 체크
		CHECK_EQUAL(1, gmgr.pPlayerObjectManager->GetPlayersCount());

		GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_pLink->GetUID());
		CHECK(pPlayerObject != NULL);

		CHECK_EQUAL(m_nAID, pPlayerObject->GetAccountInfo().nAID);
		CHECK(0 == wcscmp(L"birdkr", pPlayerObject->GetAccountInfo().szUserID));

		CHECK_EQUAL(m_nCID, pPlayerObject->GetEntity()->GetCID());
	}


	TEST_FIXTURE(FLogin, TestLogin_MC_COMM_REQUEST_LOGIN_GAME_SERVER__WrongConnectionKey)
	{
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);

		// 잘못된 ConnectionKey로 클라이언트 로그인
		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(SUnitTestUtil::NewUID());

		// 로그인 응답 커맨드 체크
		CHECK_EQUAL(MC_COMM_RESPONSE_LOGIN_GAME_SERVER, m_pLink->GetCommandID(0));
		CHECK_EQUAL(CR_FAIL_LOGIN, m_pLink->GetParam<int>(0, 0));
		CHECK_EQUAL(MUID::ZERO, m_pLink->GetParam<MUID>(0, 1));

		CHECK_EQUAL(0, gmgr.pPlayerObjectManager->GetPlayersCount());
		CHECK_EQUAL(1, gmgr.pPlayerObjectManager->GetAcceptManager()->GetSize(GPlayerAcceptor::FROM_LOGIN_SERVER));
	}


	TEST_FIXTURE(FLogin, TestLogin_MC_CHAR_GAMESTART_Success)
	{
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);
		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidConnectionKey);

		m_pLink->ResetCommands();
		m_pNetAgent->ResetSendCommands();

		// 게임 시작 요청
		OnRecv_MC_CHAR_GAMESTART();

		CHECK_EQUAL(0, m_pLink->GetCommandCount());
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

		// 마스터 서버 커맨드 체크
		CHECK_EQUAL(MMC_PLAYER_STATE_IN_WORLD_REQ, m_pNetAgent->GetSendCommandID(0));
		CHECK_EQUAL(m_nAID,					m_pNetAgent->GetSendParam<int>(0, 0));
		CHECK_EQUAL(m_nCID,					m_pNetAgent->GetSendParam<int>(0, 1));
		CHECK_EQUAL(m_pLink->GetUID(),		m_pNetAgent->GetSendParam<MUID>(0, 2));
		CHECK(wstring(L"birdkr") == m_pNetAgent->GetSendParam<const wchar_t*>(0, 3));
		CHECK_EQUAL(m_nGID,					m_pNetAgent->GetSendParam<int>(0, 4));
	}
	
	TEST_FIXTURE(FLogin, TestLogin_MMC_PLAYER_STATE_IN_WORLD_RES_Success)
	{
		/// Fixture 설치
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);
		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidConnectionKey);
		OnRecv_MC_CHAR_GAMESTART();
		
		m_pLink->ResetCommands();
		m_pNetAgent->ResetSendCommands();

		/// SUT 실행
		OnRecv_MMC_PLAYER_STATE_IN_WORLD_RES(m_nCID, CR_SUCCESS);
		
		/// 검증
		CHECK_EQUAL(2, m_pLink->GetCommandCount());
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

		ASSERT_EQUAL(MC_CHAR_MYINFO, m_pLink->GetCommandID(0));

		TD_MYINFO tdMyInfo;
		ASSERT_CHECK(m_pLink->GetCommand(0).GetParameter(&tdMyInfo, 0, MPT_SINGLE_BLOB));
		CHECK(0 == wcscmp(L"birdkr", tdMyInfo.szName));

		CHECK_EQUAL(MC_FIELD_PREPARING_TO_LOAD, m_pLink->GetCommandID(1));
		CHECK_EQUAL(MMC_GATE_ASK_SHARED_FIELD, m_pNetAgent->GetSendCommandID(0));
	}
	
	TEST_FIXTURE(FLogin, TestLogin_MC_CHAR_GAMESTART__ReqField)
	{
		/// Fixture 설치
		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);
		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidConnectionKey);

		const int REQ_FIELD = 2;
		const int nChannelID = 1;
		TD_GATE_INFO tdGateInfo = GUTHelper_CmdHandler::MakeParam_TD_GATE_INFO(REQ_FIELD, nChannelID);
		/// SUT 실행
		OnRecv_MC_CHAR_GAMESTART(REQ_FIELD);
		OnRecv_MMC_PLAYER_STATE_IN_WORLD_RES(m_nCID, CR_SUCCESS);
		GUTHelper_CmdHandler::OnRecv_MMC_GATE_RES(m_pNetAgent, m_pLink->GetUID(), RGT_EXECUTE, &tdGateInfo);

		/// 검증
		CHECK_EQUAL(MC_FIELD_CHANGE_FIELD, m_pLink->GetCommandID(3));
		CHECK_EQUAL(REQ_FIELD, m_pLink->GetParam<int>(3, 0));
	}

	TEST_FIXTURE(FLogin, TestLogin_MC_CHAR_GAMESTART__ExitSharedField)
	{
		const int EXIT_FIELD_ID = 2;
		MockDBManagerWrapper.Get()->ExpectSharedFieldID(EXIT_FIELD_ID);

		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);
		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidConnectionKey);

		// SUT 실행
		OnRecv_MC_CHAR_GAMESTART();
		OnRecv_MMC_PLAYER_STATE_IN_WORLD_RES(m_nCID, CR_SUCCESS);

		TD_GATE_INFO tdGateInfo = GUTHelper_CmdHandler::MakeParam_TD_GATE_INFO(EXIT_FIELD_ID, 1);
		GUTHelper_CmdHandler::OnRecv_MMC_GATE_RES(m_pNetAgent, m_pLink->GetUID(), RGT_EXECUTE, &tdGateInfo);

		/// 검증
		CHECK_EQUAL(MC_FIELD_CHANGE_FIELD, m_pLink->GetCommandID(3));
		CHECK_EQUAL(EXIT_FIELD_ID, m_pLink->GetParam<int>(3, 0));
	}

	TEST_FIXTURE(FLogin, TestLogin_MC_CHAR_GAMESTART__ExitSharedField_Exception_StandAlone)
	{
		/// Fixture 설치
		bool bStandAlone_Origin = GConfig::m_bStandAlone;
		GConfig::m_bStandAlone = true;

		const int EXIT_FIELD_ID = 3452;	// 없는 필드
		MockDBManagerWrapper.Get()->ExpectSharedFieldID(EXIT_FIELD_ID);

		TD_LOGIN_ACCOUNT_INFO tdLoginAccountInfo = MakeParam_TD_LOGIN_ACCOUNT_INFO();
		OnRecv_MMC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidRequestServer, m_uidRequestID, m_uidConnectionKey, &tdLoginAccountInfo);
		OnRecv_MC_COMM_REQUEST_LOGIN_GAME_SERVER(m_uidConnectionKey);

		/// SUT 실행
		OnRecv_MC_CHAR_GAMESTART();
		OnRecv_MMC_PLAYER_STATE_IN_WORLD_RES(m_nCID, CR_SUCCESS);

		/// 검증 : StandAlone일 경우에만 성공하고, 서버의 첫번째 필드로 입장처리 됨
		CHECK_EQUAL(MC_FIELD_CHANGE_FIELD, m_pLink->GetCommandID(3));
		CHECK_EQUAL(m_nSharedFieldID, m_pLink->GetParam<int>(3, 0));

		/// Fixture 해체
		GConfig::m_bStandAlone = bStandAlone_Origin;
	}


	class FLogin_MoveGameServer : public FLogin
	{
	public:
		FLogin_MoveGameServer() : FLogin() 
		{

		}
		virtual ~FLogin_MoveGameServer() 
		{

		}

		void OnRecv_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER(MUID uidPlayer, MUID uidConnectionKey)
		{
			GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
			gmgr.pPlayerObjectManager->AddPlayer(pPlayerObject);

			GEntityPlayer* pEntityPlayer = pPlayerObject->GetEntity();

			MCommand* pCommand = GUTHelper_Login::NewCommand_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER(pEntityPlayer, uidConnectionKey, m_pNetAgent->GetServerUID(), m_pNetAgent->GetUID());

			m_pNetAgent->OnRecv(pCommand);

			gmgr.pPlayerObjectManager->DeleteAllPlayer();
		}
	};
	TEST_FIXTURE(FLogin_MoveGameServer, TestLogin_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER)
	{
		GPlayerAcceptManager* pAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();


		// 처음엔 Acceptor가 하나도 없음
		CHECK_EQUAL(0, pAcceptManager->GetSize(GPlayerAcceptor::FROM_GAME_SERVER));

		MUID uidPlayer = SUnitTestUtil::NewUID();
		MUID uidConnectionKey = SUnitTestUtil::NewUID();

		OnRecv_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER(uidPlayer, uidConnectionKey);


		// 한명 예약됨
		CHECK_EQUAL(1, pAcceptManager->GetSize(GPlayerAcceptor::FROM_GAME_SERVER));

		CHECK_EQUAL(MMC_COMM_RESPONSE_READY_ENTER_MOVE_PLAYER, m_pNetAgent->GetSendCommandID(0));
		MUID uidServer = m_pNetAgent->GetServerUID();
		CHECK_EQUAL(uidServer, m_pNetAgent->GetSendParam<MUID>(0, 0));
		CHECK_EQUAL(CR_SUCCESS, m_pNetAgent->GetSendParam<int>(0, 2));
	}

	TEST_FIXTURE(FLogin_MoveGameServer, TestLoginMoveGameServer_RemainPastPlayerObject_Should_CreateAsyncTask)
	{
		/// Fixture 설치 : 과거에 Logout 처리가 진행 중이던 플레이어 오브젝트를 생성합니다.
		MUID uidPaskPlayer = SUnitTestUtil::NewUID();
		GPlayerObject* pPastPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(uidPaskPlayer);
		gmgr.pPlayerObjectManager->AddPlayer(pPastPlayerObject);

		pPastPlayerObject->OnLogout();

		// 서버 이동을 요청만 할 플레이어 생성 : Acceptor 등록을 위해 생성. Entity 가 필요하기 때문에 생성.
		MUID uidMoveServerPlayer = uidPaskPlayer; ///< 동일한 플레이어의 서버 이동임으로 같은 UID 를 사용합니다.
		GPlayerObject* pMoveServerPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(uidMoveServerPlayer);
		GEntityPlayer* pEntityPlayer = pMoveServerPlayerObject->GetEntity();

		MUID uidConnectionKey = SUnitTestUtil::NewUID();
		
		// 커맨드를 처리하여 Acceptor 를 생성합니다. 생성된 Acceptor 는 GLoginLogicMoveGameServer::Init() 메소드에서 참조합니다.
		MCommand* pCommand = GUTHelper_Login::NewCommand_MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER(pEntityPlayer, uidConnectionKey, m_pNetAgent->GetServerUID(), m_pNetAgent->GetUID());
		m_pNetAgent->OnRecv(pCommand);

		// 서버를 이동 중인 플레이어는 사실상 오브젝트가 없는 플레이어이기 때문에 제거합니다.
		pMoveServerPlayerObject->Destroy();
		SAFE_DELETE(pMoveServerPlayerObject);
		
		
		GLoginLogicMoveGameServer loginLogicMoveGameServer;
		MUID uidPlayerSession = SUnitTestUtil::NewUID();
		bool bRet = loginLogicMoveGameServer.Init(uidPlayerSession, uidConnectionKey);
		
		CHECK_EQUAL(true, bRet);
		CHECK_EQUAL(0, gsys.pServer->GetAsyncTaskManager()->GetTaskCount());
		
		/// SUT 실행
		bRet = loginLogicMoveGameServer.Apply();

		/// 검증
		CHECK_EQUAL(false, bRet);
		CHECK_EQUAL(1, gsys.pServer->GetAsyncTaskManager()->GetTaskCount());

		MUID uidPrevTask = gsys.pServer->GetAsyncTaskManager()->NewUID() - 1;
		GAsyncTask* pTask = gsys.pServer->GetAsyncTaskManager()->FindTask(uidPrevTask);
		ASSERT_CHECK(NULL != pTask);
		CHECK_EQUAL(ASYNC_TASK_LAZY_LOGIN_MOVE_GAMESERVER, pTask->GetID());
		
		gmgr.pPlayerObjectManager->DeleteAllPlayer();
		gsys.pServer->GetAsyncTaskManager()->Clear();
	}
}


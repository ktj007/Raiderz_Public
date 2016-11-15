#include "stdafx.h"
#include "LServer.h"
#include "LDef.h"
#include "SDef.h"
#include "LConfig.h"
#include "LCommandTable.h"
#include "LCommandCenter.h"
#include "LPlayerObjectManager.h"
#include "LPlayerObject.h"
#include "SFilePath.h"
#include "LMasterClient.h"
#include "LDBManager.h"
#include "LServerStatusUpdater.h"
#include "LTimeoutManager.h"
#include "LServableChecker.h"
#include "SCommandProfileController.h"
#include "CSStringFilter.h"
#include "SNetworkCardMgr.h"
#include "LPlayerLoginGameServerProcManager.h"
#include "LPlayerLoginGameServerRetryer.h"
#include "PMServerInitLogger.h"
#include "SDsnFactory.h"
#include "SDefaultDsnFactory.h"
#include "MCommandProfiler.h"
#include "CSGameWordsFilter.h"
#include "CSNameWordFilter.h"
#include "MLocale.h"
#include "LWorldLocatorServer.h"
#include "LPlayerAcceptManager.h"

//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////

LServer::LServer()
: m_bCreated(false)
, m_bExiting(false)
, m_pCommandCenter(NULL)
, m_pNetServer(NULL)
, m_pWorldLocatorServer(NULL)
, m_pServerStatusUpdater(NULL)
, m_pPlayerLoginGameServerProcManager(NULL)
, m_pPlayerLoginGameServerRetryer(NULL)
, m_pMasterClient(NULL)
, m_pCommandProfileController(NULL)
{
	m_pServerInfoViewRglt = new MRegulator(APP_SERVERVIEW_UPDATE_TICK);			
	m_pServableChecker = new LServableChecker();
	m_pServableChecker->SetServerStartCallback(OnServerStartCallback);
}

LServer::~LServer()
{	
	SAFE_DELETE(m_pServableChecker);
	SAFE_DELETE(m_pPlayerLoginGameServerProcManager);
	SAFE_DELETE(m_pPlayerLoginGameServerRetryer);
	SAFE_DELETE(m_pServerStatusUpdater);
	SAFE_DELETE(m_pServerInfoViewRglt);
}

bool LServer::CreateNetwork()
{
	minet::MNetServerDesc server_desc;
	server_desc.bHeartBeat = true;
	server_desc.nHeartBeatTickTime = 60000;		// 60초마다 heartbeat
	server_desc.nHeartBeatTimeoutTime = 600000;	// 10분동안 아무 응답이 없으면 접속 끊음

	m_pNetServer = new LNetServer(server_desc);
	m_pNetServer->AttachCommandProfiler(new MCommandProfiler());

	m_pCommandCenter = new LCommandCenter(m_pNetServer);
	gsys.pCommandCenter = m_pCommandCenter;	// 일단..
	m_pCommandProfileController = new SCommandProfileController(m_pNetServer,
		LConfig::m_strCommandProfiler_LogFolder,
		LConfig::m_strCommandProfiler_LogName,
		LConfig::m_bCommandProfiler_AutoSave,
		LConfig::m_nCommandProfiler_AutoSaveTickMinute);

	if (LConfig::m_bCommandProfiler_AutoStart)
		StartCommandProfile();

	/// 서버 네트워크 모듈 생성 (Player 통신용)
	MUID uidSeed = MUID(LConfig::m_nServerID * SERVER_UID_SEED_POWER, 0);
	wstring strMyNetworkCardIP = GetNetworkCardMgr()->GetIP(LConfig::m_nMyNetworkCardID);
	
	if (m_pNetServer->Create(LConfig::m_nPort, false ,uidSeed, LConfig::m_nSocketPoolSize, LConfig::m_nSendPendingLimitCount, MLocale::ConvUTF16ToAnsi(strMyNetworkCardIP.c_str()).c_str()) == false)
	{
		mlog("Failed To NetServer Creation\n");
		return false;
	}

	m_pWorldLocatorServer = new LWorldLocatorServer();
	if (!m_pWorldLocatorServer->Create(LConfig::m_nUDPPort))
	{
		mlog("Failed To WorldLocator Creation\n");
		return false;
	}


	/// 클라이언트 네트워크 모듈 생성 (MasterServer 통신용)
	m_pMasterClient = new LMasterClient();
	if (m_pMasterClient->Create() == false)
	{
		mlog("Failed To MasterClient Creation\n");
		return false;
	}

	mlog("Server Socket Opened. (Port = %d)\n", LConfig::m_nPort);
	return true;
}

void LServer::DestroyNetwork()
{
	if (m_pWorldLocatorServer != NULL)
	{
		m_pWorldLocatorServer->Destroy();
	}

	if (m_pNetServer != NULL)
	{
		m_pNetServer->Destroy();
	}
	
	if (m_pMasterClient != NULL)
	{
		m_pMasterClient->Destroy();
	}

	SAFE_DELETE(m_pWorldLocatorServer);
	SAFE_DELETE(m_pCommandProfileController);
	SAFE_DELETE(m_pCommandCenter);
	SAFE_DELETE(m_pNetServer);
	SAFE_DELETE(m_pMasterClient);	

	gsys.pCommandCenter = NULL;
}

bool LServer::Create()
{
	if (CreateNetwork() == false)
	{
		return false;
	}
	
	if (InitInfo() == false)
	{
		return false;
	}

	m_pServerStatusUpdater = new LServerStatusUpdater(gsys.pDBManager);
	m_pServerStatusUpdater->Start();

	
	// 플레이어의 게임서버 로그인 처리 관리자
	m_pPlayerLoginGameServerProcManager = new LPlayerLoginGameServerProcManager;

	// 플레이어의 게임서버 로그인 재시도 요청자
	m_pPlayerLoginGameServerRetryer = new LPlayerLoginGameServerRetryer;
	gsys.pPlayerLoginGameServerRetryer = m_pPlayerLoginGameServerRetryer;
	
	// 타임아웃 매니저 초기화
	gmgr.pTimeoutManager->InitWatcher();


	m_bCreated = true;

	WriteServerInfoLog();

	if (LConfig::m_bStandAlone)
		mlog("StandAlone Mode!!!\n");

	return true;
}

void LServer::Destroy()
{
	ReleaseDB();

	DestroyNetwork();

	m_bCreated = false;

	mlog("Server Closed\n");
}


void LServer::SetDsnFactory()
{
	SDsnFactory::GetInstance().Set(
		new SDefaultDsnFactory(
			mdb::MDatabaseDesc(LConfig::m_strOdbcDriver, LConfig::m_AccountDBConfig.strServer, LConfig::m_AccountDBConfig.strDBName, LConfig::m_AccountDBConfig.strUserName, LConfig::m_AccountDBConfig.strPassword)
		  , mdb::MDatabaseDesc(LConfig::m_strOdbcDriver, LConfig::m_GameDBConfig.strServer, LConfig::m_GameDBConfig.strDBName, LConfig::m_GameDBConfig.strUserName, LConfig::m_GameDBConfig.strPassword)
		  , mdb::MDatabaseDesc(LConfig::m_strOdbcDriver, LConfig::m_LogDBConfig.strServer, LConfig::m_LogDBConfig.strDBName, LConfig::m_LogDBConfig.strUserName, LConfig::m_LogDBConfig.strPassword)));
}


bool LServer::InitDB()
{
	SetDsnFactory();

	mdb::MDatabaseDesc dbDesc = SDsnFactory::GetInstance().Get()->GetGameDSN();

	if (gsys.pDBManager->Connect(dbDesc))
	{
		mlog("Create Sync Database success.\n");
	}
	else
	{
		mlog3 ("Create Sync Database failed.\n");
		return false;
	}

	if (mdb::MDBAR_SUCCESS != gsys.pDBManager->InitAsyncDB())
	{
		
		mlog("Create Async Database Failed\n");
		return false;
	}
	else
	{
		mlog("Create Async Database Successed\n");
	}

	return true;
}

void LServer::ReleaseDB()
{
	gsys.pDBManager->Disconnect();
	gsys.pDBManager->Release();
}

bool LServer::InitInfo()
{
	if (InitDB() == false)
	{
		_ASSERT(0);
		mlog3("Failed InitDB\n");
		return false;
	}

	// 금칙어 목록 로드
	if (!GetStringFilter()->LoadFromFile(FILENAME_ABUSE))
	{
		mlog3("Failed! Load String Filter.\n");
		return false;
	}

	// 이름 필터링 데이터 로드
	if (!GetNameWordFilter()->LoadFromFile(FILENAME_NAME_ILLEGALWORDS, FILENAME_NAME_ALLOWEDCHARS))
	{
		mlog3("Failed! Load name_illegalwords.txt, name_allowedchars.txt\n");
		return false;
	}
		 
	if (!GetGameWordsFilter()->LoadFromFile(FILENAME_GAMEWORDS))	
	{		
		mlog3("Failed! Load GameWords.txt.\n");
		return false;
	}

	return true;
}

void LServer::DeInitInfo()
{
}


bool LServer::Update(float fDelta)
{
	if (m_bExiting)
		return false;

	m_pServableChecker->Update(fDelta);	
			
	// 커맨드 펌프는 틱에 상관없이 처리한다.
	m_pMasterClient->Run();
	m_pNetServer->Run();

	// DB 요청 처리는 커맨드 펌프가 끝난 다음에 한다.
	gsys.pDBManager->Update();

	{
		int nTick = DEFAULT_UPDATE_TICK;

		static float fElapsed = 0;
		fElapsed += fDelta;

		if (fElapsed < (1.0f / nTick)) 
		{
			Sleep(10);
			return true;
		}
		fDelta = fElapsed;
		fElapsed = 0.0f;

		m_ServerInfo.nFPS = m_FrameCounter.Update();
	}

	
	// 등록된 타임아웃 감시
	gmgr.pTimeoutManager->Update();

	// Acceptor 관리자
	gmgr.pPlayerAcceptManager->Update();

	// 캐릭터 선택 처리 관리자
	m_pPlayerLoginGameServerProcManager->Update(fDelta);

	// 게임서버 로그인 재시도 처리
	m_pPlayerLoginGameServerRetryer->Update(fDelta);


	UpdateForDebug(fDelta);
	m_pServerStatusUpdater->Update(fDelta);
	
	if (m_pWorldLocatorServer)
	{
		m_pWorldLocatorServer->Update(fDelta);
	}


	if (m_pCommandProfileController)
		m_pCommandProfileController->Update(fDelta);

	return true;
}

void LServer::DisconnectPlayer(MUID uidPlayer)
{
	LPlayerObject* pObj = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pObj == NULL) return;

	if (m_pNetServer)
		m_pNetServer->Disconnect(pObj->GetUID());
}

void LServer::HardDisconnectPlayer(MUID uidPlayer)
{
	LPlayerObject* pObj = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pObj == NULL) return;

	if (m_pNetServer)
		m_pNetServer->HardDisconnect(pObj->GetUID());
}

void LServer::OnServerStartCallback()
{
	mlog("===== All servers start!\n");

	if (gsys.pServer->GetNetServer())
		gsys.pServer->GetNetServer()->ChangeCmdHandlerForServerStart();
}


// 서버에서 쓰는 각종 셋팅 값들을 표시한다.
void LServer::WriteServerInfoLog()
{
	mlog("Server Created. (WorldID=%d , ServerID=%d)\n", LConfig::m_nWorldID, LConfig::m_nServerID);
}


void LServer::UpdateServerInfoView()
{
	m_ServerInfo.nLinkCount = m_pNetServer->GetLinkCount();

	m_ServerInfo.nCurrPlayerCount = gmgr.pPlayerObjectManager->GetClientsCount();

	LNetServer* pNetServer = dynamic_cast<LNetServer*>(m_pNetServer);
	m_ServerInfo.nRecvCPS = pNetServer->GetRecvCPS();
	m_ServerInfo.nSendCPS = pNetServer->GetSendCPS();
	m_ServerInfo.nLocalCPS = pNetServer->GetLocalCPS();

	m_pNetServer->GetSecondTraffic(m_ServerInfo.nRecvBPS, m_ServerInfo.nSendBPS);

	// UI 업데이트
	// ZApplication::GetInstancePtr()->SetServerInfoView(m_ServerInfo);
}

void LServer::UpdateForDebug( float fDelta )
{
	// 서버 정보 업데이트
	if (m_pServerInfoViewRglt->IsReady(fDelta))
	{
		UpdateServerInfoView();	
	}
}

void LServer::StartCommandProfile()
{
	if (m_pCommandProfileController->IsStarted())
	{
		mlog("!Notice! Command Profiler Already Started!\n");
		return;
	}

	mlog("Command Profiler Start!\n");
	m_pCommandProfileController->Start();
}

void LServer::FinishCommandProfile()
{
	mlog("Command Profiler Finish!\n");
	m_pCommandProfileController->Save(true);
}

void LServer::Exit()
{
	m_bExiting = true;
}
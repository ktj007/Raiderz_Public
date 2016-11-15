#include "stdafx.h"
#include "GServer.h"
#include "GPlayerObject.h"
#include "GDef.h"
#include "GCalculator.h"
#include "GConst.h"
#include "GBirdMonitor.h"
#include "GTalentInfo.h"
#include "GBuffInfo.h"
#include "GNPCInfo.h"
#include "GItemManager.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "GQuestInfoMgr.h"
#include "GPathFinder.h"
#include "GSpawnManager.h"
#include "GColtRunner.h"
#include "GGlobal.h"
#include "GConditionsInfoMgr.h"
#include "GItemConditionsInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GCommandTable.h"
#include "GCommandCenter.h"
#include "GWorld.h"
#include "GTalentInfoMgr.h"
#include "GConfig.h"
#include "GStrings.h"
#include "GFieldMgr.h"
#include "GPlayerObjectManager.h"
#include "GSoulBindingInfoMgr.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GMasterNetClient.h"
#include "GServerResLoader.h"
#include "GFieldInfoMgr.h"
#include "CSLuaCallbackLogger.h"
#include "GBattleArenaMgr.h"
#include "GAIProfiler.h"
#include "GFieldFactory.h"
#include "GAsyncTask.h"
#include "GMasterServerFacade.h"
#include "GAppServerFacade.h"
#include "GServerStatusUpdater.h"
#include "GServerDumper.h"
#include "GServableChecker.h"
#include "GNPCShopInfoMgr.h"
#include "GRecipeInfoMgr.h"
#include "GCraftInfoMgr.h"
#include "GInnRoomInfoMgr.h"
#include "GFactionInfoMgr.h"
#include "GFactionRelationInfoMgr.h"
#include "GCutsceneInfoMgr.h"
#include "SCommandProfileController.h"
#include "GRecordModeServer.h"
#include "CSStringFilter.h"
#include "SNetworkCardMgr.h"
#include "GEmblemInfoMgr.h"
#include "GReserveCmdManager.h"
#include "PMServerInitLogger.h"
#include "SDsnFactory.h"
#include "SBaseDsnFactory.h"
#include "SDefaultDsnFactory.h"
#include "PmDsnFactory.h"
#include "CSGameWordsFilter.h"
#include "GDBCacheSystem.h"
#include "GDBCacheRegularFlush.h"
#include "CSNameWordFilter.h"
#include "GServerCloser.h"
#include "GAutoPartyMgr.h"
#include "GQPEventInfoMgr.h"
#include "GFieldStatusUpdater.h"
#include "GPresetInfoMgr.h"
#include "GChallengerQuestMgr.h"
#include "GTestSystem.h"

//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////

GServer::GServer()
: m_bCreated(false)
, m_pCommandCenter(NULL)
, m_pNetServer(NULL)
, m_pServerStatusUpdater(NULL)
, m_pFieldStatusUpdater(NULL)
, m_pCommandProfileController(NULL)
, m_pCommandProfileController_ForMasterServer(NULL)
, m_pCommandProfileController_ForAppServer(NULL)
{
	m_pServerInfoViewRglt = new MRegulator(APP_SERVERVIEW_UPDATE_TICK);
	m_pFrameCounter = new SFrameCounter(gsys.pSystem);
	m_pAsyncTaskManager = new GAsyncTaskManager();
	m_pMasterServerFacade = new GMasterServerFacade();
	m_pAppServerFacade = new GAppServerFacade();
	m_pServableChecker = new GServableChecker();
	m_pDBCacheRegularFlush = new GDBCacheRegularFlush();
	m_pServerCloser = new GServerCloser();

	gsys.pMasterServerFacade = m_pMasterServerFacade;
	gsys.pAppServerFacade = m_pAppServerFacade;
}

GServer::~GServer()
{
	SAFE_DELETE(m_pServerCloser);
	SAFE_DELETE(m_pServableChecker);
	SAFE_DELETE(m_pAsyncTaskManager);
	SAFE_DELETE(m_pMasterServerFacade);
	SAFE_DELETE(m_pAppServerFacade);
	SAFE_DELETE(m_pFrameCounter);
	SAFE_DELETE(m_pServerStatusUpdater);
	SAFE_DELETE(m_pFieldStatusUpdater);
	SAFE_DELETE(m_pServerInfoViewRglt);
	SAFE_DELETE(m_pDBCacheRegularFlush);

	gsys.pMasterServerFacade = NULL;
	gsys.pAppServerFacade = NULL;
}

bool GServer::Create( const std::wstring& strServerVersion, bool bConnectDB )
{
	if (InitRequisites(bConnectDB) == false)
	{
		return false;
	}

	if (InitInfo() == false)
	{
		return false;
	}

	if (bConnectDB)
	{
		Init_UpdateDB_Infos();
	}

	gsys.pWorld->Create();

	gsys.pMasterServerFacade->Init();
	gsys.pAppServerFacade->Init();

	m_bCreated = true;

	WriteServerInfoLog();
	
	/// 로딩이 모두 끝난 뒤에 Server Status Update 를 시작해야 합니다.
	m_pServerStatusUpdater->Start(strServerVersion);

	return true;
}

void GServer::Destroy()
{
	// 반드시 GPlayerObjectManager보다 먼저 릴리즈 되어야 한다.
	gsys.pDBCacheSystem->Release();

	gmgr.pPlayerObjectManager->DeleteAllPlayer();
	gmgr.pPlayerObjectManager->Clear();
	gmgr.pFieldMgr->Clear();

	gsys.pWorld->Destroy();	
	gsys.pScriptManager->Fini();

	ReleaseDB();

	DestroyNetwork();

	m_bCreated = false;
}

bool GServer::InitRequisites(bool bConnectDB)
{
	if (CreateNetwork() == false)
	{
		mlog3("Failed CreateNetwork.\n");
		SetServerInitResult(SERVERINIT_FAILED_NETWORK_INIT);
		return false;
	}

	mlog("Socket initialized\n");

	if (bConnectDB)
	{
		if (InitDB() == false)
		{
			_ASSERT(0);
			mlog3("Failed InitDB.\n");
			SetServerInitResult(SERVERINIT_FAILED_DB_CONNECT);
			return false;
		}
	}

	m_pServerStatusUpdater = new GServerStatusUpdater(gsys.pDBManager);
	m_pFieldStatusUpdater = new GFieldStatusUpdater();

	return true;
}

void GServer::SetDsnFactory()
{
	SDsnFactory::GetInstance().Set(
		new SDefaultDsnFactory(
		mdb::MDatabaseDesc()
		, mdb::MDatabaseDesc(GConfig::m_strOdbcDriver, GConfig::m_strGameDB_Server, GConfig::m_strGameDB_DatabaseName, GConfig::m_strGameDB_UserName, GConfig::m_strGameDB_Password)
		, mdb::MDatabaseDesc(GConfig::m_strOdbcDriver, GConfig::m_strLogDB_Server, GConfig::m_strLogDB_DatabaseName, GConfig::m_strLogDB_UserName, GConfig::m_strLogDB_Password)));

	if (GConfig::m_bPmangDBEncrypt)
	{
		SDsnFactory::GetInstance().Set(
			new PmDsnFactory(
			mdb::MDatabaseDesc()
			, mdb::MDatabaseDesc(GConfig::m_strOdbcDriver, GConfig::m_strGameDB_Server, GConfig::m_strGameDB_DatabaseName, GConfig::m_strGameDB_UserName, GConfig::m_strGameDB_Password)
			, mdb::MDatabaseDesc(GConfig::m_strOdbcDriver, GConfig::m_strLogDB_Server, GConfig::m_strLogDB_DatabaseName, GConfig::m_strLogDB_UserName, GConfig::m_strLogDB_Password)));
	}
}

bool GServer::InitDB()
{
	SetDsnFactory();

	mdb::MDatabaseDesc GamdDBDesc = SDsnFactory::GetInstance().Get()->GetGameDSN();
	
	if (gsys.pDBManager->Connect(GamdDBDesc))
	{
		mlog("Create Sync Database success.\n");
	}
	else
	{
		mlog3 ("Create Sync Database failed.\n");
		return false;
	}

	if (gsys.pDBManager->InitAsyncDB())
	{
		mlog ("Create Async Database success.\n");
	}
	else
	{
		mlog3 ("Create Async Database failed.\n");
		return false;
	}

	return true;
}

bool GServer::InitInfo()
{
	if (InitDependencyInfo() == false)
	{
		SetServerInitResult(SERVERINIT_FAILED_DEPENDENCYFILE_LOAD);
		return false;
	}

	if (LoadInfoFiles() == false)
	{
		SetServerInitResult(SERVERINIT_FAILED_DATAFILE_LOAD);
		return false;
	}

	return true;
}

bool GServer::InitDependencyInfo()
{
	GEntityPlayer::GetPlayerCRTLogger().Init();

	return true;
}

void GServer::DeInitInfo()
{
	// instances
//	gmgr.pFieldMgr->Clear();
//	gmgr.pNPCMgr->Clear();
//	gmgr.pPlayerObjectManager->Clear();
	
	// info
	gmgr.pItemManager->Clear();
	gmgr.pTalentInfoMgr->Clear();
	gmgr.pBuffInfoMgr->Clear();
	gmgr.pNPCInfoMgr->Clear();
	gmgr.pCondtionsInfoMgr->Clear();
	gmgr.pItemConditionsInfoMgr->Clear();
	gmgr.pDialogInfoMgr->Clear();
	//gmgr.pFieldInfoMgr->Clear();
	gmgr.pQuestInfoMgr->Clear();
	gmgr.pChallengerQuestMgr->Clear();
	gmgr.pSoulBindingInfoMgr->Clear();
	gmgr.pNPCShopInfoMgr->Clear();
	gmgr.pRecipeInfoMgr->Clear();
	gmgr.pCraftInfoMgr->Clear();
	gmgr.pInnRoomInfoMgr->Clear();
	gmgr.pFactionInfoMgr->Clear();
	gmgr.pFactionRelationInfoMgr->Clear();
	gmgr.pCutsceneInfoMgr->Clear();
	gmgr.pEmblemInfoMgr->Clear();
	gmgr.pQPEventInfoMgr->Clear();
	gmgr.pPresetInfoMgr->Clear();
}

bool GServer::Reload()
{
	//mlog1("=== Reloading Start ===\n");

	//// 접속자를 끊는다.
	//for each(pair<MUID, GPlayerObject*> itor in gmgr.pPlayerObjectManager->GetObjects())
	//{
	//	GPlayerObject* pPlayer = itor.second;
	//	MUID uid = pPlayer->GetUID();
	//	Disconnect(uid);
	//}
	//
	//DeInitInfo();
	//InitInfo();
	//mlog1("=== Reloading Finish ===\n");
	DeInitInfo();
	InitInfo();

	return true;
}


bool GServer::Update(float fDelta)
{
	if (gmgr.pPlayerObjectManager)
		gmgr.pPlayerObjectManager->UpdatePlayerAcceptorManager();

	PFC_B("GServer::Update - player object manager");
	PFI_B_LINE(30, "GServer::Update - player object manager");
	gmgr.pPlayerObjectManager->Update(fDelta);
	PFI_E(30);
	PFC_E;

	PFC_B("GServer::Update - net Run");
	PFI_B_LINE(31, "GServer::Update - net Run");
	m_pServableChecker->Update(fDelta);

	// 커맨드 펌프는 틱에 상관없이 처리한다.
	m_pMasterServerFacade->RunNetwork();
	m_pAppServerFacade->RunNetwork(fDelta);
	if (m_pNetServer) m_pNetServer->Run();
	PFI_E(31);
	PFC_E;

	m_pDBCacheRegularFlush->Update(fDelta);

	PFC_B("GServer::Update - GDBManager:Update");
	PFI_B_LINE(32, "GServer::Update - GDBManager:Update");
	// DB 요청 처리는 커맨드 펌프가 끝난 다음에 한다.
	gsys.pDBManager->Update();
	PFI_E(32);
	PFC_E;

	{
		PFC_BLOCK("GServer::Update - 틱대기(Sleep10)");
		PFI_BLOCK(33, "GServer::Update - 틱대기(Sleep10)");

		int nTick = DEFAULT_GAME_TICK;

		if (IsRunForTest())
		{
			nTick = GConst::GAME_TICK;
			if (nTick <= 0) nTick = DEFAULT_GAME_TICK;
		}

		static float fElapsed = 0;
		fElapsed += fDelta;

		if (fElapsed < (1.0f / nTick)) 
		{
			if (GConst::SERVER_SLEEP)
			{
				Sleep(10);
				return true;
			}
		}
		fDelta = fElapsed;
		fElapsed = 0.0f;

		m_ServerInfo.nFPS = m_pFrameCounter->Update();
	}

	if (gsys.pWorld)
	{
		PFC_BLOCK("GServer::Update - pWorld->Update");
		gsys.pWorld->Update(fDelta);
	}


	if (m_pServerCloser)
	{		
		PFC_BLOCK("GServer::Update - ServerCloser->Update");

		// 종료처리가 완료되면, 실패를 반환한다.
		if (!m_pServerCloser->Update())
		{
			return false;
		}
	}


	if (m_pAsyncTaskManager)
	{
		PFC_BLOCK("GServer::Update - AsyncTaskManager->Update");
		m_pAsyncTaskManager->Update(gsys.pSystem->GetNowTime());
	}

	{
		PFC_BLOCK("GServer::Update - ReserveCmdManager->Update()");
		gmgr.pReserveCmdManager->Update();
	}
	

	{
		PFC_BLOCK("GServer::Update - UpdateForDebug");
		UpdateForDebug(fDelta);
	}
	
	{
		PFC_BLOCK("GServer::Update - ServerStatusUpdater");
		m_pServerStatusUpdater->Update(fDelta);
	}

	{
		PFC_BLOCK("GServer::Update - FieldStatusUpdater");
		m_pFieldStatusUpdater->Update(fDelta);
	}
	
	{
		PFC_BLOCK("GServer::Update - CommandProfileController->Update()");
		if (m_pCommandProfileController && m_pCommandProfileController->IsAutoSave())
			m_pCommandProfileController->Update(fDelta);

		if (m_pCommandProfileController_ForMasterServer && m_pCommandProfileController_ForMasterServer->IsAutoSave())
			m_pCommandProfileController_ForMasterServer->Update(fDelta);

		if (m_pCommandProfileController_ForAppServer && m_pCommandProfileController_ForAppServer->IsAutoSave())
			m_pCommandProfileController_ForAppServer->Update(fDelta);
	}
	
	{
		gsys.pTestSystem->Update(fDelta);
	}

	// 메모리 체크
#ifdef _DEBUG
	_ASSERT(_CrtCheckMemory());
#endif

	AI_PFI_FRAME_UPDATE();	

	return true;
}

void GServer::CloseServer(void)
{
	m_pServerCloser->CloseServer();
}

/// 서버에서 쓰는 각종 셋팅 값들을 표시한다.
void GServer::WriteServerInfoLog()
{
	mlog("Server Created. (WorldID=%d , ServerID=%d)\n", GConfig::m_nMyWorldID, GConfig::m_nMyServerID);
}


void GServer::UpdateServerInfoView()
{
	m_ServerInfo.nLinkCount = m_pNetServer->GetLinkCount();

	m_ServerInfo.nNowPlayer = gmgr.pPlayerObjectManager->GetPlayersCount();

	m_ServerInfo.nMaxPlayer = max(m_ServerInfo.nMaxPlayer, m_ServerInfo.nNowPlayer);

	GNetServer* pNetServer = static_cast<GNetServer*>(m_pNetServer);
	m_ServerInfo.nRecvCPS = pNetServer->GetRecvCPS();
	m_ServerInfo.nSendCPS = pNetServer->GetSendCPS();
	m_ServerInfo.nLocalCPS = pNetServer->GetLocalCPS();

	if (m_pAsyncTaskManager)
		m_ServerInfo.nAsyncTaskCount = (uint32)m_pAsyncTaskManager->GetTaskCount();

	m_ServerInfo.nAsyncDBTask = gsys.pDBManager->GetAsyncQSize();
	m_ServerInfo.nAsyncDBTotalTask = gsys.pDBManager->GetGameDBQCount() + gsys.pDBManager->GetGameDBSelectCharQCount() + gsys.pDBManager->GetLogDBQCount();
	m_ServerInfo.nMaxAsyncDBTask = max(m_ServerInfo.nMaxAsyncDBTask , m_ServerInfo.nAsyncDBTask);

	m_pNetServer->GetSecondTraffic(m_ServerInfo.nRecvBPS, m_ServerInfo.nSendBPS);
	m_pNetServer->GetTotalTraffic(m_ServerInfo.nTotalRecvBytes, m_ServerInfo.nTotalSendBytes);

}

void GServer::Disconnect(const MUID& uidPlayer)
{
	GPlayerObject* pObj = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pObj == NULL) return;

	pObj->SetForceDisconnect();

	if (m_pNetServer)
		m_pNetServer->Disconnect(pObj->GetUID());
}

void GServer::HardDisconnect(const MUID& uidPlayer)
{
	GPlayerObject* pObj = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pObj == NULL) return;

	pObj->SetForceDisconnect();

	if (m_pNetServer)
		m_pNetServer->HardDisconnect(pObj->GetUID());
}

void GServer::UpdateForDebug( float fDelta )
{
	PFI_BLOCK(35, "GServer::UpdateForDebug");

	// 서버 정보 업데이트
	if (m_pServerInfoViewRglt->IsReady(fDelta))
	{
		UpdateServerInfoView();
	}

	// 테스트 용도
	BirdM().Update();
}

bool GServer::LoadInfoFiles()
{
	GServerResLoader resLoader;
	GServerResLoader::Result result = resLoader.LoadAll(&gmgr, L"./");
	
	if (result.bSuccess == false)
	{
		mlog3("%s", MLocale::ConvUTF16ToAnsi(result.strErrorLog.c_str()).c_str());
		return false;
	}

	// 환경 초기화 - 필드 정보를 읽은 후에 초기화해야 함
	GEnvParam envParam;
	envParam.nStartHour[TIME_DAWN] = GConst::CONST_ENV_TIME_DAWN_START_HOUR;
	envParam.nStartHour[TIME_DAYTIME] = GConst::CONST_ENV_TIME_DAYTIME_START_HOUR;
	envParam.nStartHour[TIME_SUNSET] = GConst::CONST_ENV_TIME_SUNSET_START_HOUR;
	envParam.nStartHour[TIME_NIGHT] = GConst::CONST_ENV_TIME_NIGHT_START_HOUR;

	envParam.nLocalTimePerGameTimeOneHour = GConst::CONST_LOCALTIME_PER_GAMETIME_ONE_HOUR;
	envParam.nUpdateTickSec = GConst::CONST_ENV_TIME_UPDATE_TICK_SEC;


	if (gmgr.pEnvManager->Create(envParam) == false)
	{
		_ASSERT(0);
		mlog3("Failed Loading Env Info : const.lua\n");
		return false;
	}

	gmgr.pBattleArenaMgr->CreateTestRules();

	// 금칙어 목록 로드
	if (false == GetStringFilter()->LoadFromFile(FILENAME_ABUSE))
	{
		mlog3("Failed! Load String Filter.\n");
		return false;
	}

	// 이름 필터링 데이터 로드
	if (!GetNameWordFilter()->LoadFromFile(FILENAME_NAME_ILLEGALWORDS, FILENAME_NAME_ALLOWEDCHARS))
	{
		SetServerInitResult(SERVERINIT_FAILED_DATAFILE_LOAD);
		mlog3("Failed! Load name_illegalwords.txt, name_allowedchars.txt\n");
		return false;
	}

	// 게임단어 목록 로드
	if (false == GetGameWordsFilter()->LoadFromFile(FILENAME_GAMEWORDS))
	{
		mlog3("Failed! Load GameWords.txt.\n");
		return false;
	}

	// 후처리
	GAutoPartyMgr::Cooking();

	return true;
}

void GServer::FrameCounterClear()
{
	m_pFrameCounter->Clear();
}

bool GServer::CreateNetwork()
{
	minet::MNetServerDesc server_desc;
	server_desc.bHeartBeat = GConfig::m_bEnableHeartBeat;
	server_desc.nHeartBeatTickTime = GConfig::m_nHeartBeatTickTime;
	server_desc.nHeartBeatTimeoutTime = GConfig::m_nHeartBeatTimeout;

	if (GConfig::m_dwPacketDelayTime > 0)
	{
		server_desc.bDelayTest = true;
		server_desc.nTestDelayTime = GConfig::m_dwPacketDelayTime;
	}

	if (GConfig::m_nCommandRecordingType != COMMANDREC_NONE)
	{
		m_pNetServer = new GRecordModeServer(server_desc);
	}
	else
	{
		m_pNetServer = new GNetServer(server_desc);
		m_pNetServer->AttachCommandProfiler(new MCommandProfiler());
	}
	
	m_pCommandCenter = new GCommandCenter(m_pNetServer);
	gsys.pCommandCenter = m_pCommandCenter;	// 일단..
	
	if (m_pMasterServerFacade->CreateNetwork(GConfig::m_strMasterServerIP.c_str(), GConfig::m_nMasterServerPort) == false)
	{
		return false;
	}
	m_pMasterServerFacade->GetNetClient()->AttachCommandProfiler(new MCommandProfiler());

	//
	// AppServerFacade의 경우 MasterServer에게 IP와 Port를 받기전까지 CreateNetwork()를 보류한다.
	//

	MUID uidSeed = MUID(GConfig::m_nMyServerID * SERVER_UID_SEED_POWER, 0);
	wstring strMyNetworkCardIP = GetNetworkCardMgr()->GetIP(GConfig::m_nMyNetworkCardID);
	if (m_pNetServer->Create(GConfig::m_nPort, false, uidSeed, GConfig::m_nSocketPoolSize, GConfig::m_nSendPendingLimitCount, MLocale::ConvUTF16ToAnsi(strMyNetworkCardIP.c_str()).c_str()) == false)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	/// Command Profiler
	// Client - GameServer 간
	m_pCommandProfileController = new SCommandProfileController(m_pNetServer
		, GConfig::m_strCommandProfiler_LogFolder
		, GConfig::m_strCommandProfiler_LogName
		, GConfig::m_bCommandProfiler_AutoSave
		, GConfig::m_nCommandProfiler_AutoSaveTickMinute);

	// GameServer - MasterServer 간
	m_pCommandProfileController_ForMasterServer = new SCommandProfileController(m_pMasterServerFacade->GetNetClient()
		, GConfig::m_strCommandProfiler_LogFolder
		, GConfig::m_strCommandProfiler_LogName_ForMasterServer
		, GConfig::m_bCommandProfiler_AutoSave
		, GConfig::m_nCommandProfiler_AutoSaveTickMinute);

	//
	// AppServerFacade의 경우 MasterServer에게 정보를 받기 전까지 프로파일러 생성을 보류한다.
	//

	if (GConfig::m_bCommandProfiler_AutoStart)
		StartCommandProfile();

	return true;
}

void GServer::DestroyNetwork()
{
	if (m_pNetServer)
	{
		m_pNetServer->Destroy();
	}

	SAFE_DELETE(m_pCommandProfileController_ForAppServer);
	SAFE_DELETE(m_pCommandProfileController_ForMasterServer);
	SAFE_DELETE(m_pCommandProfileController);
	SAFE_DELETE(m_pCommandCenter);
	SAFE_DELETE(m_pNetServer);

	m_pMasterServerFacade->DestroyNetwork();
	m_pAppServerFacade->DestroyNetwork();	

	gsys.pCommandCenter = NULL;
}

void GServer::ReleaseDB()
{
	gsys.pDBManager->Disconnect();
	gsys.pDBManager->Release();
}

bool GServer::ReAllocLinkUID( MUID uidOrigin, MUID uidReAlloc )
{
	return m_pNetServer->ReAllocLinkUID(uidOrigin, uidReAlloc);
}

void GServer::StartCommandProfile()
{
	if (m_pCommandProfileController->IsStarted())
	{
		mlog("!Notice! Command Profiler Already Started!\n");
	}
	else
	{
		mlog("Command Profiler Start!\n");
		m_pCommandProfileController->Start();
	}

	if (m_pCommandProfileController_ForMasterServer->IsStarted())
	{
		mlog("!Notice! Command Profiler(For MasterServer) Already Started!\n");
	}
	else
	{
		mlog("Command Profiler(For MasterServer) Start!\n");
		m_pCommandProfileController_ForMasterServer->Start();
	}

	if (m_pCommandProfileController_ForAppServer)
	{
		if (m_pCommandProfileController_ForAppServer->IsStarted())
		{
			mlog("!Notice! Command Profiler(For AppServer) Already Started!\n");
		}
		else
		{
			mlog("Command Profiler(For AppServer) Start!\n");
			m_pCommandProfileController_ForAppServer->Start();
		}
	}
}

void GServer::DumpCommandProfile()
{
	mlog("Dump Command Profile...\n");
	m_pCommandProfileController->Save(false);
	m_pCommandProfileController_ForMasterServer->Save(false);

	if (m_pCommandProfileController_ForAppServer)
		m_pCommandProfileController_ForAppServer->Save(false);
}

void GServer::CreateCommandProfileForAppServer(MCommandCommunicator* pCC)
{
	m_pCommandProfileController_ForAppServer = new SCommandProfileController(pCC
		, GConfig::m_strCommandProfiler_LogFolder
		, GConfig::m_strCommandProfiler_LogName_ForAppServer
		, GConfig::m_bCommandProfiler_AutoSave
		, GConfig::m_nCommandProfiler_AutoSaveTickMinute);

	if (GConfig::m_bCommandProfiler_AutoStart)
		m_pCommandProfileController_ForAppServer->Start();
}

void GServer::FinishCommandProfile()
{
	mlog("Command Profiler Finish!\n");
	m_pCommandProfileController->Save(true);
	m_pCommandProfileController_ForMasterServer->Save(true);

	if (m_pCommandProfileController_ForAppServer)
		m_pCommandProfileController_ForAppServer->Save(true);
}

void GServer::Dump(wstring strID)
{
	gsys.pServerDumper->Dump(strID);
}

void GServer::Init_UpdateDB_Infos()
{
	gsys.pDBManager->UpdateDB_FromItemInfo();
	gsys.pDBManager->UpdateDB_FromQeustInfo();
}

bool GServer::IsServerClosing()
{
	return m_pServerCloser->IsRunServerCloser();
}

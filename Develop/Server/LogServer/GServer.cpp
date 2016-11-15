#include "stdafx.h"
#include "GServer.h"
#include "GDef.h"
#include "GConfig.h"
#include "GCommandTable.h"
#include "GCommandCenter.h"
#include "GDBManager.h"
#include "GGameServerMgr.h"
#include "GApplication.h"
#include "GGameServer.h"
#include "MStatisticsProfiler.h" // DNC

//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////

GServer::GServer()
: m_bCreated(false)
{
	m_pServerInfoViewRglt = new MRegulator(APP_SERVERVIEW_UPDATE_TICK);	
	
	minet::MNetServerDesc server_desc;
	server_desc.bHeartBeat = true;
	server_desc.nHeartBeatTickTime = 60000;		// 60초마다 heartbeat
	server_desc.nHeartBeatTimeoutTime = 600000;	// 10분동안 아무 응답이 없으면 접속 끊음
	
	m_pNetServer = new GNetServer(server_desc);
	m_pCommandCenter = new GCommandCenter(*m_pNetServer);
	gsys.pCommandCenter = m_pCommandCenter;	// 일단..
}

GServer::~GServer()
{
	SAFE_DELETE(m_pServerInfoViewRglt);

	SAFE_DELETE(m_pCommandCenter);
	SAFE_DELETE(m_pNetServer);
	gsys.pCommandCenter = NULL;
}

bool GServer::Create()
{
	/// 네트워크 모듈 생성
	if (m_pNetServer->Create(GConfig::m_nPort) == false)
	{
		return false;
	}
	
	if (InitDB() == false)
	{
		return false;
	}

	m_bCreated = true;

	WriteServerInfoLog();

	return true;
}

void GServer::Destroy()
{
	gsys.pDBManager->Disconnect();

	m_pNetServer->Destroy();

	m_bCreated = false;
}


bool GServer::InitDB()
{
	string str = gsys.pDBManager->BuildDSNString(GConfig::m_szDB_DNS, GConfig::m_szDB_UserName, GConfig::m_szDB_Password);

	if (gsys.pDBManager->Connect(str))
	{
		mlog("DBMS connected\n");
	}
	else
	{
		mlog("Can't Connect To DBMS\n");
		return false;
	}

	return true;
}

class FrameCounter
{
private:
    MTimeValue m_nTime;
    MTimeValue m_nFrameCounter;
    MTimeValue m_nCurrFrame;
    MTimeValue m_nLastTime;
public:
    FrameCounter() : m_nTime(0), m_nFrameCounter(0), m_nCurrFrame(0) { m_nLastTime = GetNowTime(); }
    int Update()
    {
        ++m_nFrameCounter;
        MTimeValue nNowTime = GetNowTime();

        m_nTime += (nNowTime - m_nLastTime);
        m_nLastTime = nNowTime;

        if (m_nTime >= 1000)
        {
            m_nCurrFrame = m_nFrameCounter;
            m_nFrameCounter = 0;
            m_nTime = 0;
        }
    
		return m_nCurrFrame.GetValue();
    }
	
	uint32 GetNowTime()
	{
		if (gsys.pSystem)
			return gsys.pSystem->GetNowTime();

		return timeGetTime();
	}
} g_FrameCounter;


void GServer::Update(float fDelta)
{
	PFI_B_LINE(31, "GServer::Update - GNetServer:Run");
	// 커맨드 펌프는 틱에 상관없이 처리한다.
	m_pNetServer->Run();
	PFI_E(31);

	{
		PFI_BLOCK(32, "GServer::Update - 틱대기(Sleep10)");

		int nTick = DEFAULT_GAME_TICK;

		static float fElapsed = 0;
		fElapsed += fDelta;

		if (fElapsed < (1.0f / nTick)) 
		{
			Sleep(10);
			return;
		}
		fDelta = fElapsed;
		fElapsed = 0.0f;

		m_ServerInfo.nFPS = g_FrameCounter.Update();
	}

	UpdateLog();
	UpdateForDebug(fDelta);
}

// 서버에서 쓰는 각종 셋팅 값들을 표시한다.
void GServer::WriteServerInfoLog()
{
	mlog("Server Created.\n");
}


void GServer::UpdateServerInfoView()
{
	gmgr.pGameServerMgr->Lock();
	m_ServerInfo.nNowPlayer = gmgr.pGameServerMgr->GetSize();
	gmgr.pGameServerMgr->Unlock();

	m_ServerInfo.nMaxPlayer = max(m_ServerInfo.nMaxPlayer, m_ServerInfo.nNowPlayer);

	m_ServerInfo.nRecvCPS = m_pNetServer->GetRecvCPS();
	m_ServerInfo.nSendCPS = m_pNetServer->GetSendCPS();
	m_ServerInfo.nLocalCPS = m_pNetServer->GetLocalCPS();

	m_pNetServer->GetSecondTraffic(m_ServerInfo.nRecvBPS, m_ServerInfo.nSendBPS);

	// UI 업데이트
	GApplication::GetInstancePtr()->SetServerInfoView(m_ServerInfo);
}



void GServer::Disconnect(MUID& uidGameServer)
{
	GGameServer* pGameServer = gmgr.pGameServerMgr->Get(uidGameServer);
	if (pGameServer == NULL) return;

	m_pNetServer->Disconnect(pGameServer->GetUID());
}

void GServer::UpdateForDebug( float fDelta )
{
	PFI_BLOCK(35, "GServer::UpdateForDebug");

	// 서버 정보 업데이트
	if (m_pServerInfoViewRglt->IsReady(fDelta))
	{
		UpdateServerInfoView();	
	}
}

void GServer::UpdateLog()
{
	vector<GLog*> vecLog;
	gmgr.pGameServerMgr->PickUpLog(MAX_EXECUTE_QUERY, vecLog);

	for each (GLog* pLog in vecLog)
	{
		string strQuery = pLog->MakeQuery();		
		gsys.pDBManager->Execute(strQuery.c_str());
		SAFE_DELETE(pLog);
	}	
}

MCommand* MakeNewCommand(int nCmdID, MUID TargetUID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	if (gsys.pServer == NULL ||
		!gsys.pServer->IsCreated())
	{
		va_list marker;
		MCommandParameter* pParam = pCmdParam;

		va_start( marker, pCmdParam );
		for (int i = 0; i < nParamCount; i++)
		{
			SAFE_DELETE(pParam);
			pParam = va_arg( marker, MCommandParameter*);
		}
		va_end( marker );
		return NULL;
	}
	MCommand* pNewCommand = gsys.pCommandCenter->NewCommand(nCmdID, TargetUID);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */

	for (int i = 0; i < nParamCount; i++)
	{
		pNewCommand->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	return pNewCommand;
}

MCommand* MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	if (gsys.pServer == NULL ||
		!gsys.pServer->IsCreated())
	{
		va_list marker;
		MCommandParameter* pParam = pCmdParam;

		va_start( marker, pCmdParam );
		for (int i = 0; i < nParamCount; i++)
		{
			SAFE_DELETE(pParam);
			pParam = va_arg( marker, MCommandParameter*);
		}
		va_end( marker );
		return NULL;
	}

	MCommand* pNewCommand = gsys.pCommandCenter->NewCommand(nCmdID, 0);

	va_list marker;
	MCommandParameter* pParam = pCmdParam;

	va_start( marker, pCmdParam );     /* Initialize variable arguments. */
	for (int i = 0; i < nParamCount; i++)
	{
		pNewCommand->AddParameter(pParam);
		pParam = va_arg( marker, MCommandParameter*);
	}
	va_end( marker );              /* Reset variable arguments.      */

	return pNewCommand;
}

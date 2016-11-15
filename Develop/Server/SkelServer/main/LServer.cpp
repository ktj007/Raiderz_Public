#include "stdafx.h"
#include "LServer.h"
#include "LDef.h"
#include "LConfig.h"
#include "LCommandTable.h"
#include "LCommandCenter.h"
#include "LCommObjectManager.h"
#include "LCommObject.h"
#include "SFilePath.h"

//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////

LServer::LServer()
: m_bCreated(false)
, m_pCommandCenter(NULL)
, m_pNetServer(NULL)
{
	m_pServerInfoViewRglt = new MRegulator(APP_SERVERVIEW_UPDATE_TICK);	
	
}

LServer::~LServer()
{
	SAFE_DELETE(m_pServerInfoViewRglt);
}

bool LServer::CreateNetwork()
{
	minet::MNetServerDesc server_desc;
	server_desc.bHeartBeat = true;
	server_desc.nHeartBeatTickTime = 60000;		// 60초마다 heartbeat
	server_desc.nHeartBeatTimeoutTime = 600000;	// 10분동안 아무 응답이 없으면 접속 끊음

	m_pNetServer = new LNetServer(server_desc);
	m_pCommandCenter = new LCommandCenter(m_pNetServer, gmgr.pCommObjectManager);
	gsys.pCommandCenter = m_pCommandCenter;	// 일단..

	/// 네트워크 모듈 생성
	if (m_pNetServer->Create(LConfig::m_nPort) == false)
	{
		return false;
	}

	mlog("Server Socket Opened. (Port = %d)\n", LConfig::m_nPort);

	return true;
}

void LServer::DestroyNetwork()
{
	m_pNetServer->Destroy();

	SAFE_DELETE(m_pCommandCenter);
	SAFE_DELETE(m_pNetServer);
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

	m_bCreated = true;

	WriteServerInfoLog();

	return true;
}

void LServer::Destroy()
{
	DestroyNetwork();

	m_bCreated = false;

	mlog("Server Closed\n");
}


bool LServer::InitDB()
{
	//string str = gsys.pDBManager->BuildDSNString(GConfig::m_szDB_DNS, GConfig::m_szDB_UserName, GConfig::m_szDB_Password);

	//if (gsys.pDBManager->Connect(str))
	//{
	//	mlog("DBMS connected\n");
	//}
	//else
	//{
	//	mlog("Can't Connect To DBMS\n");
	//	return false;
	//}

	return true;
}

bool LServer::InitInfo()
{
	if (InitDB() == false)
	{
		_ASSERT(0);
		mlog3("Failed InitDB\n");
		return false;
	}

	// 필드 정보
	string strSystemPath = LConfig::m_strSystemPath;

	string strFieldListFileName = SFilePath::MakeFieldList(strSystemPath.c_str());
	string strZoneListFileName = SFilePath::MakeZoneList(strSystemPath.c_str());

	string strZonePath = LConfig::m_strZonePath;
	string strFieldPath = LConfig::m_strFieldPath;


	return true;
}

void LServer::DeInitInfo()
{
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
		return timeGetTime();
	}
} g_FrameCounter;


void LServer::Update(float fDelta)
{
	// 커맨드 펌프는 틱에 상관없이 처리한다.
	m_pNetServer->Run();

	{
		int nTick = DEFAULT_UPDATE_TICK;

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

	UpdateForDebug(fDelta);
}

// 서버에서 쓰는 각종 셋팅 값들을 표시한다.
void LServer::WriteServerInfoLog()
{
	mlog("Server Created.\n");
}


void LServer::UpdateServerInfoView()
{
	gmgr.pCommObjectManager->Lock();
	m_ServerInfo.nCurrGameServerCount = gmgr.pCommObjectManager->GetClientsCount();
	gmgr.pCommObjectManager->Unlock();

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


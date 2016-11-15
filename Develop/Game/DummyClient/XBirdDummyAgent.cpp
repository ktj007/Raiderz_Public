#include "stdafx.h"
#include "XBirdDummyAgent.h"
#include "XBirdClient.h"
#include "XDummyBot.h"
#include "CSNet.h"
#include "XFieldInfo.h"
#include "CCommandResultTable.h"
#include "RCollisionTree.h"
#include "XDummyCollision.h"
#include "MiNetHelper.h"
#include "XStrings.h"
#include "XDummyHelper.h"
#include "XDummyMaster.h"
#include "XNet.h"
#include "MServerTcpSocket.h"


XBirdDummyAgent::XBirdDummyAgent()
: m_bDisconnectForTest(false)
, m_eConState(DISCONNECTING)
, m_pConnectionManager(NULL)
{
	m_pClient = NULL;

	m_nID = 0;
	m_vCenter = vec3::ZERO;
	m_nLastCommandTime = 0;
	m_bCreated = false;
	m_nLastCheckConnection = 0;
	m_nConnCount = 0;

	m_eStatus = DAS_NOTCONNECTED;

	m_nLoginFieldID = 0;
	m_nPreStayFieldID = 0;
	m_bDirectLoginToGameServer = false;

	m_bIsFirstAgent = false;
}	

XBirdDummyAgent::~XBirdDummyAgent()
{
	m_eConState = DISCONNECTING;
	SAFE_DELETE(m_pConnectionManager);

	if (m_pClient)
	{
		SAFE_DELETE(m_pClient);
	}

	for each (XDummyBot* const bot in m_bots)
	{
		delete bot;
	}
	m_bots.clear();
}

MCommandResult XBirdDummyAgent::OnCommand(XBirdClient* pClient, MCommand* pCmd)
{
	if (m_pClient == NULL) return CR_FALSE;
	m_nLastCommandTime = XGetNowTime();

	switch(pCmd->GetID())
	{
	case MC_LOCAL_NET_ONCONNECT:
		{
			SetStatusRunning();
			m_nConnCount++;
		}
		break;

	case MC_LOCAL_NET_ONDISCONNECT:
		{
			if (m_pConnectionManager->OnDisconnect() == true)
			{
				return CR_TRUE;
			}

			if (CONN_STATUS_DISCONNECT == m_pConnectionManager->GetStatus())
			{
				USES_CONVERSION_EX;

				SetStatusNotConnected();
				
				if (!IsDisconnectForTest())
					mlog3("XBirdDummyAgent::OnCommand(), MC_LOCAL_NET_ONDISCONNECT, id=%s, field=%d\n", W2A_EX(GetLoginID(), 0), GetLoginFieldID());
				
				SetDisconnectForTest(false);
			}
		}
		break;
	case MC_COMM_RESPONSE_LOGIN_GAME_SERVER:
		{
			CommonResponseLogin(pCmd);
		}
		break;
	case MC_COMM_RESPONSE_MOVE_GAME_SERVER:
		{
			CommonResponseLogin(pCmd);
		}
		break;
	case MC_COMM_MOVE_GAME_SERVER:
		{
			OnCommMoveGameServer(pCmd);
		}
		break;
	case MC_COMM_RESPONSE_LOGIN:
		{
			int nResult;
			pCmd->GetParameter(&nResult, 0, MPT_INT);

			if (nResult == CR_SUCCESS)
				m_pConnectionManager->OnConnectedToLoginServer();
		}
		break;
	}

	for (size_t i = 0; i < m_vecHandler.size(); i++)
	{
		if (m_vecHandler[i]->OnCommand(this, pCmd) == CR_TRUE) return CR_TRUE;
	}

	for each (XDummyBot* const bot in m_bots)
	{
		assert(NULL!=bot);
		if (CR_TRUE == bot->OnCommand(this, pCmd))
		{
			return CR_TRUE;
		}
	}

	for each (XDummyBot* const bot in m_deletedBots)
	{
		assert(NULL!=bot);
		BOTS::iterator it = m_bots.begin();
		for (; it != m_bots.end(); ++it)
		{
			XDummyBot* const curBot(*it);
			assert(NULL!=curBot);
			if (curBot == bot)
			{
				m_bots.erase(it);
				break;
			}
		}
		delete bot;
	}
	m_deletedBots.clear();

	return CR_FALSE;
}

void XBirdDummyAgent::Run(float fDelta)
{
	if (!m_bCreated) return;

	if (m_pClient)
	{
		m_pClient->Run();
	}

	for each (XDummyBot* const bot in m_bots)
	{
		assert(NULL!=bot);
		bot->Run(fDelta);
	}
	///< 게임서버를 이동할 때 이 부분에서 다시 연결을 시도해버리는 바람에 테스트가 정상동작 하지 않는 문제가 있습니다. - aibeast
	//CheckConnection();
}

void XBirdDummyAgent::Create(const XDummyAgentInfo& agentInfo, int nAgentID, const wchar_t* szLoginID, const wchar_t* szGameServer, const wchar_t* szLoginServer, XBirdDummyAgentMgr* pAgentMgr)
{
	m_pClient = new XBirdClient(this);
	m_pConnectionManager = new XConnectionManager(new XBirdClientWrapper(m_pClient, this));
	m_pAgentMgr = pAgentMgr;

	m_strName = agentInfo.strName;
	m_nID = nAgentID;
	m_strGameServer = szGameServer;
	m_strLoginServer = szLoginServer;

	m_vCenter.x = XMath::RandomNumber(agentInfo.vCenter.x - agentInfo.fRadius * 0.5f, agentInfo.vCenter.x + agentInfo.fRadius * 0.5f);
	m_vCenter.y = XMath::RandomNumber(agentInfo.vCenter.y - agentInfo.fRadius * 0.5f, agentInfo.vCenter.y + agentInfo.fRadius * 0.5f);
	m_vCenter.z = agentInfo.vCenter.z;

	m_nLoginFieldID = agentInfo.nFieldID;
	m_pCollision = agentInfo.pCollision;

	//m_strLoginID = szLoginID;
	//wchar_t szID[64];
	//_itow_s(nAgentID, szID, 64, 10);
	//m_strLoginID += szID;

	m_strLoginID = XDummyHelper::MakeCharacterName(szLoginID, nAgentID);

	m_bCreated = true;
}

void XBirdDummyAgent::Connect(const wchar_t* const szIP, int nPort)
{
	assert(NULL!=szIP);

	char szAnsiIP[32] = {0,};
	wsprintfA(szAnsiIP, "%S", szIP); ///< 유니코드를 ANSI로 변환

	char szPort[128] = {0,};
	_itoa_s(nPort, szPort, sizeof(szPort), 10);
	
	assert(NULL!=m_pAgentMgr);
	m_eConState = REQUEST_CONNECT;
	if (false == m_pAgentMgr->Connect(szAnsiIP, szPort, *this))
	{
		assert(NULL!=m_pAgentMgr);
		m_eConState = DISCONNECTING_REQUEST_FAILED;
	}
}

void XBirdDummyAgent::AddCommandHandler( XDummyHandler* pHandler )
{
	if (pHandler == NULL) return;
	m_vecHandler.push_back(pHandler);
}

void XBirdDummyAgent::CheckConnection()
{
	if (m_nLastCheckConnection == 0) m_nLastCheckConnection = timeGetTime();
	if (timeGetTime() - m_nLastCheckConnection <= 1000) return;
	m_nLastCheckConnection = timeGetTime();

	assert(NULL!=m_pClient);
	if (CONN_STATUS_DISCONNECT == m_eConState || DISCONNECTING_REQUEST_FAILED == m_eConState)
	{
		USES_CONVERSION_EX;
		Connect(m_strGameServer.c_str(), DEFAULT_GAME_SERVER_PORT);
	}
}

void XBirdDummyAgent::AddBot( XDummyBot* pBot )
{
	assert(NULL!=pBot);
	m_bots.push_back(pBot);
}

void XBirdDummyAgent::DelBot(XDummyBot* const pBot)
{
	assert(NULL!=pBot);
	BOTS::iterator it = m_bots.begin();
	for (; it != m_bots.end(); ++it)
	{
		XDummyBot* const curBot(*it);
		assert(NULL!=curBot);
		if (curBot == pBot)
		{
			m_deletedBots.push_back(pBot);
			return;
		}
	}
}

bool XBirdDummyAgent::IsStayTrialField(void)
{
	CSFieldList& fieldList = info.field->GetFieldList();
	CSFieldList::iterator it = fieldList.find(m_nLoginFieldID);
	if (it == fieldList.end())		return false;

	CSFieldFileInfo* pFieldFieldInfo = it->second;
	if (pFieldFieldInfo == NULL)	return false;

	return pFieldFieldInfo->bDynamic;
}

void XBirdDummyAgent::PostConnectToGameServer()
{
	const int nPort = DEFAULT_GAME_SERVER_PORT;
	Connect(m_strGameServer.c_str(), nPort);
}

void XBirdDummyAgent::PostConnectToLoginServer()
{
	if (m_bDirectLoginToGameServer)
	{
		PostConnectToGameServer();
		return;
	}

	int nPort = DEFAULT_LOGIN_SERVER_PORT;
	Connect(m_strLoginServer.c_str(), nPort);
}

void XBirdDummyAgent::CommonResponseLogin( MCommand* pCommand)
{
	int nResult = 0;
	MUID uidAlloc;
	unsigned short nUIID = 0;

	if( !pCommand->GetParameter(&nResult,	0, MPT_INT) )	return;
	if( !pCommand->GetParameter(&uidAlloc,	1, MPT_UID) )	return;
	if( !pCommand->GetParameter(&nUIID,		2, MPT_USHORT) )	return;

	if (nResult == CR_SUCCESS)
	{
		m_pConnectionManager->OnConnectedToGameServer();

		m_UID = uidAlloc;
		m_nUIID = nUIID;

		m_pConnectionManager->PostGameStart();
	}
	else
	{
		const TCHAR* const resultMsg = XCmdResultStr(nResult);
		if (NULL != resultMsg) {
			wchar_t szLog[1024] = L"";
			swprintf_s(szLog, 1024, L"%s\n", resultMsg);
			mlog3(szLog);
		} else {
			mlog3(L"XBirdDummyAgent::CommonResponseLogin() - Unknown cmd result str");
		}
		
		PostDisconnect();
	}
}

void XBirdDummyAgent::OnCommMoveGameServer(MCommand* pCommand)
{
	USES_CONVERSION_EX;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return;

	TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo = (TD_LOGIN_GAME_SERVER_INFO*)pParam->GetPointer();
	int nCharCount = pParam->GetBlobCount();
	if (nCharCount <= 0)
	{
		mlog3("Error! XBirdDummyAgent::OnCommMoveGameServer(), TD_LOGIN_GAME_SERVER_INFO Count is Zero!\n");
		return;
	}

	if (NULL == pLoginGameServerInfo)
	{
		mlog3("Error! XBirdDummyAgent::OnCommMoveGameServer(), TD_LOGIN_GAME_SERVER_INFO is NULL!\n");
		return;
	}
	
	wstring strIP = pLoginGameServerInfo->strHostName;
	int nPort = (int)pLoginGameServerInfo->nPort;

	if (false == m_pConnectionManager->MoveFromGameServerToGameServer(strIP, nPort, pLoginGameServerInfo->uidConnectionKey))
	{
		mlog3("Error! XBirdDummyAgent::OnCommMoveGameServer(), Failed XConnectionManager::MoveFromGameServerToGameServer()!\n");
		return;
	}
}

void XBirdDummyAgent::OnConnect(MServerTcpSocket& tcp_socket)
{
	m_eConState = CONNECTING;
	assert(NULL!=m_pClient);
	m_pClient->OnConnect(tcp_socket);
}

void XBirdDummyAgent::OnDisconnect()
{
	m_eConState = DISCONNECTING;
	assert(NULL!=m_pClient);
	m_pClient->OnDisconnect();
}

void XBirdDummyAgent::OnRecv(const BYTE* const data,const DWORD& dataLen)
{
	assert(NULL!=data);
	assert(NULL!=m_pClient);

	m_pClient->OnRecv(data, dataLen);
}

void XBirdDummyAgent::OnSend(DWORD bytes_transferred)
{
	assert(NULL!=m_pClient);
	m_pClient->OnSend(bytes_transferred);
}

void XBirdDummyAgent::PostDisconnect()
{
	XBIRDPOSTCMD0(m_pClient, MC_LOCAL_NET_DISCONNECT);
}

void XBirdDummyAgent::SetLoginField( int nFieldID, int nChannelID, vec3 vStartPos)
{
	if (m_nLoginFieldID != nFieldID) m_nPreStayFieldID = m_nLoginFieldID;

	m_nLoginFieldID = nFieldID;
	m_nLoginChannelID = nChannelID;
	m_vCenter = vStartPos;
}

float XBirdDummyAgent::GetGroundZ( vec3 vPos )
{
	float fZ = vPos.z;

	if (m_pCollision)
	{
		RCollisionPickInfo pickInfo;
		pickInfo.m_inPickOrigin = vPos;
		pickInfo.m_inPickOrigin.z += 300.0f;
		pickInfo.m_inPickTo = vPos;
		pickInfo.m_inPickTo.z -= 50000.0f;

		if (m_pCollision->Pick(pickInfo))
		{
			fZ = pickInfo.m_outPosition.z;
		}
	}

	return fZ;
}

XDummyBot* XBirdDummyAgent::GetFrontBot()
{
	if (m_bots.empty()) return NULL;

	return *m_bots.begin();
}

bool XBirdDummyAgent::IsOddNumberID()
{
	return (0 != GetID() % 2);
}

bool XBirdDummyAgent::IsEvenNumberID()
{
	return !IsOddNumberID();
}

//////////////////////////////////////////////////////////////////////////
XBirdDummyAgentMgr::XBirdDummyAgentMgr()
{

}

XBirdDummyAgentMgr::~XBirdDummyAgentMgr()
{
	Clear();
}

void XBirdDummyAgentMgr::Run(float fDelta)
{
	Sleep(1);
	for (size_t i = 0; i < m_vecAgents.size(); ++i)
	{
		m_vecAgents[i]->Run(fDelta);
	}
}

XBirdDummyAgent* XBirdDummyAgentMgr::AddAgent(const XDummyAgentInfo& agentInfo, const wchar_t* szLoginID, int nAgentID, const wchar_t* szGameServer, const wchar_t* szLoginServer)
{
	XBirdDummyAgent* pNewAgent = new XBirdDummyAgent();
	pNewAgent->Create(agentInfo, nAgentID, szLoginID, szGameServer, szLoginServer, this);
	m_vecAgents.push_back(pNewAgent);

	if (m_vecAgents.size() == 1)
		pNewAgent->SetFirstAgent();

	return pNewAgent;
}

void XBirdDummyAgentMgr::Clear()
{
	Net().Stop();
	for (size_t i = 0; i < m_vecAgents.size(); ++i)
	{
		SAFE_DELETE(m_vecAgents[i]);
	}
	m_vecAgents.clear();
}

void XBirdDummyAgentMgr::PostConnectToLoginServer()
{
	for (size_t i = 0; i < m_vecAgents.size(); ++i)
	{
		m_vecAgents[i]->PostConnectToLoginServer();
	}
}

XBirdDummyAgent* XBirdDummyAgentMgr::GetAgentByID( int nID )
{
	int nIndex = nID - m_nStartingID;
	if (0 > nIndex || nIndex >= static_cast<int>(m_vecAgents.size())) return NULL;

	return GetAgent(nIndex);
}

bool XBirdDummyAgentMgr::Connect(const char* const addr,const char* const port,XBirdDummyAgent& agent)
{
	assert(NULL!=addr);
	assert(NULL!=port);
	cml2::MCriticalSection::Guard guard(m_cs);
	MServerTcpSocket* const socket = Net().Connect(addr, port);
	if (NULL == socket)
	{
		return false;
	}

	agent.m_pClient->SetSocket(socket);
	assert(m_pendingConnectAgents.end()==m_pendingConnectAgents.find(socket));
	m_pendingConnectAgents.insert(make_pair(socket, &agent));

	return true;
}

void XBirdDummyAgentMgr::OnConnect(MServerTcpSocket& tcp_socket)
{
	XBirdDummyAgent* agent = NULL;
	{
		cml2::MCriticalSection::Guard guard(m_cs);
		assert(false == m_pendingConnectAgents.empty());
		CONNECTING_AGENTS::iterator it = m_pendingConnectAgents.find(&tcp_socket);
		assert(it != m_pendingConnectAgents.end());
		agent = it->second;
		m_pendingConnectAgents.erase(it);
		assert(m_mapAgents.end()==m_mapAgents.find(tcp_socket.GetLink()));
		assert(agent->m_pClient->Socket()==&tcp_socket);
		m_mapAgents.insert(make_pair(tcp_socket.GetLink(), agent));
	}
	assert(NULL!=agent);
	agent->OnConnect(tcp_socket);
}

void XBirdDummyAgentMgr::OnDisconnect(const MUID& uid)
{
	XBirdDummyAgent* agent = NULL;
	{
		cml2::MCriticalSection::Guard guard(m_cs);
		AGENTS::iterator it = m_mapAgents.find(uid);
		if (it == m_mapAgents.end())
		{
			// 네트웍 라이브러리에서 종료 처리하는 부분이 여러곳이라 여러번 들어 올 수 있음
			// ex : 리시버, 디스커넥터 등
			return;
		}
		agent = it->second;
		m_mapAgents.erase(it);
	}

	assert(NULL!=agent);
	agent->OnDisconnect();
}

void XBirdDummyAgentMgr::OnRecv(const MUID& uid,const BYTE* const data,const DWORD& dataLen)
{
	assert(NULL!=data);
	XBirdDummyAgent* agent = NULL;
	{
		cml2::MCriticalSection::Guard guard(m_cs);
		AGENTS::iterator it = m_mapAgents.find(uid);
		if (it == m_mapAgents.end())
		{
			return;
		}
		agent = it->second;
	}

	assert(NULL!=agent);
	agent->OnRecv(data, dataLen);
}

void XBirdDummyAgentMgr::OnSend(const MUID& uid,DWORD bytes_transferred)
{
	XBirdDummyAgent* agent = NULL;
	{
		cml2::MCriticalSection::Guard guard(m_cs);
		AGENTS::iterator it = m_mapAgents.find(uid);
		if (it == m_mapAgents.end())
		{
			return;
		}
		agent = it->second;
	}

	assert(NULL!=agent);
	agent->OnSend(bytes_transferred);
}

//////////////////////////////////////////////////////////////////////////
const wchar_t* XBirdClientWrapper::GetLoginID()
{
	return m_pAgent->GetLoginID();
}

const wchar_t* XBirdClientWrapper::GetPassword()
{
	return L"1111";
}

int XBirdClientWrapper::GetDirectLoginFieldID()
{
	return m_pAgent->GetLoginFieldID();
}

vec3 XBirdClientWrapper::GetStartPos()
{
	return m_pAgent->GetLoginFieldStartPos();
}

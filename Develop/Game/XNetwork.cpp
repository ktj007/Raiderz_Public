#include "stdafx.h"
#include "XNetwork.h"
#include "XNetClient.h"
#include "MockNetClient.h"
#include "XConnectionManager.h"
#include "CSNet.h"
#include "MLocale.h"
#include "XWorldLocatorClient.h"

XNetwork::XNetwork()
: m_pWorldLocatorClient(NULL)
{

}

XNetwork::~XNetwork()
{
	Destroy();
}

void XNetwork::Create()
{
	minet::MNetClientDesc client_desc;
	client_desc.bKeepAlive = true;
	client_desc.nKeepAliveTickTime = 30000;

	m_pNetClient = NewClient(client_desc);
	m_pConnectionManager = new XConnectionManager(m_pNetClient);

	m_pWorldLocatorClient = new XWorldLocatorClient();
	
	if (XCONST::COMMAND_PROFILER_AUTO_SAVE)
	{
		StartCommandProfile();
	}
}

void XNetwork::Destroy()
{
	if (XCONST::COMMAND_PROFILER_AUTO_SAVE)
	{
		FinishCommandProfile();
	}

	SAFE_DELETE(m_pConnectionManager);
	SAFE_DELETE(m_pNetClient);
	SAFE_DELETE(m_pWorldLocatorClient);
}

void XNetwork::Update(float fDelta)
{
	m_pWorldLocatorClient->Update(fDelta);
	m_pNetClient->Run();
}

minet::MNetClient* XNetwork::NewClient(minet::MNetClientDesc desc)
{
	return new XNetClient(desc);
}

bool XNetwork::Post( MCommand* pCommand )
{
	return m_pNetClient->Post(pCommand);
}

MCommand* XNetwork::NewCommand( int nCmdID, MUID TargetUID )
{
	return m_pNetClient->NewCommand(nCmdID, TargetUID);
}

void XNetwork::PostConnect( const wchar_t* szIP, const int nPort )
{
	m_Desc.strServerIP = szIP;
	m_Desc.nServerPort = nPort;

	XPOSTCMD2(MC_LOCAL_NET_CONNECT, MCommandParameterString(MLocale::ConvUTF16ToAnsi(szIP).c_str()), MCommandParameterInt(nPort));
}

void XNetwork::PostDisconnect()
{
	XPOSTCMD0(MC_LOCAL_NET_DISCONNECT);
}

void XNetwork::PostConnectToGameServer( const wchar_t* szIP, int nPort )
{
	PostConnect(szIP, nPort);
}

void XNetwork::PostConnectToLoginServer( const wchar_t* szIP )
{
	if (gvar.Network.bLoginToGameServer)
	{
		PostConnectToGameServer(szIP, DEFAULT_GAME_SERVER_PORT);
		return;
	}

	int nPort = DEFAULT_LOGIN_SERVER_PORT;
	PostConnect(szIP, nPort);
}

void XNetwork::PostConnectToLocatorServer(const vector<wstring>& vecIP)
{
#ifndef _PUBLISH
	if (gvar.Network.bLoginToGameServer)
	{
		// 게임서버에는 로케이터 기능이 없습니다. - 스텐드얼론으로 띄어놓고 접속할때 당황하지 않도록 처리해줍니다.
		gvar.MyInfo.strLoginID = gvar.Network.szLoginID;

		wstring strGameServerIP;
		if (!vecIP.empty())
		{
			strGameServerIP = vecIP.front();
		}		

		PostConnectToGameServer(strGameServerIP.c_str(), DEFAULT_GAME_SERVER_PORT);
		return;
	}
	else
#endif
	{
		// TODO: 월드 목록을 받을때까지, 모든 로케이터 IP 목록을 순회하며, 지속 시도해야한다. [2/24/2011 praptor]
		m_pWorldLocatorClient->Start(vecIP, DEFAULT_LOCATOR_SERVER_PORT);
	}
}

void XNetwork::StartCommandProfile()
{
#ifdef _PUBLISH
	return;
#endif

	mlog("Command Profiler Start!\n");
	if (m_pNetClient) m_pNetClient->InitProfiler();
}

void XNetwork::FinishCommandProfile()
{
#ifdef _PUBLISH
	return;
#endif

	mlog("Command Profiler Finish!\n");
	if (m_pNetClient) m_pNetClient->ProfileAnalysis("./", "command_profile", true);
}

minet::MNetClient* MockNetwork::NewClient( minet::MNetClientDesc desc )
{
	return new MockNetClient(desc);
}

MockNetwork::MockNetwork() : XNetwork()
{

}
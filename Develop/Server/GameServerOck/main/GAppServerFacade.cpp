#include "stdafx.h"
#include "GAppServerFacade.h"
#include "GClientConnectionKeeper.h"
#include "GGlobal.h"
#include "GClientConnectionKeeper.h"
#include "GSystem.h"
#include "GCommandTable.h"
#include "GMasterServerFacade.h"
#include "GProxyChatChannel.h"
#include "GConfig.h"
#include "GAppServerChatChannelRouter.h"
#include "GStandAloneModeChatChannelRouter.h"
#include "SNetworkCardMgr.h"
#include "MLocale.h"


GAppServerFacade::GAppServerFacade()
: m_pClient(NULL)
, m_pConnectionKeeper(NULL)
, m_appCreateRegulator(10.0f)
, m_pChatChannelManager(new GProxyChatChannelManager())
, m_pChatChannelRouter(NULL)
{
	m_appCreateRegulator.Stop();
}

GAppServerFacade::GAppServerFacade(MNetClient* pNetClient)
: m_pClient(pNetClient)
, m_pConnectionKeeper(NULL)
, m_appCreateRegulator(10.0f)
, m_pChatChannelManager(new GProxyChatChannelManager())
, m_pChatChannelRouter(NULL)
{
	m_appCreateRegulator.Stop();
}

GAppServerFacade::~GAppServerFacade()
{
	DestroyNetwork();

	SAFE_DELETE(m_pChatChannelRouter);
	SAFE_DELETE(m_pChatChannelManager);
}

void GAppServerFacade::Init(void)
{
	m_appCreateRegulator.Start();

	// Router 생성
	if (GConfig::IsStandAloneMode())
	{
		SetChatChannelRouter(new GStandAloneModeChatChannelRouter());
	}
	else
	{
		SetChatChannelRouter(new GAppServerChatChannelRouter());
	}
}

bool GAppServerFacade::CreateNetwork(const wchar_t* szAppServerIP, int nAppServerPort)
{
	if (m_pClient != NULL)	return false;
	
	m_appCreateRegulator.Stop();

	minet::MNetClientDesc master_net_client_desc;	// heartbeat 체크를 하지 않습니다.

	m_pClient = new GAppNetClient(master_net_client_desc);
	wstring strNetworkCardIP = GetNetworkCardMgr()->GetIP(GConfig::m_nPrivateNetworkCardID);
	m_pConnectionKeeper = new GClientConnectionKeeper(m_pClient, szAppServerIP, nAppServerPort, gsys.pSystem, 10000, strNetworkCardIP);
	m_pConnectionKeeper->SetConnectionType(GClientConnectionKeeper::CHAT_SERVER);

	if (ConnectToAppServer(szAppServerIP, nAppServerPort, strNetworkCardIP.c_str()) == false)
	{
		return false;
	}

	return true;
}

void GAppServerFacade::DestroyNetwork()
{
	SAFE_DELETE(m_pConnectionKeeper);

	if (m_pClient != NULL)	
	{
		if (m_pClient->IsConnected())
		{
			m_pClient->Disconnect();
		}

		SAFE_DELETE(m_pClient);
	}	
}

void GAppServerFacade::SetChatChannelRouter(GServerChatChannelRouter* pChatChannelRouter)
{
	_ASSERT(pChatChannelRouter != NULL);

	SAFE_DELETE(m_pChatChannelRouter);
	m_pChatChannelRouter = pChatChannelRouter;
}

void GAppServerFacade::RunNetwork(float fDelta)
{
	// CreateNetwork가 호출되기전까진, 꼬박꼬박 요청한다.
	if (m_appCreateRegulator.IsReady(fDelta))
	{
		gsys.pMasterServerFacade->RouteRequestAppServerList();
	}

	if (m_pClient)				m_pClient->Run();
	if (m_pConnectionKeeper)	m_pConnectionKeeper->Run();
}

void GAppServerFacade::OnConnect()
{
	if (m_pConnectionKeeper)
	{
		m_pConnectionKeeper->OnConnect();
	}	

	mlog("Connected To AppServer\n");
}

void GAppServerFacade::OnDisconnect()
{
	if (m_pConnectionKeeper)
	{
		m_pConnectionKeeper->OnDisconnect();
	}

	// 현재 서버에 소속된 모든 플레이어들에게 채널 퇴장 통보.
	// Chatting 관련 Proxy 정보 모두 제거.
	m_pChatChannelManager->DisconnectAppServer();

	mlog("Disconnected From AppServer\n");
}

void GAppServerFacade::OnError( int nErrorCode )
{
	if (nErrorCode != 10061)
	{
		mlog("Client NetError (ErrorCode=%d)\n", nErrorCode);
	}
	else
	{
		if (m_pConnectionKeeper)
		{
			m_pConnectionKeeper->OnNetError(nErrorCode);
		}
	}
}

MCommand* GAppServerFacade::MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	MCommand* pNewCommand = m_pClient->NewCommand(nCmdID, 0);

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

bool GAppServerFacade::Route(MCommand* pCmd)
{
	VALID_RET(pCmd != NULL, false);

	if (m_pClient == NULL)	
	{
		SAFE_DELETE(pCmd);
		return false;
	}

	MUID uidSender = m_pClient->GetServerUID();
	pCmd->SetReceiverUID(uidSender);

	return m_pClient->Post(pCmd);
}

bool GAppServerFacade::ConnectToAppServer(const wchar_t* szIP, int nPort, const wchar_t* szNetworkCardIP)
{
	_ASSERT(m_pClient != NULL);

	MCommand* pNewCommand = m_pClient->NewLocalCommand(MC_LOCAL_NET_CONNECT_WITH_NIC);
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szIP).c_str()));
	pNewCommand->AddParameter(new MCommandParameterInt(nPort));
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szNetworkCardIP).c_str()));
	return m_pClient->Post(pNewCommand);
}

void GAppServerFacade::SetNetClient(minet::MNetClient* pNetClient)
{
	if (m_pClient)
	{		
		delete m_pClient;
	}

	m_pClient = pNetClient;
}

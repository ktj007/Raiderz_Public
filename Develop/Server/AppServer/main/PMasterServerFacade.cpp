#include "StdAfx.h"
#include "PMasterServerFacade.h"
#include "PGlobal.h"
#include "PClientConnectionKeeper.h"
#include "MCommandTable.h"
#include "PSystem.h"
#include "SNetworkCardMgr.h"
#include "PConfig.h"
#include "PServer.h"
#include "MLocale.h"

PMasterServerFacade::PMasterServerFacade(MNetClient* pMasterNetClient)
: m_pMasterNetClient(pMasterNetClient)
, m_pConnectionKeeper(NULL)
{
}

PMasterServerFacade::~PMasterServerFacade(void)
{
	DestroyNetwork();
}

bool PMasterServerFacade::CreateNetwork(const wchar_t* szMasterServerIP, int nMasterServerPort)
{
	wstring strNetworkCardIP = GetNetworkCardMgr()->GetIP(PConfig::m_nMasterServer_NetworkCardID);
	m_pConnectionKeeper = new PClientConnectionKeeper(m_pMasterNetClient, szMasterServerIP, nMasterServerPort, gsys.pSystem, 10000, strNetworkCardIP.c_str());
	if (ConnectToMasterServer(szMasterServerIP, nMasterServerPort, strNetworkCardIP.c_str()) == false)
	{
		return false;
	}

	return true;
}
void PMasterServerFacade::DestroyNetwork()
{
	if (NULL != m_pMasterNetClient)
		if (m_pMasterNetClient->IsConnected())
			m_pMasterNetClient->Disconnect();

	SAFE_DELETE(m_pConnectionKeeper);
	SAFE_DELETE(m_pMasterNetClient);
}
void PMasterServerFacade::RunNetwork()
{
	if (m_pMasterNetClient) m_pMasterNetClient->Run();
	if (m_pConnectionKeeper) m_pConnectionKeeper->Run();
}
void PMasterServerFacade::OnConnect()
{
	if (m_pConnectionKeeper)
		m_pConnectionKeeper->OnConnect();
	mlog("Connected To MasterServer\n");
}
void PMasterServerFacade::OnDisconnect()
{
	if (m_pConnectionKeeper)
		m_pConnectionKeeper->OnDisconnect();
	
	gsys.pServer->Exit();

	mlog("Connected To MasterServer\n");
}
void PMasterServerFacade::OnError(int nErrorCode)
{
	if (nErrorCode == WSAECONNREFUSED)
	{
		// 연결 실패인 경우
		if (m_pConnectionKeeper)
		{
			m_pConnectionKeeper->OnNetError(nErrorCode);
		}
	}
	else
	{
		// 네트워크 오류
		gsys.pServer->Exit();
		mlog("Client NetError (ErrorCode=%d)\n", nErrorCode);		
	}
}


bool PMasterServerFacade::ConnectToMasterServer( const wchar_t* szMasterServerIP, int nPort, const wchar_t* szNetworkCardIP )
{
	if (m_pMasterNetClient == NULL) return false;

	MCommand* pNewCommand = m_pMasterNetClient->NewLocalCommand(MC_LOCAL_NET_CONNECT_WITH_NIC);
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szMasterServerIP).c_str()));
	pNewCommand->AddParameter(new MCommandParameterInt(nPort));
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szNetworkCardIP).c_str()));
	return m_pMasterNetClient->Post(pNewCommand);
}
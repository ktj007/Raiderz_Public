#include "stdafx.h"
#include "LMasterClient.h"
#include "LMasterNetClient.h"
#include "LClientConnectionKeeper.h"
#include "LConfig.h"
#include "LCommandTable.h"
#include "LGlobal.h"
#include "LSystem.h"
#include "SNetworkCardMgr.h"
#include "LServer.h"
#include "MLocale.h"

LMasterClient::LMasterClient()
: m_pConnectionKeeper(NULL), m_pNetClient(NULL)
{

}

LMasterClient::~LMasterClient()
{

}

bool LMasterClient::Create()
{
	minet::MNetClientDesc master_net_client_desc;	// heartbeat 체크를 하지 않습니다.

	return Create( new LMasterNetClient(master_net_client_desc) );
}

bool LMasterClient::Create( MNetClient* pNetClient )
{
	m_pNetClient = pNetClient;	

	wstring strNetworkCardIP = GetNetworkCardMgr()->GetIP(LConfig::m_nMasterServer_NetworkCardID);
	m_pConnectionKeeper = new LClientConnectionKeeper(m_pNetClient, LConfig::m_strMasterServerIP.c_str(), LConfig::m_nMasterServerPort, gsys.pSystem, 10000, strNetworkCardIP.c_str());

	/// 네트워크 모듈 생성
	if (Post_ConnectToMasterServer(LConfig::m_strMasterServerIP.c_str(), LConfig::m_nMasterServerPort, strNetworkCardIP.c_str()) == false)
	{
		return false;
	}

	return true;
}

void LMasterClient::Destroy()
{
	if (m_pNetClient->IsConnected())
	{
		m_pNetClient->Disconnect();
	}

	SAFE_DELETE(m_pConnectionKeeper);
	SAFE_DELETE(m_pNetClient);
}

void LMasterClient::Run()
{
	if (LConfig::m_bStandAlone)
		return;

	m_pNetClient->Run();
	if (m_pConnectionKeeper) m_pConnectionKeeper->Run();
}


bool LMasterClient::Post_PlayerLoginToGameServer(MUID uidPlayer, const TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, const TD_LOGIN_FIELD_INFO* pLoginFieldInfo)
{
	RVALID_RET(uidPlayer.IsValid(), false);
	RVALID_RET(NULL != pLoginAccountInfo, false);
	RVALID_RET(NULL != pLoginFieldInfo, false);

	MCommand* pNewCommand = m_pNetClient->NewSendCommand(MLC_COMM_REQUEST_LOGIN_GAME_SERVER);
	pNewCommand->AddParameter(NEW_UID(uidPlayer));
	pNewCommand->AddParameter(NEW_SINGLE_BLOB(pLoginAccountInfo, sizeof(TD_LOGIN_ACCOUNT_INFO)));
	pNewCommand->AddParameter(NEW_SINGLE_BLOB(pLoginFieldInfo, sizeof(TD_LOGIN_FIELD_INFO)));

	return m_pNetClient->Post(pNewCommand);
}

bool LMasterClient::Post_ConnectToMasterServer( const wchar_t* szIP, int nPort, const wchar_t* szNetworkCardIP )
{
	MCommand* pNewCommand = m_pNetClient->NewLocalCommand(MC_LOCAL_NET_CONNECT_WITH_NIC);
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szIP).c_str()));
	pNewCommand->AddParameter(new MCommandParameterInt(nPort));
	pNewCommand->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szNetworkCardIP).c_str()));
	return m_pNetClient->Post(pNewCommand);
}

void LMasterClient::OnConnect()
{
	if (m_pConnectionKeeper)
	{
		m_pConnectionKeeper->OnConnect();
	}

	mlog("Connected To MasterServer\n");
}

void LMasterClient::OnDisconnect()
{
	gsys.pServer->Exit();

	mlog("Disconnected From MasterServer\n");
}

void LMasterClient::OnError( int nErrorCode )
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
		gsys.pServer->Exit();
		mlog("Client NetError (ErrorCode=%d)\n", nErrorCode);		
	}
}

bool LMasterClient::Post_Disconnect(bool bConnectionKeeperActive)
{
	if (m_pConnectionKeeper)
	{
		m_pConnectionKeeper->SetActive(bConnectionKeeperActive);
	}

	MCommand* pNewCommand = m_pNetClient->NewLocalCommand(MC_LOCAL_NET_DISCONNECT);
	return m_pNetClient->Post(pNewCommand);
}

MCommand* LMasterClient::MakeNewSendCommand( int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... )
{
	MCommand* pNewCommand = m_pNetClient->NewSendCommand(nCmdID);

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
#include "stdafx.h"
#include "XConnectionManager.h"
#include "MLocale.h"
#include "CCommandResultTable.h"

bool XConnectionManager::IsStandAloneServer()
{
	return gvar.Network.bLoginToGameServer;
}

bool XConnectionManager::IsMovingStatus()
{
	if (GetStatus() == CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER ||
		GetStatus() == CONN_STATUS_MOVING_FROM_GAMESERVER_TO_GAMESERVER ||
		GetStatus() == CONN_STATUS_MOVING_FROM_GAMESERVER_TO_LOGINSERVER) 
	{
		return true;
	}
	return false;

}
bool XConnectionManager::OnDisconnect()
{
	if (IsMovingStatus() == false)
	{
		m_nStatus = CONN_STATUS_DISCONNECT;
		return false;
	}

	if (m_ToServer.bFilled == false)
	{
		m_nStatus = CONN_STATUS_DISCONNECT;
		mlog3("Error! ToServer info is Empty!\n");
		return false;
	}

	PostConnectToServer(m_ToServer.strIP.c_str(), m_ToServer.nPort);

	if (GetStatus() == CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER)
		_PostEnterWorldMsgLocal(CR_ENTER_WORLD_CONNECT_TO_GAMESERVER);

	return true;
}

void XConnectionManager::OnReplyConnect()
{
	switch (GetStatus())
	{
	case CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER:
		{
			PostRequestLoginToGameServer();
		}
		break;
	case CONN_STATUS_MOVING_FROM_GAMESERVER_TO_GAMESERVER:
		{
			PostRequestMoveToGameServer();
		}
		break;
	case CONN_STATUS_MOVING_FROM_GAMESERVER_TO_LOGINSERVER:
		{
			PostRequestMoveToLoginServer();
		}
		break;
	default:
		{
			PostRequestLogin();
		}
		break;
	}

	m_ToServer.Reset();
}

void XConnectionManager::OnConnectedToLoginServer()
{
	m_nStatus = CONN_STATUS_CONNECTED_TO_LOGIN_SERVER;
}

void XConnectionManager::OnConnectedToGameServer()
{
	m_nStatus = CONN_STATUS_CONNECTED_TO_GAME_SERVER;
}

void XConnectionManager::MoveFromLoginServerToGameServer(wstring strIP, int nPort, MUID uidConnectionKey)
{
	if (GetStatus() == CONN_STATUS_CONNECTED_TO_GAME_SERVER) return;

	if (GetStatus() == CONN_STATUS_CONNECTED_TO_LOGIN_SERVER)
	{
		m_nStatus = CONN_STATUS_MOVING_FROM_LOGINSERVER_TO_GAMESERVER;
	}
	else
	{
		_ASSERT(0);
		return;
	}

	StartToMoveServer(strIP, nPort, uidConnectionKey);
}
bool XConnectionManager::MoveFromGameServerToGameServer(wstring strIP, int nPort, MUID uidConnectionKey)
{
	if (GetStatus() == CONN_STATUS_CONNECTED_TO_LOGIN_SERVER) return false;

	if (GetStatus() == CONN_STATUS_CONNECTED_TO_GAME_SERVER)
	{
#ifndef _PUBLISH
		/// StandAlone 일 경우, 다른 게임서버로의 이동은 처리하지 않습니다.
		if (IsStandAloneServer()) return false;
#endif

		m_nStatus = CONN_STATUS_MOVING_FROM_GAMESERVER_TO_GAMESERVER;
	}
	else
	{
		_ASSERT(0);
		return false;
	}

	StartToMoveServer(strIP, nPort, uidConnectionKey);
	return true;
}
bool XConnectionManager::MoveFromGameServerToLoginServer(wstring strIP, int nPort, MUID uidConnectionKey)
{
	if (GetStatus() == CONN_STATUS_CONNECTED_TO_LOGIN_SERVER) return false;

	if (GetStatus() == CONN_STATUS_CONNECTED_TO_GAME_SERVER)
	{
		m_nStatus = CONN_STATUS_MOVING_FROM_GAMESERVER_TO_LOGINSERVER;
	}
	else
	{
		_ASSERT(0);
		return false;
	}

	StartToMoveServer(strIP, nPort, uidConnectionKey);
	return true;
}

void XConnectionManager::StartToMoveServer(wstring strIP, int nPort, MUID uidConnectionKey)
{
	m_ToServer.strIP = strIP;
	m_ToServer.nPort = nPort;
	m_ToServer.uidConnectionKey = uidConnectionKey;
	m_ToServer.bFilled = true;

	assert(NULL!=m_pNetClientWrapper);
	m_pNetClientWrapper->Disconnect();
}

void XConnectionManager::PostGameStart()
{
	// 내 캐릭터를 선택했으니 게임 시작합시다~~~
	if (m_pNetClientWrapper->GetDirectLoginFieldID() <= 0)
	{
		_PostGameStart();
	}
	else
	{
		_PostGameStart(m_pNetClientWrapper->GetDirectLoginFieldID(), m_pNetClientWrapper->GetStartPos());
	}

	_PostEnterWorldMsgLocal(CR_ENTER_WORLD_GAMESTART);
}

void XConnectionManager::_PostRequestLogin( const wchar_t* szID, const wchar_t* szPassword, int nCommandVersion )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_LOGIN);
	pNewCmd->AddParameter(new MCommandParameterWideString(szID));
	pNewCmd->AddParameter(new MCommandParameterWideString(szPassword));
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::PostRequestLogin()
{
	if (gvar.Network.PmangInfo.isLoginOnPmang)
	{
		_PostRequestLoginOnPmang(SH_COMMAND_VERSION);
	}
	else
	{
		_PostRequestLogin(m_pNetClientWrapper->GetLoginID(), m_pNetClientWrapper->GetPassword(), SH_COMMAND_VERSION);
	}

	XGetAntiHack()->PlayerLogin( gvar.Network.szLoginID);
}

void XConnectionManager::PostRequestLoginToGameServer()
{
	if (gvar.Network.PmangInfo.isLoginOnPmang)
	{
		_PostRequestLoginToGameServerOnPmang(m_ToServer.uidConnectionKey, SH_COMMAND_VERSION);
	}
	else
	{
		_PostRequestLoginToGameServer(m_ToServer.uidConnectionKey, SH_COMMAND_VERSION);
	}

	_PostEnterWorldMsgLocal(CR_ENTER_WORLD_LOGIN_GAMESERVER);
}

void XConnectionManager::PostRequestMoveToGameServer()
{
	if (gvar.Network.PmangInfo.isLoginOnPmang)
	{
		_PostRequestMoveToGameServerOnPmang(m_ToServer.uidConnectionKey, SH_COMMAND_VERSION);
	}
	else
	{
		_PostRequestMoveToGameServer(m_ToServer.uidConnectionKey, SH_COMMAND_VERSION);
	}
}

void XConnectionManager::PostRequestMoveToLoginServer()
{
	if (gvar.Network.PmangInfo.isLoginOnPmang)
	{
		_PostRequestMoveToLoginServerOnPmang(m_ToServer.uidConnectionKey, SH_COMMAND_VERSION);
	}
	else
	{
		_PostRequestMoveToLoginServer(m_ToServer.uidConnectionKey, SH_COMMAND_VERSION);
	}
}

void XConnectionManager::_PostRequestLoginOnPmang( int nCommandVersion )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_LOGIN_ON_PMANG);
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	pNewCmd->AddParameter(new MCommandParameterWideString(gvar.Network.PmangInfo.strLauncherParameter.c_str()));
	m_pNetClientWrapper->Post(pNewCmd);
}

void XConnectionManager::_PostRequestLoginToGameServer( MUID uidConnectionKey, int nCommandVersion )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_LOGIN_GAME_SERVER);
	pNewCmd->AddParameter(new MCommandParameterUID(uidConnectionKey));
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::_PostRequestMoveToGameServer( MUID uidConnectionKey, int nCommandVersion )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_MOVE_GAME_SERVER);
	pNewCmd->AddParameter(new MCommandParameterUID(uidConnectionKey));
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::_PostRequestMoveToLoginServer( MUID uidConnectionKey, int nCommandVersion )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_MOVE_LOGIN_SERVER);
	pNewCmd->AddParameter(new MCommandParameterUID(uidConnectionKey));
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::_PostRequestLoginToGameServerOnPmang(MUID uidConnectionKey, int nCommandVersion)
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_LOGIN_GAME_SERVER_ON_PMANG);
	pNewCmd->AddParameter(new MCommandParameterUID(uidConnectionKey));
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	pNewCmd->AddParameter(new MCommandParameterWideString(gvar.Network.PmangInfo.strStatIndex.c_str()));
	pNewCmd->AddParameter(new MCommandParameterInt(gvar.Network.PmangInfo.nPCCafeID));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::_PostRequestMoveToGameServerOnPmang(MUID uidConnectionKey, int nCommandVersion)
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_MOVE_GAME_SERVER_ON_PMANG);
	pNewCmd->AddParameter(new MCommandParameterUID(uidConnectionKey));
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	pNewCmd->AddParameter(new MCommandParameterWideString(gvar.Network.PmangInfo.strStatIndex.c_str()));
	pNewCmd->AddParameter(new MCommandParameterInt(gvar.Network.PmangInfo.nPCCafeID));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::_PostRequestMoveToLoginServerOnPmang( MUID uidConnectionKey, int nCommandVersion )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_REQUEST_MOVE_LOGIN_SERVER_ON_PMANG);
	pNewCmd->AddParameter(new MCommandParameterUID(uidConnectionKey));
	pNewCmd->AddParameter(new MCommandParameterInt(nCommandVersion));
	pNewCmd->AddParameter(new MCommandParameterWideString(gvar.Network.PmangInfo.strStatIndex.c_str()));
	pNewCmd->AddParameter(new MCommandParameterInt(gvar.Network.PmangInfo.nPCCafeID));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::_PostGameStart( int nFieldID/*=-1*/, vec3& pos/*=vec3(0,0,0)*/ )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_CHAR_GAMESTART);
	pNewCmd->AddParameter(new MCommandParameterInt(nFieldID));
	pNewCmd->AddParameter(new MCommandParameterVector(pos));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::PostConnectToServer( const wchar_t* szIP, int nPort )
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_LOCAL_NET_CONNECT);
	pNewCmd->AddParameter(new MCommandParameterString(MLocale::ConvUTF16ToAnsi(szIP).c_str()));
	pNewCmd->AddParameter(new MCommandParameterInt(nPort));
	m_pNetClientWrapper->Post(pNewCmd); 
}

void XConnectionManager::_PostEnterWorldMsgLocal(int nResult)
{
	MCommand* pNewCmd = m_pNetClientWrapper->NewCmd(MC_COMM_ENTER_WORLD_LOCAL_MSG);
	pNewCmd->AddParameter(new MCommandParameterInt(nResult));
	m_pNetClientWrapper->Post(pNewCmd);
}

XConnectionManager::XConnectionManager( minet::MNetClient* pNetClient ) : m_nStatus(CONN_STATUS_DISCONNECT)
{
	m_pNetClientWrapper = new XNetClientWrapper(pNetClient);
}

XConnectionManager::XConnectionManager( XNetClientWrapper* pNetClientWrapper ) : m_pNetClientWrapper(pNetClientWrapper), m_nStatus(CONN_STATUS_DISCONNECT)
{

}

XConnectionManager::~XConnectionManager()
{
	SAFE_DELETE(m_pNetClientWrapper);
}

//////////////////////////////////////////////////////////////////////////
minet::MCommand* XNetClientWrapper::NewCmd( int nCmdID )
{
	MUID uidTarget= m_pNetClient->GetServerUID();
	return m_pNetClient->NewCommand(nCmdID, uidTarget);
}

bool XNetClientWrapper::Post( minet::MCommand* pCommand )
{
	return m_pNetClient->Post(pCommand);
}

void XNetClientWrapper::Disconnect()
{
	m_pNetClient->Disconnect();
}

const wchar_t* XNetClientWrapper::GetLoginID()
{
	return gvar.MyInfo.strLoginID.c_str();
}

const wchar_t* XNetClientWrapper::GetPassword()
{
	return gvar.Temp.szPassword;
}

int XNetClientWrapper::GetDirectLoginFieldID()
{
	return gvar.Network.nDirectLoginFieldID;
}

vec3 XNetClientWrapper::GetStartPos()
{
	return gvar.Network.vStartPos;
}
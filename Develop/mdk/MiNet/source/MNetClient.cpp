#include "stdafx.h"
#include "MNetClient.h"
#include "MCommandTable.h"
#include <stdarg.h>
#include "MErrorTable.h"
#include "MDebug.h"
#include "MCommandStream.h"
#include "MCmdHandler_NetClient.h"
#include "MCommandCommunicator.h"

namespace minet {

MNetClient::MNetClient(const MNetClientDesc& desc)
{
	m_Desc = desc;
	m_This = 0;

	m_pCommandStream = new MCommandStream(GetCommandManager());
	m_pCmdHandler = new MCmdHandler_NetClient(this);

	m_Server.SetInvalid();

	// 소켓 이벤트 연결
	m_TCPSocket.SetCallbackContext(this);
	m_TCPSocket.SetConnectCallback(SocketConnectEvent);
	m_TCPSocket.SetDisconnectCallback(SocketDisconnectEvent);
	m_TCPSocket.SetRecvCallback(SocketRecvEvent);
	m_TCPSocket.SetSocketErrorCallback(SocketErrorEvent);
}

MNetClient::~MNetClient()
{
	m_TCPSocket.Finalize();

	SAFE_DELETE(m_pCmdHandler);
	SAFE_DELETE(m_pCommandStream);
}

MUID MNetClient::GetSenderUIDBySocket(SOCKET socket)
{ 
	if (m_TCPSocket.GetSocket() == socket) return m_Server;
	else return 0;
}


void MNetClient::OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp)
{
	m_Server = uidHost;
	m_This = uidAlloc;
	
	InitCrypt(uidHost, uidAlloc);
}


int MNetClient::Connect( const char* szRemoteIP, int nPort, const char* szMyNetworkCardIP )
{
	char text[256];
	sprintf_s(text, "MNetClient::Connect : RemoteIP=%s(port=%d), MyNetworkCardIP=%s", szRemoteIP, nPort, szMyNetworkCardIP);
	m_TCPSocket.Log(text);

	if (m_TCPSocket.Connect(szRemoteIP, nPort, szMyNetworkCardIP))
		return MOK;
	else 
		return MERR_UNKNOWN;
}

void MNetClient::SendCommand(MCommand* pCommand)
{
	if (m_This == MUID::ZERO) return;

	if (pCommand->m_Receiver != m_Server || pCommand->m_pCommandDesc->GetFlow() == MCF_S2C)
	{
		_ASSERT(0);
		return;
	}

	int nPacketSize = CalcPacketSize(pCommand);

	if (nPacketSize - sizeof(MPacketHeader) <= 0 ||
		nPacketSize >= MAX_PACKET_SIZE)
	{
		_ASSERT(0);
		return;
	}

	char* pSendBuf = new char[nPacketSize];

	_ASSERT(nPacketSize > 0);

	int size = MakeCmdPacket(pSendBuf, nPacketSize, pCommand);

	if (size > 0)
	{
		m_TCPSocket.Send(pSendBuf, size);		// 여기서 pSendBuf는 delete 될 수 있으므로 이 이후에 pSendBuf를 사용하면 안됨
		pSendBuf = NULL;

		m_TrafficMonitor.RecordSend((uint32)size);
		m_ConnectionKeeper.SetSendTime(timeGetTime());
	}
	else
	{
		delete [] pSendBuf;
		pSendBuf = NULL;
	}
}


void MNetClient::Disconnect()
{
	m_TCPSocket.Disconnect();
}

int MNetClient::MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, MCommand* pCommand)
{
	return m_PacketBuilder.BuildPacket(pOutPacket, iMaxPacketSize, pCommand, &m_PacketCrypter);
}

bool MNetClient::OnSockRecv(SOCKET sock, void* pPacket, DWORD dwSize)
{
	if (m_pCommandStream == NULL) return false;

	MUID uidReceiver = m_This;
	MUID uidSender = GetSenderUIDBySocket(sock);

	// New Cmd Buffer ////////////////
	m_pCommandStream->Read((char*)pPacket, dwSize);
	m_TrafficMonitor.RecordRecv(dwSize);

	while(MCommand* pCmd = m_pCommandStream->GetCommand()) 
	{
		pCmd->m_Sender = uidSender;
		pCmd->m_Receiver = uidReceiver;

		if (pCmd->GetID() == MC_NET_REALLOC_CLIENT_UID)
		{
			MUID uidAlloc;	// 할당 받을 클라이언트 UID
			if(pCmd->GetParameter(&uidAlloc, 0, MPT_UID) )
			{
				m_This = uidAlloc;
				uidReceiver = uidAlloc;
			}
			delete pCmd;
			pCmd = NULL;

			continue;
		}

		Post(pCmd);
	}

	return true;
}

bool MNetClient::OnSockConnect(SOCKET sock)
{
	MCommand* pNewCommand = NewCommand(MC_LOCAL_NET_ONCONNECT, GetUID());
	Post(pNewCommand);

	return true;
}
bool MNetClient::OnSockDisconnect()
{
	MCommand* pNewCommand = NewCommand(MC_LOCAL_NET_ONDISCONNECT, GetUID());
	pNewCommand->AddParameter(new MCommandParameterUID(GetUID()));
	Post(pNewCommand);

	return true;
}

void MNetClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &nErrorCode)
{
	MCommand* pNewCommand = NewCommand(MC_LOCAL_NET_ONERROR, GetUID());
	pNewCommand->AddParameter(new MCommandParameterInt(nErrorCode));
	Post(pNewCommand);
}

bool MNetClient::SocketRecvEvent(void* pCallbackContext, SOCKET sock, void* pPacket, DWORD dwSize)
{
	MNetClient* pClient = (MNetClient*)pCallbackContext;

	return pClient->OnSockRecv(sock, pPacket, dwSize);
}

bool MNetClient::SocketConnectEvent(void* pCallbackContext, SOCKET sock)
{
	MNetClient* pClient = (MNetClient*)pCallbackContext;

	return pClient->OnSockConnect(sock);
}
bool MNetClient::SocketDisconnectEvent(void* pCallbackContext)
{
	MNetClient* pClient = (MNetClient*)pCallbackContext;
	return pClient->OnSockDisconnect();
}

void MNetClient::SocketErrorEvent(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	MNetClient* pClient = (MNetClient*)pCallbackContext;

	pClient->OnSockError(sock, ErrorEvent, ErrorCode);
}

void MNetClient::GetTraffic(int* nSendTraffic, int* nRecvTraffic)
{ 
	return m_TCPSocket.GetTraffic(nSendTraffic, nRecvTraffic); 
}

MCommandResult MNetClient::OnCommand(MCommand* pCommand)
{
	_ASSERT(NULL != pCommand);
	MCommandHandlerMap::iterator itr = m_mapCommandHandler.find(pCommand->GetID());
	if (itr == m_mapCommandHandler.end()) return CR_FALSE;

	_COMMAND_HANDLER& node = (*itr).second;
	if (NULL == node.m_pFunc) return CR_FALSE;

	return node.m_pFunc(pCommand, node.m_pHandler);
}

void MNetClient::OnPrepareRun()
{
	CheckConnectionKeeper();
}

void MNetClient::CheckConnectionKeeper()
{
	if (m_Desc.bKeepAlive)
	{
		if (m_ConnectionKeeper.CheckToSendCommand(timeGetTime(), m_Desc.nKeepAliveTickTime))
		{
			MCommand* pNewCommand = NewCommand(MC_NET_KEEP_ALIVE, m_Server);
			Post(pNewCommand);
		}
	}
}

void MNetClient::Ping()
{
	uint32 nNowTime = timeGetTime();
	MCommand* pNewCommand = NewCommand(MC_NET_PING, m_Server);
	pNewCommand->AddParameter(new MCommandParameterUInt(nNowTime));
	Post(pNewCommand);

	m_PingData.Sent(nNowTime);
}

void MNetClient::InitCrypt( MUID& uidHost, MUID& uidAlloc )
{
	char szKey[32] = {0, };

	MMakeCryptKey((unsigned char*)szKey, 32, uidHost, uidAlloc, 0);
	m_PacketCrypter.InitKey(&szKey, 32);
	m_pCommandStream->InitCrypt(&m_PacketCrypter, false);
}

MCommand* MNetClient::NewSendCommand( int nCmdID )
{
	return new MCommand(nCmdID, m_This, m_Server);
}

void MNetClient::OnReallocUID( MUID uidAlloc )
{
	m_This = uidAlloc;
}

void MNetClient::GetTotalTraffic( int64& noutRecvSize, int64& noutSendSize )
{
	m_TrafficMonitor.GetTotalTraffic(noutRecvSize, noutSendSize);
}

void MNetClient::GetSecondTraffic( uint32& noutRecvSize, uint32& noutSendSize )
{
	m_TrafficMonitor.GetSecondTraffic(noutRecvSize, noutSendSize);
}

void MNetClient::SetCommandHandler(int nCmdID, MCommandHandler* pHandler, MCommandHanderFunc* fnFunc)
{
	MCommandHandlerMap::iterator itr = m_mapCommandHandler.find(nCmdID);
	if (itr == m_mapCommandHandler.end())
	{
		_COMMAND_HANDLER node;
		node.m_pHandler = pHandler;
		node.m_pFunc = fnFunc;
		m_mapCommandHandler.insert(MCommandHandlerMap::value_type(nCmdID, node));
	}
	else
	{
		_COMMAND_HANDLER& node = (*itr).second;
		node.m_pHandler = pHandler;
		node.m_pFunc = fnFunc;
	}
}

} // namespace minet
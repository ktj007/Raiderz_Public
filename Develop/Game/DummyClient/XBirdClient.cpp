#include "stdafx.h"
#include "XBirdClient.h"
#include "MCommandTable.h"
#include <stdarg.h>
#include "MErrorTable.h"
#include "MDebug.h"
#include "MCommandStream.h"
#include "MCmdHandler_NetClient.h"
#include "XBirdDummyAgent.h"
#include "XNet.h"
#include "MServerTcpSocket.h"

bool XBirdClient::m_bPrintCommandLog = false;

XBirdClient::XBirdClient(XBirdDummyAgent* pAgent) : m_pAgent(pAgent), m_socket(NULL), m_totalRecvBytes(0), m_totalSendBytes(0)
{
	m_pCommandStream = new MCommandStream(GetCommandManager());

	m_Server.SetInvalid();
}

XBirdClient::~XBirdClient()
{
	SAFE_DELETE(m_pCommandStream);
}

// MUID XBirdClient::GetSenderUIDBySocket(SOCKET socket)
// {
// 	if (m_pTCPSocket->GetSocket() == socket) return m_Server;
// 	else return 0;
// }

MCommandResult XBirdClient::OnCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
	case MC_NET_REPLY_CONNECT:
		{
			MUID uidAlloc;	// 할당 받을 클라이언트 UID
			MUID uidHost;	// 서버 UID
			unsigned long int nTimeStamp;

			if( !pCommand->GetParameter(&uidHost, 0, MPT_UID) ) return CR_ERROR;
			if( !pCommand->GetParameter(&uidAlloc, 1, MPT_UID) ) return CR_ERROR;
			if( !pCommand->GetParameter(&nTimeStamp, 2, MPT_UINT) ) return CR_ERROR;

			OnReplyConnect(uidHost, uidAlloc, nTimeStamp);
		}
		break;
	// MCmdHandler_NetClient.cpp 에 있던 것
	case MC_LOCAL_NET_CONNECT:
		{
			char szIP[256] = "";
			int nPort = 0;

			if( !pCommand->GetParameter(szIP,	0, MPT_STR, sizeof(szIP)) )	return CR_ERROR;
			if( !pCommand->GetParameter(&nPort, 1, MPT_INT) ) return CR_ERROR;
			assert(NULL!=m_pAgent);

			USES_CONVERSION_EX;
			m_pAgent->Connect(A2W_EX(szIP, 0), nPort);

			return CR_TRUE;
		}

		assert(0);
		break;
	case MC_LOCAL_NET_DISCONNECT:
		{
			Disconnect();
			return CR_TRUE;
		}
		break;
	case MC_LOCAL_NET_ONERROR:
		{

		}
	case MC_LOCAL_NET_ONDISCONNECT:
		{

		}
		break;
	};

	if (m_pAgent) 
	{
		if (m_pAgent->OnCommand(this, pCommand) == CR_TRUE) return CR_TRUE;
	}

	return CR_TRUE;
}

void XBirdClient::OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp)
{
	m_Server = uidHost;
	OnReallocUID(uidAlloc);

	// 암호화 키 설정
	char szKey[32] = {0, };

	MMakeCryptKey((unsigned char*)szKey, 32, uidHost, uidAlloc, 0);
	m_PacketCrypter.InitKey(&szKey, 32);
	m_pCommandStream->InitCrypt(&m_PacketCrypter, false);


	if (m_pAgent)
	{
		m_pAgent->GetConnectionManager()->OnReplyConnect();
	}

}

void XBirdClient::OnReallocUID( MUID uidAlloc )
{
	m_This = uidAlloc;
	gvar.MyInfo.MyUID = m_This;
}

void XBirdClient::OnRegisterCommand(MCommandManager* pCommandManager)
{
}

// int XBirdClient::Connect(const char* szIP,const char* const szPort)
// {
// 	m_pTCPSocket->Log("XBirdClient::Connect");
// 	if (m_pTCPSocket->Connect(szIP, nPort))
// 		return MOK;
// 	else 
// 		return MERR_UNKNOWN;
// }

void XBirdClient::SendCommand(MCommand* pCommand)
{
	MCriticalSection::Guard guard(m_csRecvLock);
	if (NULL == m_socket)
		return;

	if (pCommand->m_Receiver == MUID::ZERO || pCommand->m_Sender == MUID::ZERO) return;

	if (pCommand->m_Receiver != m_Server || pCommand->m_pCommandDesc->GetFlow() == MCF_S2C)
	{
		_ASSERT(0);
		return;
	}

	int nPacketSize = CalcPacketSize(pCommand);
	char* pSendBuf = new char[nPacketSize];

	int size = MakeCmdPacket(pSendBuf, nPacketSize, pCommand);

	if (size > 0)
	{
		m_socket->Send(rcast<BYTE*>(pSendBuf), size);
	}
	else
	{
		delete [] pSendBuf;
	}

	LogCommand(pCommand);
}

bool XBirdClient::Post(MCommand* pCommand)
{
	m_csRecvLock.Lock();
	bool bRet = MCommandCommunicator::Post(pCommand);
	m_csRecvLock.Unlock();
	return bRet;
}

MCommand* XBirdClient::GetCommand()
{
	m_csRecvLock.Lock();
	MCommand* pCmd = MCommandCommunicator::GetCommand();
	m_csRecvLock.Unlock();

	return pCmd;
}

void XBirdClient::Disconnect()
{
	MCriticalSection::Guard guard(m_csRecvLock);
	if (NULL != m_socket)
	{
		//m_socket->Disconnect();
		m_socket->HardDisconnect(m_socket->GetLink());
		m_socket = NULL;
	}
}

int XBirdClient::MakeCmdPacket(char* pOutPacket, int iMaxPacketSize, MCommand* pCommand)
{
	return m_PacketBuilder.BuildPacket(pOutPacket, iMaxPacketSize, pCommand, &m_PacketCrypter);
}

// void XBirdClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &nErrorCode)
// {
// 	MCommand* pNewCommand = NewCommand(MC_LOCAL_NET_ONERROR, GetUID());
// 	pNewCommand->AddParameter(new MCommandParameterInt(nErrorCode));
// 	Post(pNewCommand);
// }

void XBirdClient::OnConnect(minet::MServerTcpSocket& socket)
{
	assert(m_socket==&socket);
	MCommand* pNewCommand = NewCommand(MC_LOCAL_NET_ONCONNECT, GetUID());
	assert(NULL!=pNewCommand);
	bool res = Post(pNewCommand);
	assert(true==res);
}

void XBirdClient::OnDisconnect()
{
	MCommand* pNewCommand = NewCommand(MC_LOCAL_NET_ONDISCONNECT, GetUID());
	assert(NULL!=pNewCommand);
	bool res = false;
	res = pNewCommand->AddParameter(new MCommandParameterUID(GetUID()));
	assert(true==res);
	res = Post(pNewCommand);
	assert(true==res);
}

void XBirdClient::OnRecv(const BYTE* const data,const DWORD& dataLen)
{
	MUID uidReceiver = m_This;
	//MUID uidSender = GetSenderUIDBySocket(sock); //todo ?
	MUID uidSender = m_Server;

	// New Cmd Buffer ////////////////
	assert(NULL!=m_pCommandStream);
	m_pCommandStream->Read((char*)data, dataLen);

	m_csRecvLock.Lock();
	m_totalRecvBytes += dataLen;
	m_TrafficLog.OnRecv(dataLen);
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
			continue;
		}

		Post(pCmd);
	}
	m_csRecvLock.Unlock();
}

void XBirdClient::OnSend(DWORD bytes_transferred)
{
	m_csRecvLock.Lock();
	m_totalSendBytes += bytes_transferred;
	m_TrafficLog.OnRecv(bytes_transferred);
	m_csRecvLock.Unlock();
}

void XBirdClient::GetTraffic(int& nSendTraffic, int& nRecvTraffic)
{ 
	nSendTraffic = m_TrafficLog.GetSendTraffic();
	nRecvTraffic = m_TrafficLog.GetRecvTraffic();
}

void XBirdClient::OnPrepareCommand( MCommand* pCommand )
{
	LogCommand(pCommand);
}

void XBirdClient::LogCommand( MCommand* pCommand )
{
	if (m_pAgent->IsFirstAgent() == false) return;
	if (m_bPrintCommandLog == false) return;

	if (global.ui->GetCommandLog() && global.ui->GetCommandLog()->GetShow())
	{
		global.ui->GetCommandLog()->AddCommand(pCommand);
	}
}


uint32 XBirdClient::GetTotalSendBytes()
{
	return m_totalSendBytes;
}

uint32 XBirdClient::GetTotalRecvBytes()
{
	return m_totalRecvBytes;
}

void XBirdClient::OnCheckCrowded()
{
	__super::OnCheckCrowded();

	if (IsCrowded())
	{
		return;
	}

	size_t nCommandCount = m_CommandManager.GetCommandQueueCount();
	if ((size_t)DUMMYCLIENT_COMMAND_CROWDED_QUEUE_SIZE <= nCommandCount)
	{
		SetCrowded();		
	}
}

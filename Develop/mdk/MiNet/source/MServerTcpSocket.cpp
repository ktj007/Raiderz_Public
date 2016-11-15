#include "stdafx.h"
#include "MServerTcpSocket.h"
#include "MAcceptor.h"
#include "MDisconnector.h"
#include "MSender.h"
#include "MReceiver.h"

namespace minet {

bool MServerTcpSocket::Reuse()
{
	ResetSendPendingCounter();
	ReleaseLink();

	assert(m_pAcceptor);
	if (!m_pAcceptor->Register( this ))
	{
		minet_logd("minet> Socket Reuse Failed!!! socket(%d)\n", m_hSocket);
		return false;
	}
	return true;
}


MServerTcpSocket::MServerTcpSocket(MAcceptor* pAcceptor, MDisconnector* pDisconnector, MSender* pSender, MReceiver* pReceiver)
: MSocket(), m_pAcceptor(pAcceptor), m_pDisconnector(pDisconnector), m_pSender(pSender), m_pReceiver(pReceiver), m_uidLink(MUID::ZERO)
, m_nSendPendingCounter(0)
, m_nSendPendingLimitCount(INT_MAX)
, m_addressFamily(AF_INET)
{

}

MServerTcpSocket::~MServerTcpSocket()
{
	m_pAcceptor = NULL;
	m_pDisconnector = NULL;
	m_pSender = NULL;
	m_pReceiver = NULL;
	m_uidLink = MUID::ZERO;
	m_nSendPendingCounter = 0;
}

bool MServerTcpSocket::Init(int addressFamily)
{
	if (CreateSocket(addressFamily, SOCK_STREAM, IPPROTO_TCP) == INVALID_SOCKET)
	{
		return false;
	}

	m_addressFamily = addressFamily;

	InitBuf();
	ResetSendPendingCounter();	

	return true;
}

bool MServerTcpSocket::ReInit()
{
	return Init(m_addressFamily);
}

void MServerTcpSocket::InitBuf()
{
	ZeroMemory( m_RecvBuffer, SOCK_BUFSIZE );
	ZeroMemory( m_AcceptBuffer, SOCK_BUFSIZE );
}

void MServerTcpSocket::Recv()
{
	DWORD recvbytes = 0;
	DWORD flags = 0;


	MTcpActRecv* pTcpActRecv = new MTcpActRecv();
	pTcpActRecv->Init( m_pReceiver, this, m_uidLink, m_RecvBuffer, SOCK_BUFSIZE );

	WSABUF wsaBuf;
	wsaBuf.buf = m_RecvBuffer;
	wsaBuf.len = SOCK_BUFSIZE;

	int ret	= WSARecv( m_hSocket, &(wsaBuf), 1, &recvbytes, &flags, pTcpActRecv, NULL );

	if( ret == SOCKET_ERROR )
	{
		int error = WSAGetLastError();

		if( error != ERROR_IO_PENDING )
		{
			minet_logd( "WSARecv() Error!!! s(%d) err(%d)\n", m_hSocket, error );
//			Disconnect();

			SAFE_DELETE(pTcpActRecv);
		}
	}
}

void MServerTcpSocket::Send( BYTE* buf, int buflen )
{
	MTcpActSend* pTcpActSend = new MTcpActSend();
	bool isAllowSend = pTcpActSend->Init(m_pSender, this, m_uidLink, reinterpret_cast<char*>(buf), buflen);

	if (!isAllowSend)
	{		
		//minet_logd( "WSASend() SendPendingError!!!\n");
		SAFE_DELETE(pTcpActSend);				
		Disconnect();
		return;
	}


	WSABUF wsaBuf;
	wsaBuf.buf	= reinterpret_cast<char*>(buf);
	wsaBuf.len	= buflen;

	DWORD dwSendNumBytes = 0;
	DWORD dwFlags = 0;

	INT ret	= WSASend(m_hSocket, 
					  &(wsaBuf), 
					  1, 
					  &dwSendNumBytes, 
					  dwFlags, 
					  pTcpActSend, 
					  NULL);
	

	if( ret == SOCKET_ERROR )
	{
		int error = WSAGetLastError();

		if( error != ERROR_IO_PENDING )
		{
			// minet_logd( "WSASend() Error!!! socket(%d) buflen(%d), err(%d)\n", m_hSocket, buflen, error );
			
			SAFE_DELETE(pTcpActSend);
			return;
		}
	}
}

bool MServerTcpSocket::Disconnect()
{
	Shutdown(SD_BOTH);

	MTcpActDisconnect* pTcpActDisconnect = new MTcpActDisconnect();
	pTcpActDisconnect->Init(m_pDisconnector, this, m_uidLink);

	if (pTcpActDisconnect->m_uidLink == MUID::ZERO)
	{
		minet_logd("uid가 0 - MServerTcpSocket::Disconnect()\n");
		return false;
	}

	BOOL ret = TransmitFile(	
		m_hSocket, 
		NULL, 
		0, 
		0, 
		pTcpActDisconnect, 
		NULL, 
		TF_DISCONNECT | TF_REUSE_SOCKET
		);

	if( ret == FALSE )
	{
		int error = WSAGetLastError();

		if( error != WSA_IO_PENDING )
		{
			SAFE_DELETE(pTcpActDisconnect);

			minet_logd("MServerTcpSocket::Disconnect() Error!!! socket(%d), err(%d)\n", m_hSocket, error);

			return false;
		}
	}

	return true;
}

void MServerTcpSocket::Shutdown( int how /*= SD_BOTH*/ )
{
	LINGER  lingerStruct = {1, 0};
	setsockopt( m_hSocket, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct) ); 

	int ret = shutdown( m_hSocket, how );

	if (ret != 0)
	{
		int error = WSAGetLastError();

//		minet_logd("MServerTcpSocket::Shutdown() Error!!! socket(%d), err(%d)\n", m_hSocket, error);
	}
}

void MServerTcpSocket::HardDisconnect(MUID uidLink)
{
	m_pDisconnector->HardDisconnect(this, uidLink);
}

void MServerTcpSocket::ReleaseLink()
{
	m_uidLink = MUID::ZERO;
}

void MServerTcpSocket::Fini()
{
	ResetSendPendingCounter();

	ReleaseLink();

	CloseSocket();
}

void MServerTcpSocket::ResetSendPendingCounter()
{
	m_nSendPendingCounter = 0;
}

bool MServerTcpSocket::IncreaseSendPendingCounter()
{
	// 기준치를 넘게 쌓였으면, 수신측에 문제가 
	if (m_nSendPendingLimitCount <= m_nSendPendingCounter)
	{
		return false;
	}

	InterlockedIncrement((LONG*)&m_nSendPendingCounter);
	
	return true;
}

void MServerTcpSocket::DecreaseSendPendingCounter()
{
	// Increase와 호응하여 사용됨으로, 항상 양수다.
	if (m_nSendPendingCounter <= 0)
	{
		minet_logd("minet> SendPendingCounter negative number error.\n");
		return;
	}

	InterlockedDecrement((LONG*)&m_nSendPendingCounter);
}

void MServerTcpSocket::SetSendPendingLimitCount(int nLimitCount)
{
	if (nLimitCount < 0)	return;

	m_nSendPendingLimitCount = nLimitCount;
}

}

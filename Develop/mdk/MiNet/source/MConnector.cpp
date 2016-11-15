#include "stdafx.h"
#include "MConnector.h"
#include "MServerTcpSocketPool.h"
#include "MServerTcpSocket.h"
#include "MProactor.h"
#include "MUtil.h"

#include <WS2tcpip.h>


namespace minet {

namespace
{
	// 주소 해석 헬퍼. from windows programming for micro windows 2e
	addrinfo* const ResolveAddress(const char* const addr, const char* const port, int af, int type, int proto)
	{
		addrinfo hints;
		addrinfo* res = NULL;
		int rc;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_flags  = ((addr) ? 0 : AI_PASSIVE);
		hints.ai_family = af;
		hints.ai_socktype = type;
		hints.ai_protocol = proto;

		rc = getaddrinfo( addr, port, &hints, &res);
		if (0 != rc)
		{
			return NULL;
		}
		return res;
	}

	// addr구조체에 포트 설정 헬퍼. from windows programming for micro windows 2e
	void SetPort(int af, SOCKADDR* const sa, USHORT port)
	{
		assert(NULL!=sa);
		if (AF_INET == af)
		{
			(rcast<SOCKADDR_IN*>(sa))->sin_port = htons(port);
		}
		else if (AF_INET6 == af)
		{
			(rcast<SOCKADDR_IN6*>(sa))->sin6_port = htons(port);
		} else {
			// 알 수 없는 어드레스 패밀리
			assert(0);
		}
	}

	LPFN_CONNECTEX g_connectExFunc = NULL;
	bool LoadConnectExFunc(const SOCKET& socket)
	{
		if (NULL != g_connectExFunc)
		{
			return true;
		}

		GUID guidConnectEx = WSAID_CONNECTEX;
		DWORD bytes = 0;
		
		return SOCKET_ERROR != (
			WSAIoctl(
				socket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guidConnectEx,
				sizeof(guidConnectEx),
				&g_connectExFunc,
				sizeof(g_connectExFunc),
				&bytes, NULL, NULL)
			);
	}
}

MConnector::MConnector(MProactor* const proactor,MServerTcpSocketPool* const socketPool)
: m_pListener(NULL)
, m_pSocketPool(socketPool)
{
	m_pProactor = proactor;
	assert(NULL!=m_pProactor);
}

MConnector::~MConnector()
{
}

MServerTcpSocket* const MConnector::Connect(const char* const addr,const char* const port)
{
	if (NULL == addr)
	{
		return NULL;
	}

	if (NULL == port)
	{
		return NULL;
	}

	class AutoReleaseAddr
	{
	public:
		AutoReleaseAddr(addrinfo* const addr) : m_addr(addr) { assert(NULL!=m_addr); }
		~AutoReleaseAddr() { assert(NULL!=m_addr); freeaddrinfo(m_addr); }

	private:
		addrinfo* const m_addr;
	};

	addrinfo* const servAddr = ResolveAddress(addr, port, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	if (NULL == servAddr)
	{
		return false;
	}
	AutoReleaseAddr reserveDelServeAddr(servAddr);

	addrinfo* curAddr = servAddr;
	MServerTcpSocket* const connectSocket = NULL;
	while (NULL != curAddr)
	{
		// 대상 주소 포트의 ipv4 또는 ipv6 tcp/ip정보를 얻음
		if ((AF_INET != curAddr->ai_family && AF_INET6 != curAddr->ai_family) ||
			SOCK_STREAM != curAddr->ai_socktype ||
			IPPROTO_TCP != curAddr->ai_protocol)
		{
			curAddr = curAddr->ai_next;
			continue;
		}

		addrinfo* const localAddr = ResolveAddress(NULL, "0", curAddr->ai_family, curAddr->ai_socktype, curAddr->ai_protocol);
		if (NULL == localAddr)
		{
			continue;
		}
		AutoReleaseAddr reserveDelLocalAddr(localAddr);

		assert(NULL!=m_pSocketPool);
		MServerTcpSocket* const pSocket = m_pSocketPool->AddSocket(curAddr->ai_family);
		if (NULL == pSocket)
		{
			continue;
		}

		if (false == LoadConnectExFunc(pSocket->GetSocket()))
		{
			continue;
		}

		pSocket->SetSendPendingLimitCount(INT_MAX);

		assert(NULL!=m_pProactor);
		if (false == m_pProactor->RegisterToCompletionPort(rcast<HANDLE>(pSocket->GetSocket())))
		{
			continue;
		}
		
		
		SetPort(localAddr->ai_family, localAddr->ai_addr, 0);
		if (SOCKET_ERROR == bind(pSocket->GetSocket(), localAddr->ai_addr, localAddr->ai_addrlen))
		{
			m_pSocketPool->Restore(pSocket);
			return NULL;
		}

		// 접속지의 addr정보를 저장
		CopyMemory(&(pSocket->m_remoteAddr), curAddr->ai_addr, curAddr->ai_addrlen);
		pSocket->m_remoteAddrLen = curAddr->ai_addrlen;

		MTcpActConnect* actConnect = new MTcpActConnect(this, pSocket);
		if (NULL == actConnect)
		{
			continue;
		}
		AutoDelete<MTcpActConnect> autoDel(actConnect);

		DWORD bytes;
		if (FALSE == g_connectExFunc(pSocket->GetSocket(), rcast<SOCKADDR*>(&pSocket->m_remoteAddr),
			pSocket->m_remoteAddrLen, NULL, 0, &bytes, actConnect) &&
			WSA_IO_PENDING != WSAGetLastError() )
		{
			continue;
		}

		autoDel.Cancel();
		return pSocket;
	}

	return NULL;
}

void MConnector::SetListener(MConnectListener* const pListner)
{
	m_pListener = pListner;
}

void MConnector::ProcEvent(MAct* act,DWORD bytes_transferred)
{
	assert(NULL!=act);
	MTcpAct* pTcpAct = dcast<MTcpAct*>(act);
	assert(NULL!=pTcpAct);

	assert(pTcpAct->m_pTcpSocket);
	MServerTcpSocket& tcpsocket = *(pTcpAct->m_pTcpSocket);

	HANDLE hSocketHandle = rcast<HANDLE>(tcpsocket.GetSocket());
	assert(m_pProactor);
	m_pProactor->RegisterToCompletionPort(hSocketHandle);

	if (NULL != m_pListener)
	{
		m_pListener->OnConnect(tcpsocket);
	}
	tcpsocket.Recv();
}

void MConnector::ProcError(MAct* act,DWORD bytes_transferred,DWORD erorr)
{
	assert(NULL!=act);
	MTcpAct* pTcpAct = dcast<MTcpAct*>(act);
	assert(NULL!=pTcpAct);
	assert(pTcpAct->m_pTcpSocket);

	MServerTcpSocket* pTcpSocket = pTcpAct->m_pTcpSocket;
	pTcpSocket->Fini();
	m_pSocketPool->Restore(pTcpSocket);
}

}
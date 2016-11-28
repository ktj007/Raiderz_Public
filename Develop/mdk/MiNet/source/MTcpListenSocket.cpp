#include "stdafx.h"
#include "MTcpListenSocket.h"

#if (_MSC_VER >= 1900)
#include <WS2tcpip.h>
#endif

namespace minet {

MTcpListenSocket::MTcpListenSocket() : MSocket()
{

}

bool MTcpListenSocket::Listen( MProactor* pProactor )
{
	pProactor->RegisterToCompletionPort(reinterpret_cast<HANDLE>(m_hSocket));

	int ret = bind(m_hSocket, (SOCKADDR*)&m_Addr, sizeof(m_Addr));
	if (SOCKET_ERROR == ret)
	{
		int nLastError = WSAGetLastError();
		if(nLastError == WSAEADDRINUSE)
		{
			minet_log("minet> address already in use (port=%d) \n", ntohs(m_Addr.sin_port));
		}
		minet_log("minet> bind: (error=%d)\n", nLastError);
		return false;
	}

	int backlog = 16;	// 연결 요청 대기큐의 크기
	ret = listen(m_hSocket, backlog);
	if (SOCKET_ERROR == ret)
	{
		minet_log("minet> listen: (error=%d)\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool MTcpListenSocket::Init( int addressFamily, WORD port, bool reuse, const char* szMyNetworkCardIP )
{
	if (CreateSocket(addressFamily, SOCK_STREAM, IPPROTO_TCP) == INVALID_SOCKET) return false;

	if (reuse)
	{
		int opt = 1;
		if (setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
		{
			minet_log("minet> cannot make reuse socket (error=%d)\n", WSAGetLastError());
			return false;
		}
	}

	// 주소 결함
	m_Addr.sin_family = AF_INET;
	m_Addr.sin_port = htons(port);

	DWORD dwAddr = INADDR_NONE;

	if (NULL == szMyNetworkCardIP ||
		0 == strlen(szMyNetworkCardIP) ||
		0 == strcmp(szMyNetworkCardIP, "localhost") ||
		0 == strcmp(szMyNetworkCardIP, "127.0.0.1"))
	{
		dwAddr = INADDR_ANY;
	}
	else
	{
#if (_MSC_VER >= 1900)
		IN_ADDR inAddr;
		if (inet_pton(AF_INET, szMyNetworkCardIP, &inAddr))
			dwAddr = inAddr.s_addr;
#else
		dwAddr = inet_addr(szMyNetworkCardIP);
#endif
		if (INADDR_NONE == dwAddr)
		{
			// 연결할 host name을 입력한 경우
#if (_MSC_VER >= 1900)
			ADDRINFO* pAddrInfo;

			ADDRINFO AddrHints = { 0 };
			AddrHints.ai_family		= AF_INET;
			AddrHints.ai_protocol	= IPPROTO_TCP;
			AddrHints.ai_socktype	= SOCK_STREAM;

			if (getaddrinfo(szMyNetworkCardIP, NULL, &AddrHints, &pAddrInfo) != 0)
#else
			HOSTENT* pHost = gethostbyname(szMyNetworkCardIP);
			if (pHost == NULL)	// error
#endif
			{
				minet_log("minet> MTcpListenSocket::Init(), Can't resolve hostname (Input=\"%s\")\n", szMyNetworkCardIP);
				return false;
			}
#if (_MSC_VER >= 1900)
			dwAddr = ((SOCKADDR_IN*)pAddrInfo->ai_addr)->sin_addr.s_addr;
#else
			memcpy((char FAR *)&(dwAddr), pHost->h_addr, pHost->h_length);
#endif
		}
		minet_log("minet> listen socket binding ip: (IP=\"%s\")\n", szMyNetworkCardIP);
	}

	m_Addr.sin_addr.s_addr = htonl(dwAddr);

	return true;
}




} // namespace minet {
#include "stdafx.h"
#include "MSocket.h"

#if (_MSC_VER >= 1900)
#include <WS2tcpip.h>
#endif

namespace minet {

	MSocket::MSocket()
		: m_hSocket(INVALID_SOCKET)
		, m_remoteAddrLen(0)
	{
		ZeroMemory(&m_Addr, sizeof(m_Addr));
		ZeroMemory(&m_remoteAddr, sizeof(m_remoteAddr));
	}

	SOCKET MSocket::CreateSocket( int addressFamily, int socketType, int protocol )
	{
		int         nRet;
		int         nZero = 0;
		LINGER      lingerStruct;
		SOCKET      sdSocket = INVALID_SOCKET;

#if (_MSC_VER >= 1900)
		sdSocket = WSASocketW(addressFamily, socketType, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
		sdSocket = WSASocket(addressFamily, socketType, protocol, NULL, 0, WSA_FLAG_OVERLAPPED); 
#endif
		if (INVALID_SOCKET == sdSocket) 
		{
			minet_logd("minet> WSASocket(sdSocket): (error=%d)\n", WSAGetLastError());
			return INVALID_SOCKET;
		}

		m_hSocket = sdSocket;

		BOOL val = TRUE;
		nRet = setsockopt(sdSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&val, sizeof(BOOL) );
		if (SOCKET_ERROR == nRet) 
		{
			minet_logd("minet> setsockopt(TCP_NODELAY): (error=%d)\n", WSAGetLastError());
			return sdSocket;
		}

		// graceful disconnect을 위해 LINGER 설정
		lingerStruct.l_onoff = 1;
		lingerStruct.l_linger = 0;
		nRet = setsockopt(sdSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));
		if (SOCKET_ERROR == nRet) 
		{
			minet_logd("minet> setsockopt(SO_LINGER): (error=%d)\n", WSAGetLastError());
			return sdSocket;
		}

		return sdSocket;
	}

	void MSocket::CloseSocket()
	{
		if (m_hSocket != INVALID_SOCKET)
		{
			closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
		}
	}

	std::string MSocket::GetRemoteIP() const
	{
#if (_MSC_VER >= 1900)
		char szIP[64];
		SOCKADDR_STORAGE remoteAddr = m_remoteAddr;

		if (remoteAddr.ss_family == AF_INET)
		{
			if (inet_ntop(AF_INET, &(((SOCKADDR_IN*)&remoteAddr)->sin_addr), szIP, 64))
				return std::string(szIP);
		}
		else if (remoteAddr.ss_family == AF_INET6)
		{
			if (inet_ntop(AF_INET6, &(((SOCKADDR_IN6*)&remoteAddr)->sin6_addr), szIP, 64))
				return std::string(szIP);
		}

		return std::string();	// error.
#else
		if (m_remoteAddr.ss_family == AF_INET)
			return std::string(inet_ntoa(((const SOCKADDR_IN*)&m_remoteAddr)->sin_addr));

		return std::string();
#endif
	}

} // namespace minet
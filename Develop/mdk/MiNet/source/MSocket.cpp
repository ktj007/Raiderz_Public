#include "stdafx.h"
#include "MSocket.h"

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

		sdSocket = WSASocket(addressFamily, socketType, protocol, NULL, 0, WSA_FLAG_OVERLAPPED); 
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

} // namespace minet
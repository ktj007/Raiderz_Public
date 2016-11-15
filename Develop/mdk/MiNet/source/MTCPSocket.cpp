#include "stdafx.h"
#include "MTCPSocket.h"
#include "MDebug.h"

namespace minet {


#define TCPSOCKET_MAX_SENDQUEUE_LEN			5120
#define MAX_CLIENTSOCKET_LEN				60

////////////////////////////////////////////////////////////////////////////////////////////
// MTCPSocketThread ///////////////////////////////////////////////////////////////////////////
MTCPSocketThread::MTCPSocketThread(MTCPSocket* pTCPSocket) : MThread()
{
	m_pTCPSocket = pTCPSocket;
	m_fnSocketErrorCallback = NULL;
	m_pCallbackContext = NULL;


	m_bActive = false;
	m_fnRecvCallback = NULL;
	m_fnConnectCallback = NULL;
	m_fnDisconnectCallback = NULL;

	memset(m_RecvBuf, 0, sizeof(m_RecvBuf));
}

MTCPSocketThread::~MTCPSocketThread()
{
	
}

void MTCPSocketThread::OnSocketError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode)
{
	if (m_fnSocketErrorCallback)
		m_fnSocketErrorCallback(m_pCallbackContext, sock, ErrorEvent, ErrorCode);
}

bool MTCPSocketThread::OnConnect(SOCKET sock)
{
	m_bActive = true;

	if (m_fnConnectCallback)
		return m_fnConnectCallback(m_pCallbackContext, sock);

	return false;
}

bool MTCPSocketThread::OnDisconnect()
{
	m_pTCPSocket->Log("MTCPSocketThread::OnDisconnect() [S]");
	m_bActive = false;

	if (m_fnDisconnectCallback)
		return m_fnDisconnectCallback(m_pCallbackContext);

	m_pTCPSocket->Log("MTCPSocketThread::OnDisconnect() [F]");
	return false;
}

void MTCPSocketThread::ClearSendList()
{
	LockSend();
	for (TCPSendList::iterator itor = m_SendList.begin(); itor != m_SendList.end(); )
	{
		MTCPSendQueueNode*	pNode = (*itor);
		delete [] pNode->pPacket;
		pNode->pPacket = NULL;

		delete pNode;
		pNode = NULL;

		++itor;
	}
	m_SendList.clear();

	for (TCPSendList::iterator itor = m_TempSendList.begin(); itor != m_TempSendList.end(); )
	{
		MTCPSendQueueNode*	pNode = (*itor);
		delete [] pNode->pPacket;
		pNode->pPacket = NULL;

		delete pNode;
		pNode = NULL;

		++itor;
	}
	m_TempSendList.clear();

	UnlockSend();
}

bool MTCPSocketThread::FlushSend()
{
	TCPSendList::iterator SendItor;

	LockSend();
		while (!m_TempSendList.empty())
		{
			TCPSendList::iterator itor = m_TempSendList.begin();
			m_SendList.push_back(*itor);
			m_TempSendList.erase(itor);
		}
	UnlockSend();

	while(!m_SendList.empty())
	{
		SendItor = m_SendList.begin();
		MTCPSendQueueNode* pSendNode = (MTCPSendQueueNode*)(*SendItor);

		unsigned int nTransBytes = 0;
		while(TRUE) 
		{
			int nSent = send(m_pTCPSocket->GetSocket(), (char*)pSendNode->pPacket+nTransBytes,
							pSendNode->dwPacketSize-nTransBytes, 0);

			if (nSent == SOCKET_ERROR) 
			{
				int nErrCode = WSAGetLastError();

#ifdef _DEBUG
				{
					char szBuf[64]; 
					sprintf_s(szBuf, "FLUSHSEND> FlushSend Error(%d)\n", nErrCode);
					minet_log(szBuf);
				}
#endif

				if ( (nErrCode == WSAECONNABORTED)	||
					 (nErrCode == WSAECONNRESET)	||
					 (nErrCode == WSAETIMEDOUT) ) 
				{
#ifdef _DEBUG
					 minet_log("FLUSHSEND> Connection ERROR Closed!!!!!!!!!! \n");
#endif
					 return false;
				}
				
				if (nErrCode == WSAEWOULDBLOCK) continue;
				break;
			} 
			else
			{
				nTransBytes += nSent;
				if (nTransBytes >= pSendNode->dwPacketSize) break;
			}
		} // while

		m_TrafficLog.OnSend(nTransBytes);

		
		if (pSendNode != NULL)
		{
			delete [] pSendNode->pPacket;
			pSendNode->pPacket = NULL;

			delete pSendNode;
			pSendNode = NULL;
		}

		LockSend();
			m_SendList.erase(SendItor);
		UnlockSend();
	}

	return true;
}


bool MTCPSocketThread::Recv()
{
	int				nRecv = 0;

	while(TRUE) 
	{
		nRecv = recv(m_pTCPSocket->GetSocket(), m_RecvBuf, MAX_RECVBUF_LEN, 0);
		if (nRecv != SOCKET_ERROR) 
		{
			m_TrafficLog.OnRecv(nRecv);
		}

		if (nRecv <= 0) break;

		if (OnRecv(m_pTCPSocket->GetSocket(), m_RecvBuf, nRecv) == true)
		{
			continue;
		}
	}

	return true;
}

bool MTCPSocketThread::OnRecv(SOCKET socket, char *pPacket, DWORD dwSize)
{
	if (m_fnRecvCallback) return m_fnRecvCallback(m_pCallbackContext, socket, pPacket, dwSize);

	return true;
}

bool MTCPSocketThread::PushSend(char *pPacket, DWORD dwPacketSize)
{
	size_t nSendListSize = 0;

	LockSend();
		nSendListSize = m_SendList.size();
	UnlockSend();

	if ((!m_bActive) || (nSendListSize > TCPSOCKET_MAX_SENDQUEUE_LEN)) 
	{
		delete [] pPacket;
		return false;
	}
	
	_ASSERT(dwPacketSize > 0);
	if (dwPacketSize == 0) 
	{
		return false;
	}


	MTCPSendQueueNode* pSendNode = new MTCPSendQueueNode();

	pSendNode->pPacket = pPacket;
	pSendNode->dwPacketSize = dwPacketSize;

	LockSend();
		m_TempSendList.push_back(pSendNode);
	UnlockSend();

	m_SendEvent.Signal();

	return true;
}

void MTCPSocketThread::Run()
{
	m_bActive = true;

	while(true) 
	{	
		if (m_KillEvent.Wait(100))
		{
			m_KillEvent.Signal(); 
			return;
		}
		else
		{
			if (m_pTCPSocket)
				break;
		}
	}

	RunSocket();

	m_bActive = false;

	//while(true)
	//{
	//	Sleep(1);
	//}
}

void MTCPSocketThread::RunSocket()
{
	m_pTCPSocket->Log("MTCPSocketThread::RunSocket[s]");
	char thread_name[256]="";
	sprintf_s(thread_name, "minetThread(%u)", m_pTCPSocket->GetSocket());
	cml2::SetThisThreadName(thread_name);

	if (m_pTCPSocket->GetSocket() == INVALID_SOCKET)
	{
		m_pTCPSocket->OuputLog();
		return;
	}

	// 이벤트 타입 설명
	// - FD_READ : 수신할 데이터가 있을 때 통지 받음
	// - FD_WRITE : 전송이 완료되었을 때 통지 받음
	// - FD_CONNECT : 연결이 완료되거나 멀티캐스트 가입이 완료되었을 때 통지 받음
	// - FD_CLOSE : 소켓이 종료되었을 때 통지 받음

	WSANETWORKEVENTS NetEvent;
	WSAEVENT hFDEvent = WSACreateEvent();
	int result;
	result = WSAEventSelect(m_pTCPSocket->GetSocket(), hFDEvent, FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE);
	if (SOCKET_ERROR == result)
	{
		int error = WSAGetLastError();
#ifdef _DEBUG
		minet_log("<SOCKETTHREAD_ERROR> WSAEventSelect ERROR(%d)</SOCKETTHREAD_ERROR>\n", error);
#endif
		_ASSERT(m_pTCPSocket->GetSocket() == INVALID_SOCKET);
		return;
	}

	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	WORD wEventIndex = 0;

	EventArray[wEventIndex++] = hFDEvent;
	EventArray[wEventIndex++] = m_SendEvent.GetEvent();
	EventArray[wEventIndex++] = m_KillEvent.GetEvent();

	bool bEnd = false;
	while (!bEnd && !Terminated())
	{
		result = WSAWaitForMultipleEvents(wEventIndex, EventArray, FALSE, 100, FALSE);
		if (result == WSA_WAIT_FAILED) continue;

		switch (result)
		{
		case WAIT_OBJECT_0:		// Socket Event
			{
				result = WSAEnumNetworkEvents(m_pTCPSocket->GetSocket(), hFDEvent, &NetEvent);

				if (SOCKET_ERROR == result)
				{
					int error = WSAGetLastError();
					if (error != WSAENOTSOCK)
					{
#ifdef _DEBUG
						m_pTCPSocket->Log("<SOCKETTHREAD_ERROR> WSAEnumNetworkEvents Error (%d)</SOCKETTHREAD_ERROR>\n");
						minet_log("<SOCKETTHREAD_ERROR> WSAEnumNetworkEvents Error (%d)</SOCKETTHREAD_ERROR>\n", error);
#endif
					}

					ResetEvent(hFDEvent);
					continue;
				}

				if (NetEvent.lNetworkEvents & FD_CONNECT)
				{
					if (NetEvent.iErrorCode[FD_CONNECT_BIT] == 0)
					{
#ifdef _DEBUG
						m_pTCPSocket->Log("FD_CONNECT : OnConnect");
#endif
						OnConnect(m_pTCPSocket->GetSocket());
					}
					else
					{
#ifdef _DEBUG
						m_pTCPSocket->Log("FD_CONNECT : SockError");
						minet_log("SockError(FD_CONNECT): socket(%d), error(%d)\n", m_pTCPSocket->GetSocket(), NetEvent.iErrorCode[FD_CONNECT_BIT]);
#endif
						OnSocketError(m_pTCPSocket->GetSocket(), eeConnect, NetEvent.iErrorCode[FD_CONNECT_BIT]);
						m_pTCPSocket->CloseSocket();

						_ASSERT(m_pTCPSocket->GetSocket() == INVALID_SOCKET);
						bEnd = true;
					}
				}

				if (NetEvent.lNetworkEvents & FD_READ)
				{
					if (NetEvent.iErrorCode[FD_READ_BIT] == 0)
					{
						if (m_bActive) Recv();
					}
					else
					{
#ifdef _DEBUG
						minet_log("SockError(FD_READ): socket(%d), error(%d)\n", m_pTCPSocket->GetSocket(), NetEvent.iErrorCode[FD_READ_BIT]);
#endif
						OnSocketError(m_pTCPSocket->GetSocket(), eeReceive, NetEvent.iErrorCode[FD_READ_BIT]);
					}
				}

				if (NetEvent.lNetworkEvents & FD_WRITE)
				{
					if (NetEvent.iErrorCode[FD_WRITE_BIT] == 0)
					{
						if (m_bActive) FlushSend();
					}
					else
					{
#ifdef _DEBUG
						minet_log("SockError(FD_WRITE): socket(%d), error(%d)\n", m_pTCPSocket->GetSocket(), NetEvent.iErrorCode[FD_WRITE_BIT]);
#endif
						OnSocketError(m_pTCPSocket->GetSocket(), eeSend, NetEvent.iErrorCode[FD_WRITE_BIT]);
					}
				}

				if (NetEvent.lNetworkEvents & FD_CLOSE)
				{
					if (NetEvent.iErrorCode[FD_CLOSE_BIT] != 0)
					{
#ifdef _DEBUG
						m_pTCPSocket->Log("FD_CLOSE : SockError");
						minet_log("SockError(FD_CLOSE): socket(%d), error(%d)\n", m_pTCPSocket->GetSocket(), NetEvent.iErrorCode[FD_CLOSE_BIT]);
#endif
						OnSocketError(m_pTCPSocket->GetSocket(), eeDisconnect, NetEvent.iErrorCode[FD_CLOSE_BIT]);
					}

					if (m_bActive) Recv();
					m_pTCPSocket->CloseSocket();

#ifdef _DEBUG
					m_pTCPSocket->Log("FD_CLOSE : OnDisconnect");
#endif
					OnDisconnect();

					_ASSERT(m_pTCPSocket->GetSocket() == INVALID_SOCKET);
					bEnd = true;
				}
			}
			break;
		case WAIT_OBJECT_0 + 1:		// Send Event
			{
				if (m_bActive) FlushSend();
			}
			break;
		case WAIT_OBJECT_0 + 2:		// Kill Event
			{
				_ASSERT(m_pTCPSocket->GetSocket() == INVALID_SOCKET);
				bEnd = true;
			}
			break;
		case WSA_WAIT_TIMEOUT:
			{
				if (m_bActive && GetSendWaitQueueCount() > 0)
					FlushSend();
			}
			break;
		default:
			{
#ifdef _DEBUG
				minet_log("<SOCKETTHREAD_ERROR> exceptional case </SOCKETTHREAD_ERROR>\n");
#endif
				bEnd = true;
			}
			break;
		}	// switch

	}	// while

	WSACloseEvent(hFDEvent);

	ClearSendList();

	m_pTCPSocket->Log("MTCPSocketThread::RunSocket[F]");
}


void MTCPSocketThread::Kill()
{
	m_KillEvent.Signal();
}


////////////////////////////////////////////////////////////////////////////////////////////
// MTCPSocket ///////////////////////////////////////////////////////////////////////////
MTCPSocket::MTCPSocket() : m_bInitialized(false), m_nPort(0), m_Socket(INVALID_SOCKET), m_bSocketReuse(false), m_pSocketThread(NULL)
{
	Initialize();
}

MTCPSocket::MTCPSocket(bool bSocketReuse) : m_bInitialized(false), m_nPort(0), m_Socket(INVALID_SOCKET), m_bSocketReuse(bSocketReuse), m_pSocketThread(NULL)
{
	Initialize();
}

MTCPSocket::~MTCPSocket()
{
	Finalize();
}

bool MTCPSocket::Initialize()
{
	m_nStatus = INITIALIZED;

	WSADATA	wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
		return false;

	if ( (LOBYTE( wsaData.wVersion ) != 2) || (HIBYTE( wsaData.wVersion ) != 2) ) 
	{
		minet_log("<TCPSOCKET_ERROR> WinSock Version invalid </TCPSOCKET_ERROR>\n");
		return false;
	} 

	m_bInitialized = true;
	m_pSocketThread = new MTCPSocketThread(this);

	return true;
}

void MTCPSocket::Finalize()
{
	if (!m_bInitialized) return;

	CloseSocket();


	if (m_pSocketThread)
	{
		m_pSocketThread->Stop();
		SAFE_DELETE(m_pSocketThread);
	}

	WSACleanup();

	m_bInitialized = false;
}

bool MTCPSocket::OpenSocket()
{
	Log("OpenSocket[S]");

	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) 
	{
		minet_log("debug> OpenSocket Failed(1)\n");
		return false;
	}

	m_SocketLock.Lock();
	_ASSERT(m_Socket == INVALID_SOCKET);
	m_Socket = sockfd;

	m_nStatus = OPENED;
	m_SocketLock.Unlock();


	_ASSERT(m_Socket != INVALID_SOCKET);
	unsigned long option = 1;
	int nRet = ::ioctlsocket(m_Socket, FIONBIO, (unsigned long*)&option);
	if (SOCKET_ERROR == nRet)
	{
		minet_log("<TCPSOCKET_ERROR> ioctl failed </TCPSOCKET_ERROR>\n");
		return false;
	}

	BOOL val = TRUE;
	nRet = ::setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&val, sizeof(BOOL) );
	if (SOCKET_ERROR == nRet) 
	{
		minet_log("<TCPSOCKET_ERROR> setsockopt(TCP_NODELAY) \n");
		return false;
	}

	LINGER  lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));

	// reuse
	if (m_bSocketReuse)
	{
		int opt = 1;
		if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
		{
			minet_log("minet> cannot make reuse socket (error=%d)\n", WSAGetLastError());
			return false;
		}
	}

	Log("OpenSocket[F]");

	return true;
}

void MTCPSocket::CloseSocket()
{
	Log("CloseSocket[S]");

	m_SocketLock.Lock();

	if (m_Socket != INVALID_SOCKET)
	{
		shutdown(m_Socket, SD_BOTH );
		int ret = closesocket(m_Socket);

		m_nStatus = CLOSED;

		if (ret == SOCKET_ERROR)
		{
			minet_log("<minet> closesocket Error (%d)</minet>\n", WSAGetLastError());
		}

		m_Socket = INVALID_SOCKET;
	}

	m_SocketLock.Unlock();

	Log("CloseSocket[F]");
}

bool MTCPSocket::Connect( const char* szRemoteIP, int nPort, const char* szMyNetworkCardIP )
{
	Log("Connect[S]");

	if (m_Socket != INVALID_SOCKET) 
	{
		CloseSocket();
	}

	strcpy_s(m_szHost, szRemoteIP);
	m_nPort = nPort;

	m_pSocketThread->Stop();
	m_pSocketThread->Wait(10000);

	if (OpenSocket() == false)
	{
		minet_log("minet> failed OpenSocket\n");
		return false;
	}

	_ASSERT(m_Socket != INVALID_SOCKET);
	m_pSocketThread->Start();

	//////////////////////////////////////////////////////////////////////////
	// NetworkCard IP가 유효한 경우 Bind
	if (BindSocket(szMyNetworkCardIP) == false)
	{
		minet_log("minet> failed BindSocket\n");
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	sockaddr_in ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(ServerAddr));

	DWORD dwAddr = inet_addr(szRemoteIP);
	if (dwAddr != INADDR_NONE) 
	{
		memcpy(&(ServerAddr.sin_addr), &dwAddr, 4);
	} 
	else 
	{		
		// 연결할 host name을 입력한 경우
		HOSTENT* pHost = gethostbyname(szRemoteIP);
		if (pHost == NULL)	// error
		{	
			minet_log("<TCPSOCKET_ERROR> Can't resolve hostname </TCPSOCKET_ERROR>");
			return false;
		}
		memcpy((char FAR *)&(ServerAddr.sin_addr), pHost->h_addr, pHost->h_length);
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(nPort);

	DWORD result = connect(m_Socket, (sockaddr*)&ServerAddr, sizeof(sockaddr_in));
	if (SOCKET_ERROR == result)	
	{
		DWORD dwError = WSAGetLastError();
		if (dwError != WSAEWOULDBLOCK) 
		{
			char szLog[64];
			sprintf_s(szLog, "Connect Failed (ErrorCode=%d) \n", dwError);
			minet_log(szLog);

			CloseSocket();
			m_pSocketThread->Stop();

			return false;
		}
	}

	Log("Connect[F]");
	return true;
}

bool MTCPSocket::BindSocket( const char* szMyNetworkCardIP )
{
	if (NULL == szMyNetworkCardIP || 0 == strlen(szMyNetworkCardIP))
	{
//		minet_log("minet> MTCPSocket::BindSocket(), Skip Bind (Input=\"%s\")\n", szMyNetworkCardIP);
		return true;
	}

	if (0 == strcmp(szMyNetworkCardIP, "127.0.0.1") ||
		0 == strcmp(szMyNetworkCardIP, "localhost"))
	{
		minet_log("minet> MTCPSocket::BindSocket(), MyNetworkCardIP is not allowed localhost(or 127.0.0.1): (Input=\"%s\")\n", szMyNetworkCardIP);
		return false;
	}

	DWORD dwAddr = inet_addr(szMyNetworkCardIP);
	if (INADDR_NONE == dwAddr)
	{
		// 연결할 host name을 입력한 경우
		HOSTENT* pHost = gethostbyname(szMyNetworkCardIP);
		if (pHost == NULL)	// error
		{
			minet_log("minet> MTCPSocket::BindSocket(), Can't resolve hostname (Input=\"%s\")\n", szMyNetworkCardIP);
			return false;
		}
		memcpy((char FAR *)&(dwAddr), pHost->h_addr, pHost->h_length);
	}

	sockaddr_in ClientAddr;
	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_addr.s_addr = dwAddr;
	ClientAddr.sin_port = 0;

	int ret = bind(m_Socket, (SOCKADDR*)&ClientAddr, sizeof(ClientAddr));
	if (SOCKET_ERROR == ret)
	{
		int nLastError = WSAGetLastError();
		if(nLastError == WSAEADDRINUSE)
		{
			minet_log("minet> client socket address already in use (ip=\"%s\":port=%d) \n", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
		}
		minet_log("minet> client socket bind: (error=%d)\n", nLastError);
		return false;
	}
	minet_log("minet> client socket binding ip: (IP=\"%s\")\n", szMyNetworkCardIP);
	return true;
}

bool MTCPSocket::Disconnect()
{
	Log("Disconnect[S]");

	if (!IsActive()) 
	{
		return false;
	}

	m_SocketLock.Lock();
	if (m_Socket == INVALID_SOCKET)
	{
		m_SocketLock.Unlock();
		return false;
	}
	m_SocketLock.Unlock();

	shutdown(m_Socket, SD_BOTH );

	Log("Disconnect[F]");

	return true;
}

bool MTCPSocket::Send(char *pPacket, DWORD dwPacketSize)
{
	if ((!m_bInitialized)) 
	{
		delete [] pPacket;
		return false;
	}

	return ((MTCPSocketThread*)(m_pSocketThread))->PushSend(pPacket, dwPacketSize);
}


void MTCPSocket::GetTraffic(int* nSendTraffic, int* nRecvTraffic) 
{
	*nSendTraffic = m_pSocketThread->m_TrafficLog.GetSendTraffic();
	*nRecvTraffic = m_pSocketThread->m_TrafficLog.GetRecvTraffic();
}

void MTCPSocket::Log( const char* szLog )
{
	return;

	cml2::MCriticalSection::Guard guard(m_LogLock);

	char text[256] = "";

	DWORD thread_id = ::GetCurrentThreadId();
	sprintf_s(text, "%u) %s , time=%u", thread_id, szLog, timeGetTime());
	m_strLastFunc = text;
	m_FuncStr.push_back(m_strLastFunc);
}

uint32 MTCPSocket::GetTotalSendBytes()
{
	return m_pSocketThread->m_TrafficLog.GetTotalSendBytes();
}

uint32 MTCPSocket::GetTotalRecvBytes()
{
	return m_pSocketThread->m_TrafficLog.GetTotalRecvBytes();
}

} // namespace minet
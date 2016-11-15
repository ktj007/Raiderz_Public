#include "stdafx.h"
#include "MIocpNet.h"
#include "MDebug.h"
#include "MSession.h"

namespace minet {

MIocpNet::MIocpNet() : m_dwWorkerThreadCount(0), 
					   m_nPort(DEFAULT_SERVER_PORT), 
					   m_pListenSession(NULL), 
					   m_hIOCP(INVALID_HANDLE_VALUE), 
					   m_sdListen(INVALID_SOCKET),
					   m_bWorkThread(true),
					   m_fnCallback(NULL),
					   m_pCallbackContext(NULL),
					   m_bEndServer(false),
					   m_hCleanupEvent(NULL),
					   m_pIOCPAccept(NULL)
{
	
}

MIocpNet::~MIocpNet()
{
	DeleteAcceptSocket();
	if (m_pListenSession)
	{
		delete m_pListenSession; m_pListenSession = NULL;
	}
}


bool MIocpNet::InitializeSocket()
{
	WSADATA		wsaData;
	int			nRet;

	for (int i = 0; i < MAX_IOCP_WORKER_THREAD; i++)
	{
		m_ThreadHandles[i] = INVALID_HANDLE_VALUE;
	}

	if ((nRet = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0) 
	{
		mlog("MIocpNet> WSAStartup failed: %d\n", nRet);
		return false;
	}

	return true;
}

void MIocpNet::FinalizeSocket()
{
	DeleteAcceptSocket();

	if (m_sdListen != INVALID_SOCKET) 
	{
		closesocket(m_sdListen);                                
		m_sdListen = INVALID_SOCKET;
	}

	WSACleanup();
}

bool MIocpNet::Create(int nPort, const bool bReuse)
{
	m_hCleanupEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hCleanupEvent == NULL) {
		mlog("MIocpNet> CreateEvent(CleanupEvent) failed: %d\n",GetLastError());
		return false;
	}

	if (!InitializeSocket()) 
	{
		CloseHandle(m_hCleanupEvent);
		return false;
	}
	if (nPort != 0)	m_nPort = nPort;

	m_bEndServer = false;
	ResetEvent(m_hCleanupEvent);

	if (!CreateWorkerIOCP()) return false;
	if (!CreateWorkerThreads()) return false;

	if (!CreateListenSocket(bReuse)) return false;
    if (!CreateAcceptSocket(true)) return false;

	return true;
}

bool MIocpNet::CreateWorkerIOCP()
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_hIOCP) 
	{
		mlog("MIocpNet> CreateIoCompletionPort failed to create I/O completion port: %d\n", GetLastError());
        return false;
	}

	return true;
}

bool MIocpNet::CreateWorkerThreads()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	m_dwWorkerThreadCount = systemInfo.dwNumberOfProcessors * 2;

	for (DWORD dwCPU = 0; dwCPU < m_dwWorkerThreadCount; dwCPU++) 
	{
		HANDLE  hThread;
		DWORD   dwThreadId;

		hThread = CreateThread(NULL, 0, _WorkerThread, this, 0, &dwThreadId);
		if (hThread == NULL) 
		{
			mlog("RealCPNet> CreateThread failed to create worker thread: %d\n", GetLastError());
			return false;
		}

		m_ThreadHandles[dwCPU] = hThread;
	}

	return true;
}

bool MIocpNet::CreateListenSocket( const bool bReuse )
{
	// 리슨소켓 생성
	m_sdListen = CreateSocket();
	if (INVALID_SOCKET == m_sdListen) return false;

	if (bReuse) 
	{
		int opt = 1;
		if (setsockopt(m_sdListen, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
			return false;
	}

	// 주소 결합
	SOCKADDR_IN		si_addrlocal;
	si_addrlocal.sin_family		 = AF_INET;
	si_addrlocal.sin_port		 = htons(m_nPort);
	si_addrlocal.sin_addr.s_addr = htonl(INADDR_ANY);       

	int nRet = bind(m_sdListen, (struct sockaddr *)&si_addrlocal, sizeof(si_addrlocal));
	if (SOCKET_ERROR == nRet) 
	{
		mlog("RealCPNet> bind: %d\n", WSAGetLastError());
		return false;
	}

	// 대기모드 진입
	nRet = listen(m_sdListen, 16);
	if (SOCKET_ERROR == nRet) 
	{
		mlog("RealCPNet> listen: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool MIocpNet::CreateAcceptSocket(bool bUpdateIOCP)
{
	int		nRet;
	DWORD	dwRecvNumBytes = 0;     

	//The context for listening socket uses the SockAccept member to store the
	//socket for client connection. 
	if (bUpdateIOCP) 
	{
		m_pListenSession = UpdateCompletionPort(m_sdListen, MIOCP_OP_ACCEPT, false);
		if (m_pListenSession == NULL) 
		{
			mlog("failed to update listen socket to IOCP\n");
			return false;
		}
	}

	SOCKET sdAccept = CreateSocket();
	if (INVALID_SOCKET == sdAccept) 
	{
		mlog("failed to create new accept socket\n");
		delete m_pListenSession;
		m_pListenSession = NULL;
		return false;
	}

	m_pIOCPAccept = new MIOCP_OverlappedAccept();
	m_pIOCPAccept->SetBuffer(m_pListenSession->m_RecvBuffer, MAX_BUFF_SIZE);
	m_pIOCPAccept->SetSocket(sdAccept);

	// pay close attention to these parameters and buffer lengths
	nRet = AcceptEx(m_sdListen, 
					m_pIOCPAccept->GetSocket(),
					(LPVOID)m_pIOCPAccept->GetBuffer(),
					0,
					sizeof(SOCKADDR_IN) + 16,
					sizeof(SOCKADDR_IN) + 16,
					&dwRecvNumBytes,
					(LPOVERLAPPED)m_pIOCPAccept	);

	if (nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		mlog("AcceptEx Failed: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

SOCKET MIocpNet::CreateSocket(void)
{
	int         nRet;
	int         nZero = 0;
	LINGER      lingerStruct;
	SOCKET      sdSocket = INVALID_SOCKET;

	sdSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 
	if (INVALID_SOCKET == sdSocket) 
	{
		mlog("RealCPNet> WSASocket(sdSocket): %d\n", WSAGetLastError());
		return sdSocket;
	}

	BOOL val = TRUE;
	nRet = setsockopt(sdSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR *)&val, sizeof(BOOL) );
	if (SOCKET_ERROR == nRet) 
	{
		mlog("RealCPNet> setsockopt(TCP_NODELAY): %d\n", WSAGetLastError());
		return sdSocket;
	}

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;
	nRet = setsockopt(sdSocket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));
	if (SOCKET_ERROR == nRet) 
	{
		mlog("RealCPNet> setsockopt(SO_LINGER): %d\n", WSAGetLastError());
		return sdSocket;
	}

	return sdSocket;
}

MSession* MIocpNet::UpdateCompletionPort(SOCKET sd, MIOCP_OPERATION nOperation, bool bAddToList)
{
	// 새로운 세션 생성
	MSession* pNewSession = new MSession();
	pNewSession->Initialize(sd);

	m_hIOCP = CreateIoCompletionPort((HANDLE)sd, m_hIOCP, (DWORD_PTR)pNewSession, 0);
	if (NULL == m_hIOCP) 
	{
		mlog("CreateIoCompletionPort: %d\n", GetLastError());

		delete pNewSession;
		return NULL;
	}

	if (bAddToList) m_SessionMap.Add(pNewSession);

	return pNewSession;
}

void MIocpNet::Destroy()
{
	m_bEndServer = true;

	if (m_pListenSession) 
	{
		if (m_pListenSession->GetSocket() != INVALID_SOCKET) 
		{
			closesocket(m_pListenSession->GetSocket());
			m_pListenSession->Finalize();
		}

		if (m_pListenSession)
		{
			delete m_pListenSession;
            m_pListenSession = NULL;
		}
	}

	DeleteAllSession();

	if (m_hIOCP) 
	{
		m_bWorkThread = false;
		for(DWORD i = 0; i < m_dwWorkerThreadCount; i++)
		{
			// WorkerThread에 종료 메시지를 보낸다.
			PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
		}

		CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
	}

	FinalizeSocket();
	CloseHandle(m_hCleanupEvent);

	
}

int g_nCnt = 0;

/////////////////////////////////////
// IOCP Worker Thread ///////////////
void MIocpNet::WorkerThread()
{
	BOOL			bSuccess = FALSE;
	LPOVERLAPPED	lpOverlapped = NULL;
	MSession*		pSession = NULL;
	DWORD			dwIoSize = 0;

	while (m_bWorkThread) 
	{
		bSuccess = GetQueuedCompletionStatus(m_hIOCP,
											&dwIoSize,
											(LPDWORD)&pSession,
											&lpOverlapped,
											INFINITE );
		// 클라이언트가 접속을 끊었을 경우
		if (!bSuccess) 
		{
			if( lpOverlapped == NULL && pSession == NULL )
			{
//				continue;
			}

			//MIOCP_Overlapped* pOverlapped = (MIOCP_Overlapped*)lpOverlapped;
			//LPOVERLAPPED_EX lpOverlappedEx = (LPOVERLAPPED_EX)lpOverlapped;
			//pSession = (MSession*)lpOverlappedEx->s_lpConnection;
			//if( lpConnection == NULL )
			//	continue;
			////Overlapped I/O요청 되어있던 작업의 카운트를 줄인다.
			//if( lpOverlappedEx->s_eOperation == OP_ACCEPT )
			//	lpConnection->DecrementAcceptIoRefCount();
			//else if( lpOverlappedEx->s_eOperation == OP_RECV )
			//	lpConnection->DecrementRecvIoRefCount();
			//else if( lpOverlappedEx->s_eOperation == OP_SEND )
			//	lpConnection->DecrementSendIoRefCount();
			//
			//CloseConnection( lpConnection );
//			continue;
		}

		if (pSession == NULL)
		{
			// CTRL-C handler used PostQueuedCompletionStatus to post an I/O packet with
			// a NULL CompletionKey (or if we get one for any reason).  It is time to exit.
			return;
		}

		if (m_bEndServer) {
			// main thread will do all cleanup needed - see finally block
			return;
		}

		MIOCP_Overlapped* pOverlapped = (MIOCP_Overlapped*)lpOverlapped;
		if( NULL == pOverlapped ) continue;

		//We should never skip the loop and not post another AcceptEx if the current
		//completion packet is for previous AcceptEx
		if (pOverlapped->GetIOOperation() != MIOCP_OP_ACCEPT) 
		{
			if ( !bSuccess || (bSuccess && (0 == dwIoSize)) ) 
			{
				// client connection dropped, continue to service remaining (and possibly 
				// new) client connections
				CloseSession(pSession, FALSE);	// RAON DEAD POINT
				continue;
			}
		}

		switch( pOverlapped->GetIOOperation() )
		{
		case MIOCP_OP_ACCEPT:
			{
				DoAccept( pOverlapped );
			}
			break;
		case MIOCP_OP_RECV:
			{
				DoRecv( pOverlapped , dwIoSize , pSession);
			}
			break;
		case MIOCP_OP_SEND:
			{
				DoSend( pOverlapped , dwIoSize , pSession);
			}
			break;
		default:
			{
				_ASSERT(0);
			}
			break;
		}

		delete pOverlapped;
	}
}

bool MIocpNet::DoAccept( MIOCP_Overlapped* pOverlapped )
{
	MIOCP_OverlappedAccept* pIOCPAccept= (MIOCP_OverlappedAccept*)pOverlapped;

	if (pIOCPAccept->GetSocket() == INVALID_SOCKET) 
	{
#ifdef _DEBUG
		{
			static int nInvalidAccept = 0;
			char szLog[64]; 
			sprintf_s(szLog, "Accept with INVALID_SOCKET (Count=%d) \n", nInvalidAccept++);
			OutputDebugString(szLog);
		}
#endif
		CreateAcceptSocket(FALSE);
		return true;
	}

	// Get Address First //
	int locallen, remotelen;
	sockaddr_in *plocal = 0, *premote = 0;
	sockaddr_in LocalAddr, RemoteAddr;

	GetAcceptExSockaddrs((LPVOID)(pIOCPAccept->GetBuffer()),
									0,
									sizeof(SOCKADDR_IN) + 16,
									sizeof(SOCKADDR_IN) + 16,
									(sockaddr **)&plocal,
									&locallen,
									(sockaddr **)&premote,
									&remotelen);

	memcpy(&LocalAddr, plocal, sizeof(sockaddr_in));
	memcpy(&RemoteAddr, premote, sizeof(sockaddr_in));
	// Get Address End //

	// Check Connection Flood Attack
	// TODO: 아직은 중요한게 아니라 주석처리함 - bird
	//int nIPCount = 0;
	//if (pRealCPNet->CheckIPFloodAttack(&RemoteAddr, &nIPCount) == true) 
	//{
	//	RCPLOG("Accept Detected CONNECTION FLOOD ATTACK (IP=%s , Count=%d) \n", 
	//			inet_ntoa(RemoteAddr.sin_addr), nIPCount);
	//	closesocket(pRCPAccept->GetSocket());
	//	pRealCPNet->CreateAcceptSocket(FALSE);
	//	continue;
	//}

	int nRet = setsockopt(
				pIOCPAccept->GetSocket(), 
				SOL_SOCKET,
				SO_UPDATE_ACCEPT_CONTEXT,
				(char*)&m_sdListen,
				sizeof(m_sdListen) );

	if (nRet == SOCKET_ERROR) 
	{
		//just warn user here.
		mlog("setsockopt(SO_UPDATE_ACCEPT_CONTEXT) failed to update accept socket. (Err:%d) \n", WSAGetLastError());

		closesocket(pIOCPAccept->GetSocket());

		CreateAcceptSocket(FALSE);
		
		return true;
	}

	MSession* lpAcceptSession = UpdateCompletionPort(pIOCPAccept->GetSocket(), MIOCP_OP_ACCEPT, true);
	if (lpAcceptSession == NULL) 
	{
		//just warn user here.
		mlog("failed to update accept socket to IOCP\n");
		SetEvent(m_hCleanupEvent);
		delete pIOCPAccept; pIOCPAccept = NULL;
		
		return false;
	}

	lpAcceptSession->SetSockAddr(&RemoteAddr, remotelen);

	//Time to post another outstanding AcceptEx
	if (!CreateAcceptSocket(FALSE)) 
	{
		mlog("Please shut down and reboot the server.\n");
		SetEvent(m_hCleanupEvent);
		delete pIOCPAccept; pIOCPAccept = NULL;

		return false;
	}               

	if (m_fnCallback)
	{
		m_fnCallback(m_pCallbackContext, MIOCP_OP_ACCEPT, 
								(DWORD)lpAcceptSession->GetSocket(), 
								(MPacketHeader*)lpAcceptSession->m_RecvBuffer, 0);
	}

	PostIORecv(lpAcceptSession->GetSocket());

	return true;
}

void MIocpNet::DoRecv( MIOCP_Overlapped* pOverlapped , DWORD dwIoSize , MSession* pSession )
{
	MIOCP_OverlappedRecv* pRCPRecv = (MIOCP_OverlappedRecv*)pOverlapped;

	if (dwIoSize <= 0)
	{
		CloseSession( pSession, false );
		return;
	}

	SOCKET sdRecv = INVALID_SOCKET;

	m_SessionMap.Lock();
		if (m_SessionMap.IsExistUnsafe(pSession)) 
		{	
			// Ensure exist Session 
			if (pSession->IsValid())
			{
				sdRecv = pSession->GetSocket();

				if (m_fnCallback)
				{
					m_fnCallback(m_pCallbackContext, MIOCP_OP_RECV, 
									(DWORD)pSession->GetSocket(), 
									(MPacketHeader*)pSession->m_RecvBuffer, (DWORD)dwIoSize);
				}
			}
		}
	m_SessionMap.Unlock();

	if (INVALID_SOCKET != sdRecv) PostIORecv(sdRecv);
}

void MIocpNet::DoSend( MIOCP_Overlapped* pOverlapped , DWORD dwIoSize , MSession* pSession )
{
	MIOCP_OverlappedSend* pRCPSend = (MIOCP_OverlappedSend*)pOverlapped;
	pRCPSend->AddTransBytes(dwIoSize);
	
	_ASSERT(pRCPSend->GetTransBytes() == pRCPSend->GetTotalBytes());


	if (m_fnCallback)
	{
		DWORD dwSize = (DWORD)pRCPSend->GetTransBytes();
		m_fnCallback(m_pCallbackContext, MIOCP_OP_SEND, 
								(pSession) ? (DWORD)pSession->GetSocket() : 0, NULL, dwSize);
	}
}

DWORD WINAPI MIocpNet::_WorkerThread(LPVOID WorkContext)
{
	MIocpNet* pIocpNet = (MIocpNet*) WorkContext;
	pIocpNet->WorkerThread();
	return 1;
}

bool MIocpNet::PostIOSend(SOCKET sd, char* pBuf, DWORD nBufLen)
{
	MIOCP_OverlappedSend* pRCPSend = new MIOCP_OverlappedSend();
	pRCPSend->SetData(pBuf, nBufLen);

	WSABUF wsaBuf;
	wsaBuf.buf = pBuf;
	wsaBuf.len = nBufLen;

	DWORD dwSendNumBytes = 0;
	DWORD dwFlags = 0;

	int nRet = WSASend(sd,
					&wsaBuf, 1, &dwSendNumBytes,
					dwFlags,
					pRCPSend, NULL);
	if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		mlog("WSASend: %d\n", WSAGetLastError());
		delete pRCPSend;
	} 
	else 
	{
	}

	return true;
}

void MIocpNet::PostIORecv(SOCKET sd)
{
	int nRet = 0;
	m_SessionMap.Lock();
		MSession* pSession = m_SessionMap.GetSessionUnsafe(sd);
		if (pSession) 
		{
			MIOCP_OverlappedRecv* pRecv = new MIOCP_OverlappedRecv();
			pRecv->SetBuffer(pSession->m_RecvBuffer, MAX_BUFF_SIZE);

			WSABUF	buffRecv;
			buffRecv.buf = pRecv->GetBuffer();
			buffRecv.len = pRecv->GetBufferSize();

			DWORD dwRecvNumBytes = 0;
			DWORD dwFlags = 0;

			nRet = WSARecv(sd, &buffRecv, 1, &dwRecvNumBytes, &dwFlags, pRecv, NULL);
			if (nRet == 0) {	}
		}
	m_SessionMap.Unlock();

	if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		mlog("PostIORecv->WSARecv: %d\n", WSAGetLastError());
	}
}


bool MIocpNet::Send(SOCKET sd, MPacketHeader* pPacket, int nSize)
{
	if (nSize <= 0) return false;

	// Send Queue 구성하려면 여기에..

	m_SessionMap.Lock();
		MSession* pSession = m_SessionMap.GetSessionUnsafe(sd);
		if (pSession) 
		{
			PostIOSend(sd, (char*)pPacket, nSize);
		} 
		else 
		{
			free(pPacket);
		}
	m_SessionMap.Unlock();
	return true;
}


void MIocpNet::CloseSession(MSession* pSession, bool bGraceful)
{
	m_SessionMap.Lock();
		if (m_SessionMap.IsExistUnsafe(pSession)) 
		{
			if (pSession->IsValid())
			{
				SOCKET sd = pSession->GetSocket();

				if (!bGraceful) 
				{
					// force the subsequent closesocket to be abortative.
					LINGER  lingerStruct;
					lingerStruct.l_onoff = 1;
					lingerStruct.l_linger = 0;
					setsockopt(sd, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));
				}
				closesocket(sd);
				pSession->Finalize();

				if (m_fnCallback)
				{
					m_fnCallback(m_pCallbackContext, MIOCP_OP_DISCONNECT, (DWORD)sd, NULL, 0);
				}

				m_SessionMap.RemoveUnsafe(sd);
			}
		}
	m_SessionMap.Unlock();
		
	return;    
}

void MIocpNet::DeleteAllSession()
{
	m_SessionMap.RemoveAll();
}

void MIocpNet::Disconnect(SOCKET sd)
{
	MSession* pSession = m_SessionMap.GetSession(sd);
	if (pSession)
	{
		CloseSession(pSession, false);
	}
}

bool MIocpNet::GetAddress(SOCKET sd, char* pszAddress, int nBufferSize, int* pPort)
{
	bool bResult = false;
	m_SessionMap.Lock();
		MSession* pSession = m_SessionMap.GetSessionUnsafe(sd);
		if (pSession) {
			strcpy_s(pszAddress, nBufferSize, pSession->GetIPString());
			*pPort = pSession->GetPort();
			bResult = true;
		}
	m_SessionMap.Unlock();
	return bResult;
}

void MIocpNet::DeleteAcceptSocket()
{
	if (m_pIOCPAccept)
	{
		delete m_pIOCPAccept; m_pIOCPAccept = NULL;
	}
}
} // namespace minet
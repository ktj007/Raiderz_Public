#include "stdafx.h"
#include "MUDPSocket.h"

#ifdef _DEBUG
	#include "assert.h"
#endif

namespace minet {

#define MAX_RECVBUF_LEN		65535

#define SAFEUDP_MAX_SENDQUEUE_LENGTH		5120
#define SAFEUDP_SAFE_MANAGE_TIME			100		// WSA_INFINITE for debug
#define SAFEUDP_SAFE_RETRANS_TIME			500
#define SAFEUDP_MAX_SAFE_RETRANS_TIME		5000


#define LINKSTATE_LOG
void MTRACE(char* pszLog)
{
	char szBuf[_MAX_DIR];
	sprintf_s(szBuf, "%s", pszLog);
	OutputDebugString(szBuf);
}

////////////////////////////////////////////////////////////////////////////////////////////
// MUDPSocketThread class /////////////////////////////////////////////////////////////////////
void MUDPSocketThread::Run()
{
	while(true) {	// Waiting for SafeUDP Settting...
		DWORD dwVal = WaitForSingleObject(m_KillEvent.GetEvent(), 100);
		if (dwVal == WAIT_OBJECT_0) {
			return;
		} else if (dwVal == WAIT_TIMEOUT) {
			if (m_pSocket)
				break;
		}
	}

	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	WORD wEventIndex = 0;

	bool bSendable = false;
	WSANETWORKEVENTS NetEvent;
	WSAEVENT hFDEvent = WSACreateEvent();
	EventArray[wEventIndex++] = hFDEvent;
	EventArray[wEventIndex++] = m_SendEvent.GetEvent();
	EventArray[wEventIndex++] = m_KillEvent.GetEvent();

	WSAEventSelect(m_pSocket->GetLocalSocket(), hFDEvent, FD_READ|FD_WRITE);

	while(!Terminated()) {
		DWORD dwReturn = WSAWaitForMultipleEvents(wEventIndex, EventArray, FALSE, SAFEUDP_SAFE_MANAGE_TIME, FALSE);
		if (dwReturn == WSA_WAIT_TIMEOUT) {					// Time

		} else if (dwReturn == WSA_WAIT_EVENT_0) {			// Socket Event
			WSAEnumNetworkEvents(m_pSocket->GetLocalSocket(), hFDEvent, &NetEvent);
			if ((NetEvent.lNetworkEvents & FD_READ) == FD_READ) {
				Recv();
			} 
			if ((NetEvent.lNetworkEvents & FD_WRITE) == FD_WRITE) {
				bSendable = true;
			}
		} else if (dwReturn == WSA_WAIT_EVENT_0 + 1) {		// Packet Send Event
			if (bSendable == true)
				FlushSend();
		} else if (dwReturn == WSA_WAIT_EVENT_0 + 2) {		// Kill the Thread
			break;	// Stop Thread
		}
	}

	WSACloseEvent(hFDEvent);

	// Clear Queues
	m_csSendLock.Lock();
	{
		for (SendListItor itor = m_SendList.begin(); itor != m_SendList.end(); ) {
			delete (*itor);
			itor = m_SendList.erase(itor);
		}
	}
	{
		for (SendListItor itor = m_TempSendList.begin(); itor != m_TempSendList.end(); ) {
			delete (*itor);
			itor = m_TempSendList.erase(itor);
		}
	}
	m_csSendLock.Unlock();
	m_KillEvent.Signal(); 
}


bool MUDPSocketThread::MakeSockAddr(char* pszIP, int nPort, sockaddr_in* pSockAddr)
{
	sockaddr_in 	RemoteAddr;
	memset((char*)&RemoteAddr, 0, sizeof(sockaddr_in));

	//	Set Dest IP and Port 
	RemoteAddr.sin_family = AF_INET;
	RemoteAddr.sin_port = htons(nPort);
	DWORD dwAddr = inet_addr(pszIP);
	if (dwAddr != INADDR_NONE) {
		memcpy(&(RemoteAddr.sin_addr), &dwAddr, 4);
	} else {		// 연결할 host name을 입력한 경우
		HOSTENT* pHost = gethostbyname(pszIP);
		if (pHost == NULL) {	// error
			OutputDebugString("<SAFEUDP_ERROR> Can't resolve hostname </SAFEUDP_ERROR>\n");
			return false;
		}
		memcpy((char FAR *)&(RemoteAddr.sin_addr), pHost->h_addr, pHost->h_length);
	}
	memcpy(pSockAddr, &RemoteAddr, sizeof(sockaddr_in));
	return true;
}

bool MUDPSocketThread::PushSend(char* pszIP, int nPort, char* pPacket, DWORD dwPacketSize)
{
	if (m_SendList.size() > SAFEUDP_MAX_SENDQUEUE_LENGTH )
		return false;

	sockaddr_in Addr;
	if (MakeSockAddr(pszIP, nPort, &Addr) == false) return false;

	MUDPSendQueueItem* pSendItem = new MUDPSendQueueItem;
	pSendItem->dwIP = Addr.sin_addr.S_un.S_addr;
	pSendItem->wRawPort = Addr.sin_port;
	pSendItem->pPacket = pPacket;
	pSendItem->dwPacketSize = dwPacketSize;

	m_csSendLock.Lock();
	m_TempSendList.push_back(pSendItem);
	m_csSendLock.Unlock();

	m_SendEvent.Signal();

	return true;
}


bool MUDPSocketThread::PushSend( DWORD dwIP, int nPort, char* pPacket, DWORD dwPacketSize )
{
	if( (SAFEUDP_MAX_SENDQUEUE_LENGTH < m_SendList.size()) ||		
	 	(INADDR_NONE == dwIP) )
	 	return false;

	sockaddr_in Addr;
	memset((char*)&Addr, 0, sizeof(sockaddr_in));

	//	Set Dest IP and Port 
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(nPort);
	memcpy(&(Addr.sin_addr), &dwIP, 4);

	MUDPSendQueueItem* pSendItem = new MUDPSendQueueItem;
	if( 0 != pSendItem )
	{
		pSendItem->dwIP = dwIP;
		pSendItem->wRawPort = Addr.sin_port;
		pSendItem->pPacket = pPacket;
		pSendItem->dwPacketSize = dwPacketSize;

		m_csSendLock.Lock();
		m_TempSendList.push_back( pSendItem );
		m_csSendLock.Unlock();

		m_SendEvent.Signal();

		return true;
	}

	return false;
}



bool MUDPSocketThread::FlushSend()
{
	m_csSendLock.Lock();
	while(m_TempSendList.size() > 0) {
		SendListItor itor = m_TempSendList.begin();
		m_SendList.push_back(*itor);
		m_TempSendList.erase(itor);
	}
	m_csSendLock.Unlock();

	MUDPSendQueueItem* pSendItem;
	sockaddr_in		DestAddr;
	int				nResult;
	while(m_SendList.size() > 0) {
		SendListItor itor = m_SendList.begin();
		pSendItem = *itor;

		DestAddr.sin_family = AF_INET;
		DestAddr.sin_addr.S_un.S_addr = pSendItem->dwIP;
		DestAddr.sin_port = pSendItem->wRawPort;

		nResult = sendto(m_pSocket->GetLocalSocket(), (char*)pSendItem->pPacket, pSendItem->dwPacketSize, 
						 0, (sockaddr*)&DestAddr, sizeof(sockaddr_in));
		if (nResult == SOCKET_ERROR) {
			int nErrCode = WSAGetLastError();
			char szBuf[64]; sprintf_s(szBuf, "<SAFEUDP_ERROR>FlushSend() - sendto() ErrCode=%d \n", nErrCode);
			MTRACE(szBuf);
//			return false;
		} else {
			m_TrafficLog.OnSend(nResult);
		}
		
		delete pSendItem->pPacket;
		delete pSendItem;
		m_SendList.erase(itor);


	}
	return true;
}


bool MUDPSocketThread::Recv()
{
	sockaddr_in		AddrFrom;
	int				nAddrFromLen = sizeof(sockaddr);

	char			RecvBuf[MAX_RECVBUF_LEN];
	int				nRecv = 0;

	while(TRUE) {
		nRecv = recvfrom(m_pSocket->GetLocalSocket(), RecvBuf, MAX_RECVBUF_LEN, 0, 
						 (struct sockaddr*)&AddrFrom, &nAddrFromLen);
		if (nRecv != SOCKET_ERROR) {
			m_TrafficLog.OnRecv(nRecv);
		}

		if (nRecv <= 0) break;					// Stop Receive Loop

		if (m_fnRecvCallback && OnRecv(AddrFrom.sin_addr.S_un.S_addr, AddrFrom.sin_port, RecvBuf, nRecv) == true) {
			continue;
		}
	}

	return true;
}

bool MUDPSocketThread::OnRecv(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize)
{
	if (m_fnRecvCallback)
		return m_fnRecvCallback(m_pCallbackContext, dwIP, wRawPort, pPacket, dwSize);
	return false;
}





////////////////////////////////////////////////////////////////////////////////////////////
// MUDPSocket class ////////////////////////////////////////////////////////////////////////
MUDPSocketThread::MUDPSocketThread() : MThread()
{
	m_pSocket=NULL; 
	m_fnRecvCallback = NULL; 
	m_pCallbackContext = NULL;
}

MUDPSocketThread::~MUDPSocketThread()
{ 
	m_pSocket=NULL; 
}

bool MUDPSocket::Create(bool bBindWinsockDLL, int nPort, bool bReusePort)
{
	m_bBindWinsockDLL = bBindWinsockDLL;

	WSADATA	wsaData;
	if ((bBindWinsockDLL == true) && (WSAStartup(MAKEWORD(2,2), &wsaData) != 0))
		return false;

	if (OpenSocket(nPort, bReusePort) == false) {
		OutputDebugString("<SAFEUDP_ERROR> OpenSocket() FAILED </SAFEUDP_ERROR>\n");
		return false;
	}


	m_SocketThread.SetSocket(this);
	m_SocketThread.Start();
	return true;
}

void MUDPSocket::Destroy()
{
	if (m_SocketThread.GetSocket() == NULL)	return;

	m_SocketThread.Stop();
	CloseSocket();

	if (m_bBindWinsockDLL == true) {
		WSACleanup();
		m_bBindWinsockDLL = false;
	}
}

bool MUDPSocket::OpenSocket(int nPort, bool bReuse)
{
	SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET)
		return false;

	if (bReuse) {
		int opt = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
			return false;
	}

	sockaddr_in LocalAddress;
	LocalAddress.sin_family			= AF_INET;
	LocalAddress.sin_addr.s_addr	= htonl(INADDR_ANY);
	LocalAddress.sin_port			= htons(nPort);

	if (bind(sockfd, (struct sockaddr*)&LocalAddress, sizeof(LocalAddress)) == SOCKET_ERROR) {
		closesocket(sockfd);
		return false;
	}

	m_Socket = sockfd;
	m_LocalAddress = LocalAddress;

	return true;
}

void MUDPSocket::CloseSocket()
{
	shutdown(m_Socket, SD_SEND);
	closesocket(m_Socket);
	m_Socket = 0;
}


bool MUDPSocket::Send(char* pszIP, int nPort, char* pPacket, DWORD dwSize)
{
	return m_SocketThread.PushSend(pszIP, nPort, pPacket, dwSize);
}

bool MUDPSocket::Send(DWORD dwIP, int nPort, char* pPacket, DWORD dwSize )
{
	return m_SocketThread.PushSend( dwIP, nPort, pPacket, dwSize );
}


void MUDPSocket::GetTraffic(int* nSendTraffic, int* nRecvTraffic)
{
	*nSendTraffic = m_SocketThread.m_TrafficLog.GetSendTraffic();
	*nRecvTraffic = m_SocketThread.m_TrafficLog.GetRecvTraffic();
}

} // namespace minet
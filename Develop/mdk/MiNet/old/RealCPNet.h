#ifndef REALCPNET_H
#define REALCPNET_H

//////////////////////////////////////////////////////////////////
// Class:	MRealCPNet class (2001/10/25)
// File:	RealCPNet.cpp
// Author:	Kim young ho (moanus@maiet.net)
//
// Implements Scalable Network Module with I/O Competion Port.
// Code based on MS iocpserver/iocpclient example
////////////////////////////////////////////////////////////////// 

#pragma warning(disable:4786)
#include <list>
#include <algorithm>
using namespace std;

#include <Winsock2.h>
#include <mswsock.h>

#include "MPacket.h"
#include "MSession.h"

// Constants
#define DEFAULT_PORT		5000
#define MAX_WORKER_THREAD	16


typedef list<MPacketHeader*>		MPacketList;
typedef MPacketList::iterator		MPacketListItor;
class MRealCPNet;


typedef enum RCP_IO_OPERATION {
	RCP_IO_NONE,
	RCP_IO_ACCEPT,
	RCP_IO_CONNECT,
	RCP_IO_DISCONNECT,
	RCP_IO_READ,
	RCP_IO_WRITE,
};

class RCPOverlapped : public WSAOVERLAPPED {
protected:
	RCP_IO_OPERATION	m_IOOperation;
public:
	RCPOverlapped(RCP_IO_OPERATION nIO) {	
		ZeroMemory(this, sizeof(RCPOverlapped));	
		m_IOOperation = nIO;
	}
	RCP_IO_OPERATION GetIOOperation()			{ return m_IOOperation; }
};

class RCPOverlappedSend : public RCPOverlapped {
protected:
	int		m_nTotalBytes;
	int		m_nTransBytes;
	char*	m_pData;
public:
	RCPOverlappedSend() : RCPOverlapped(RCP_IO_WRITE) {
		m_nTotalBytes = 0;
		m_nTransBytes = 0;
		m_pData = NULL;
	}
	~RCPOverlappedSend() {
		free(m_pData);	m_pData = NULL;
	}
	void SetData(char* pData, int nDataLen) {
		m_pData = pData;
		m_nTotalBytes = nDataLen;
	}
	int GetTotalBytes()				{ return m_nTotalBytes; }
	int GetTransBytes()				{ return m_nTransBytes; }
	void AddTransBytes(int nBytes)	{ m_nTransBytes += nBytes; }
	char* GetData()					{ return m_pData; }
};

class RCPOverlappedRecv : public RCPOverlapped { 
protected:
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	RCPOverlappedRecv() : RCPOverlapped(RCP_IO_READ) {
		m_pBuffer = 0;
		m_nBufferSize = 0;
	}
	void SetBuffer(char* pBuffer, int nBufferSize) {
		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;
	}
	char* GetBuffer()				{ return m_pBuffer; }
	int GetBufferSize()				{ return m_nBufferSize; }
};

class RCPOverlappedAccept : public RCPOverlapped {
protected:
	SOCKET	m_Socket;
	char*	m_pBuffer;
	int		m_nBufferSize;
public:
	RCPOverlappedAccept() : RCPOverlapped(RCP_IO_ACCEPT) {
		m_Socket = INVALID_SOCKET;
		m_pBuffer = 0;
		m_nBufferSize = 0;
	}
	void SetSocket(SOCKET sd)		{ m_Socket = sd; }
	SOCKET GetSocket()				{ return m_Socket; }
	void SetBuffer(char* pBuffer, int nBufferSize) {
		m_pBuffer = pBuffer;
		m_nBufferSize = nBufferSize;
	}
	char* GetBuffer()				{ return m_pBuffer; }
	int GetBufferSize()				{ return m_nBufferSize; }
};

typedef void(RCPCALLBACK)(void* pCallbackContext, RCP_IO_OPERATION nIO, DWORD dwKey, MPacketHeader* pPacket, DWORD dwPacketLen);

///////////////////////////////////////////////////////////////////////////////////////////////////

class MRealCPNet {
private:
	unsigned short		m_nPort;
	BOOL				m_bEndServer;
	BOOL				m_bRestart;
	BOOL				m_bVerbose;
	HANDLE				m_hIOCP;
	SOCKET				m_sdListen;
	DWORD               m_dwThreadCount;
	HANDLE				m_ThreadHandles[MAX_WORKER_THREAD];
	HANDLE				m_hCleanupEvent;

	MRealSession*		m_pListenSession;
	_old::MSessionMap	m_SessionMap;

	CRITICAL_SECTION	m_csCrashDump;

	RCPCALLBACK*		m_fnCallback;
	void*				m_pCallbackContext;

private:
	DWORD CrashDump(PEXCEPTION_POINTERS ExceptionInfo);

protected:
	SOCKET CreateSocket(void);
	BOOL CreateListenSocket( const bool bReuse );
	BOOL CreateAcceptSocket(BOOL fUpdateIOCP);

	MRealSession* UpdateCompletionPort(SOCKET sd, RCP_IO_OPERATION nOperation, BOOL bAddToList);
	// bAddToList is FALSE for listening socket, and TRUE for connection sockets.
	// As we maintain the context for listening socket in a global structure, we
	// don't need to add it to the list.

	bool PostIOSend(SOCKET sd, char* pBuf, DWORD nBufLen);
	void PostIORecv(SOCKET sd);

	static bool MakeSockAddr(char* pszIP, int nPort, sockaddr_in* pSockAddr);
	bool CheckIPFloodAttack(sockaddr_in* pRemoteAddr, int* poutIPCount);
	void DeleteAllSession();

	static DWORD WINAPI WorkerThread(LPVOID WorkContext);

public:
	MRealCPNet();
	~MRealCPNet();
	bool Create(int nPort, const bool bReuse = false );
	void Destroy();

	void SetLogLevel(int nLevel) { if (nLevel > 0) m_bVerbose = TRUE; else m_bVerbose = FALSE; }
	void SetCallback(RCPCALLBACK* pCallback, void* pCallbackContext) { m_fnCallback = pCallback; m_pCallbackContext = pCallbackContext; }

	bool Connect(SOCKET* pSocket, char* pszAddress, int nPort);
	void Disconnect(SOCKET sd);
	VOID CloseSession(MRealSession* pSession, BOOL bGraceful);

	bool GetAddress(SOCKET sd, char* pszAddress, int* pPort);
	void* GetUserContext(SOCKET sd);
	void SetUserContext(SOCKET sd, void* pContext);

	bool Send(SOCKET sd, MPacketHeader* pPacket, int nSize);	/// PacketÀº malloc free »ç¿ë

friend RCPCALLBACK;
};


typedef void(RCPLOGFUNC)(const char *pFormat,...);
void SetupRCPLog(RCPLOGFUNC* pFunc);

#ifdef REALCPNET_LINK_SOCKET_LIBS
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "mswsock.lib")
#endif


#endif

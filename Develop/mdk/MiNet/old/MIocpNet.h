#ifndef _MIOCP_NET_H
#define _MIOCP_NET_H

#include "MiNetPrerequisites.h"
#include "MPacket.h"
#include "MiNetCommonDef.h"
#include "MSession.h"
#include "MIocpOverlapped.h"

namespace minet {

typedef void(MIOCP_CALLBACK) (void* pCallbackContext, MIOCP_OPERATION nIO, DWORD dwKey, MPacketHeader* pPacket, DWORD dwPacketLen);

class MIocpNet
{
private:
	static DWORD WINAPI _WorkerThread(LPVOID WorkContext);
protected:
	unsigned short		m_nPort;
	HANDLE				m_hIOCP;
	SOCKET				m_sdListen;
	bool				m_bEndServer;

	DWORD				m_dwWorkerThreadCount;
	bool				m_bWorkThread;
	HANDLE				m_ThreadHandles[MAX_IOCP_WORKER_THREAD];
	HANDLE				m_hCleanupEvent;

	MSession*			m_pListenSession;
	MSessionMap			m_SessionMap;

	MIOCP_OverlappedAccept*		m_pIOCPAccept;

	bool				InitializeSocket();
	void				FinalizeSocket();
	
	bool				CreateListenSocket( const bool bReuse );
	bool				CreateWorkerIOCP();
	bool				CreateWorkerThreads();
	MSession*			UpdateCompletionPort(SOCKET sd, MIOCP_OPERATION nOperation, bool bAddToList);
	void				CloseSession(MSession* pSession, bool bGraceful);
	void				DeleteAllSession();


	////////// Worker Thread에서 사용하는 함수 /////////////////////
	MIOCP_CALLBACK*		m_fnCallback;
	void*				m_pCallbackContext;

	bool				DoAccept( MIOCP_Overlapped* pOverlapped );
	void				DoRecv( MIOCP_Overlapped* pOverlapped , DWORD dwIoSize , MSession* pSession );
	void				DoSend( MIOCP_Overlapped* pOverlapped , DWORD dwIoSize , MSession* pSession );
	bool				PostIOSend(SOCKET sd, char* pBuf, DWORD nBufLen);
	void				PostIORecv(SOCKET sd);

	SOCKET				CreateSocket(void);
	bool				CreateAcceptSocket(bool bUpdateIOCP);
	void				DeleteAcceptSocket();
	////////// Worker Thread에서 사용하는 함수 /////////////////////
protected:
	friend class MNetServer;
	////////// MNetServer에서만 사용하는 함수 //////////////////////
	void Disconnect(SOCKET sd);
	bool Send(SOCKET sd, MPacketHeader* pPacket, int nSize);	/// Packet은 malloc free 사용

	void SetCallback(MIOCP_CALLBACK* pCallback, void* pCallbackContext) 
	{ 
		m_fnCallback = pCallback; 
		m_pCallbackContext = pCallbackContext; 
	}
	bool GetAddress(SOCKET sd, char* pszAddress, int nBufferSize, int* pPort);
	////////// MNetServer에서만 사용하는 함수 //////////////////////
public:
	MIocpNet();
	virtual ~MIocpNet();
	bool Create(int nPort, const bool bReuse = false );
	void Destroy();
	void WorkerThread();
};


} // namespace minet



#endif
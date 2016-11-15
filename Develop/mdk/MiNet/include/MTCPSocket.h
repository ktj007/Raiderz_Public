#ifndef	MCLIENT_SOCKET_H
#define MCLIENT_SOCKET_H

#include "MiNetPrerequisites.h"
#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MThread2.h"
#include "MPacket.h"
#include "MiNetCommon.h"

namespace minet {

class MTCPSocket;

enum SOCKET_ERROR_EVENT {eeGeneral, eeSend, eeReceive, eeConnect, eeDisconnect, eeAccept};

// client callback
typedef void (MSocketErrorCallback)	(void* pCallbackContext, SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);
typedef bool (MClientRecvCallback)	(void* pCallbackContext, SOCKET sock, void* pPacket, DWORD dwSize);
typedef bool (MConnectCallback)		(void* pCallbackContext, SOCKET sock);
typedef bool (MDisconnectCallback)	(void* pCallbackContext);

/// 클라이언트 TCP 소켓 쓰레드
class MTCPSocketThread : public cml2::MThread 
{
	friend class MTCPSocket;
public:
	struct MTCPSendQueueNode {
		char*			pPacket;
		DWORD			dwPacketSize;
	};

	typedef list<MTCPSendQueueNode*>			TCPSendList;
protected:
	static const int		MAX_RECVBUF_LEN = 8192;

	bool					m_bActive;

	MTCPSocket*				m_pTCPSocket;
	cml2::MAutoSignalEvent	m_SendEvent;
	cml2::MAutoSignalEvent	m_KillEvent;
	cml2::MCriticalSection	m_csSendLock;

	MSendRecvTrafficLog		m_TrafficLog;
	char					m_RecvBuf[MAX_RECVBUF_LEN];
	TCPSendList				m_SendList;			// Sending priority Low	(Safe|Normal) Packet
	TCPSendList				m_TempSendList;		// Temporary Send List for Sync
	
	void					OnSocketError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int &ErrorCode);
	bool					OnConnect(SOCKET sock);
	bool					OnDisconnect();
	bool					OnRecv(SOCKET socket, char* pPacket, DWORD dwSize);
	bool					FlushSend();
	bool					Recv();
	void					ClearSendList();
	void					LockSend()			{ m_csSendLock.Lock(); }
	void					UnlockSend()		{ m_csSendLock.Unlock(); }
	size_t					GetSendWaitQueueCount()	{ return m_TempSendList.size(); }
	void					Kill();

	void*					m_pCallbackContext;
	MSocketErrorCallback*	m_fnSocketErrorCallback;
	MClientRecvCallback*	m_fnRecvCallback;
	MConnectCallback*		m_fnConnectCallback;
	MDisconnectCallback*	m_fnDisconnectCallback;

	virtual void			Run();

	void RunSocket();

	bool					PushSend(char* pPacket, DWORD dwPacketSize);
	bool					IsActive()			{ return m_bActive; }
public:
							MTCPSocketThread(MTCPSocket* pTCPSocket);
	virtual					~MTCPSocketThread();
};

/// TCP 클라이언트 소켓 클래스
class MINET_API MTCPSocket
{
	friend class MTCPSocketThread;
private:
protected:
	enum _Status
	{
		INITIALIZED = 0,
		OPENED,
		CLOSED
	};
	_Status					m_nStatus;
	string					m_strLastFunc;
	vector<string>			m_FuncStr;
	cml2::MCriticalSection	m_LogLock;

	bool					m_bInitialized;
	char					m_szHost[64];			///< Host IP
	int						m_nPort;				///< Host Port
	SOCKET					m_Socket;				///< My Socket
	cml2::MCriticalSection	m_SocketLock;
	bool					m_bSocketReuse;
	MTCPSocketThread*		m_pSocketThread;		///< socket thread

	bool					OpenSocket();
	void					CloseSocket();

	bool					BindSocket(const char* szNetworkCardIP);
	
public:
							MTCPSocket();		///< 생성자
							MTCPSocket(bool bSocketReuse);		///< 생성자
	virtual					~MTCPSocket();		///< 소멸자

	bool					Initialize();
	void					Finalize();

	bool					Connect(const char* szRemoteIP, int nPort, const char* szMyNetworkCardIP = "");
	bool					Disconnect();
	bool					Send(char *pPacket, DWORD dwPacketSize);

	void					SetCallbackContext(void* pCallbackContext)				{ m_pSocketThread->m_pCallbackContext = pCallbackContext;	}
	void					SetSocketErrorCallback(MSocketErrorCallback pCallback)	{ m_pSocketThread->m_fnSocketErrorCallback = pCallback;		}
	void					SetConnectCallback(MConnectCallback pCallback)			{ m_pSocketThread->m_fnConnectCallback = pCallback;			}
	void					SetDisconnectCallback(MDisconnectCallback pCallback)	{ m_pSocketThread->m_fnDisconnectCallback = pCallback;		}
	void					SetRecvCallback(MClientRecvCallback pCallback)			{ m_pSocketThread->m_fnRecvCallback = pCallback;			}

	SOCKET					GetSocket()		{ return m_Socket; }
	int						GetPort()		{ return m_nPort; }
	bool					IsActive()		{ return m_pSocketThread->IsActive(); }
	const char*				GetHost()		{ return m_szHost; }
	void					GetTraffic(int* nSendTraffic, int* nRecvTraffic);
	uint32					GetTotalSendBytes();
	uint32					GetTotalRecvBytes();

	void Log(const char* szLog);
	void OuputLog()
	{
		for (size_t i = 0; i < m_FuncStr.size(); i++)
		{
			minet_log(m_FuncStr[i].c_str());
			minet_log("\n");
		}
	}
};


} // namespace minet

#endif
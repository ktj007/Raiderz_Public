#ifndef MUDP_SOCKET_H
#define MUDP_SOCKET_H

#include "MiNetPrerequisites.h"
#include "MThread2.h"
#include "MPacket.h"
#include "MiNetCommon.h"

namespace minet {

// INNER CLASS //////////////////////////////////////////////////////////////////////////
typedef bool(MUDPRecvCallback)(void* pCallbackContext, DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);	// Real UDP Packet

class MUDPSocket;

class MUDPSocketThread : public cml2::MThread 
{
	friend class MUDPSocket;
private:
	struct MUDPSendQueueItem {
		DWORD			dwIP;
		WORD			wRawPort;
		char*			pPacket;
		DWORD			dwPacketSize;
	};

	typedef list<MUDPSendQueueItem*>	SendList;
	typedef SendList::iterator			SendListItor;

	MUDPSocket*							m_pSocket;
	cml2::MAutoSignalEvent				m_SendEvent;
	cml2::MAutoSignalEvent				m_KillEvent;

	SendList							m_SendList;			// Sending priority Low	(Safe|Normal) Packet
	SendList							m_TempSendList;		// Temporary Send List for Sync
	cml2::MCriticalSection				m_csSendLock;

	MSendRecvTrafficLog					m_TrafficLog;
	void*								m_pCallbackContext;
	MUDPRecvCallback*					m_fnRecvCallback;

	bool								FlushSend();

	bool								Recv();
	bool								OnRecv(DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize);
	bool								MakeSockAddr(char* pszIP, int nPort, sockaddr_in* pSockAddr);

	void								SetSocket(MUDPSocket* pSocket)		{ m_pSocket = pSocket; }
	MUDPSocket*							GetSocket()							{ return m_pSocket; }
public:
										MUDPSocketThread();
	virtual								~MUDPSocketThread();
	bool								PushSend(char* pszIP, int nPort, char* pPacket, DWORD dwPacketSize);
	bool								PushSend( DWORD dwIP, int nPort, char* pPacket, DWORD dwPacketSize );
	virtual void						Run();
};

// OUTER CLASS //////////////////////////////////////////////////////////////////////////
class MUDPSocket {
private:
	bool						m_bBindWinsockDLL;		// Socket DLL Load
	SOCKET						m_Socket;				// My Socket
	sockaddr_in					m_LocalAddress;			// My IP and Port
	MUDPSocketThread			m_SocketThread;

	bool						OpenSocket(int nPort, bool bReuse);
	void						CloseSocket();
public:
	bool						Create(bool bBindWinsockDLL, int nPort, bool bReuse=true);
	void						Destroy();
	void						SetCallbackContext(void* pCallbackContext)	{ m_SocketThread.m_pCallbackContext = pCallbackContext;	}
	void						SetRecvCallback(MUDPRecvCallback pCallback) { m_SocketThread.m_fnRecvCallback = pCallback; }
	bool						Send(char* pszIP, int nPort, char* pPacket, DWORD dwSize);
	bool						Send(DWORD dwIP, int nPort, char* pPacket, DWORD dwSize );
	
	SOCKET						GetLocalSocket()		{ return m_Socket; }
	char*						GetLocalIPString()		{ return inet_ntoa(m_LocalAddress.sin_addr); }
	DWORD						GetLocalIP()			{ return m_LocalAddress.sin_addr.S_un.S_addr; }
	WORD						GetLocalPort()			{ return m_LocalAddress.sin_port; }
	void						GetTraffic(int* nSendTraffic, int* nRecvTraffic);
};

} // namespace minet

#endif

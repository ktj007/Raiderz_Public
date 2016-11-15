#ifndef _MREAL_SESSION_H
#define _MREAL_SESSION_H

#include <list>
#include <algorithm>
#include <map>
using namespace std;

#include <Winsock2.h>
#include <mswsock.h>
#include "MSync.h"

// Constants
#define MAX_BUFF_SIZE		8192

class MRealCPNet;

// For AcceptEx, the IOCP key is the MRealSession for the listening socket,
// so we need to another field SocketAccept in PER_IO_CONTEXT. When the outstanding
// AcceptEx completes, this field is our connection socket handle.
class MRealSession 
{
public:
	enum SESSIONSTATE { SESSIONSTATE_IDLE, SESSIONSTATE_ACTIVE, SESSIONSTATE_DEAD };

private:
	SOCKET				m_sdSocket;
	SOCKADDR_IN			m_SockAddr;
	SESSIONSTATE		m_nSessionState;
	void*				m_pUserContext;
public:
	char				m_RecvBuffer[MAX_BUFF_SIZE];	// RCPOverlappedRecv 용도

						MRealSession();
	virtual				~MRealSession()		{ }

	void				SetSocket(SOCKET sd)	{ m_sdSocket = sd; }
	SOCKET				GetSocket()			{ return m_sdSocket; }

	void				SetSockAddr(SOCKADDR_IN* pAddr, int nAddrLen);
	SOCKADDR_IN*		GetSockAddr()			{ return &m_SockAddr; }
	char*				GetIPString()			{ return inet_ntoa(m_SockAddr.sin_addr); }
	DWORD				GetIP()					{ return m_SockAddr.sin_addr.S_un.S_addr; }
	int					GetPort()				{ return ntohs(m_SockAddr.sin_port); }
	WORD				GetRawPort()			{ return m_SockAddr.sin_port; }

	void				SetSessionState(SESSIONSTATE nState)	{ m_nSessionState = nState; }
	SESSIONSTATE		GetSessionState()						{ return m_nSessionState; }
	void				SetUserContext(void* pContext)			{ m_pUserContext = pContext; }
	void*				GetUserContext()						{ return m_pUserContext; }
};

namespace _old {

class MSessionMap : protected map<SOCKET, MRealSession*> 
{
protected:
	MCriticalSection		m_csLock;
public:
							MSessionMap()	{  }
	virtual					~MSessionMap()	{  }

	// Safe Methods ////////////////////////////////////////////////////////
	void					Add(MRealSession* pSession);
	bool					Remove(SOCKET sd, MSessionMap::iterator* pNextItor=NULL);
	void					RemoveAll();
	bool					IsExist(SOCKET sd);
	MRealSession*			GetSession(SOCKET sd);

	// Unsafe Methods /////////////////////////////////////////////////////////////////////
	// ~Unsafe() 용도 이외엔 사용금지
	void					Lock()		{ m_csLock.Lock(); }
	void					Unlock()	{ m_csLock.Unlock(); }

	MSessionMap::iterator	GetBeginItorUnsafe()	{ return begin(); }
	MSessionMap::iterator	GetEndItorUnsafe()	{ return end(); }
	MRealSession*			GetSessionUnsafe(SOCKET sd);		// 반드시 Lock과 Unlock을 동반해서 사용
	bool					IsExistUnsafe(MRealSession* pSession);
	bool					RemoveUnsafe(SOCKET sd, MSessionMap::iterator* pNextItor=NULL);

friend MRealCPNet;
};

}



#endif
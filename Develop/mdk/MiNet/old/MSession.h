#ifndef _MINET_SESSION_H
#define _MINET_SESSION_H

#include "MThread2.h"
#include <map>
using namespace std;

namespace minet {

#define MAX_BUFF_SIZE		8192

class MIocpNet;

class MSession
{
private:
	SOCKET				m_sdSocket;
	SOCKADDR_IN			m_SockAddr;
public:
	char				m_RecvBuffer[MAX_BUFF_SIZE];	// RCPOverlappedRecv 용도

						MSession();
	virtual				~MSession()		{ }
	void				Initialize(SOCKET sdSocket);
	void				Finalize();

	void				SetSockAddr(SOCKADDR_IN* pAddr, int nAddrLen);
	SOCKET				GetSocket()						{ return m_sdSocket; }
	SOCKADDR_IN*		GetSockAddr()					{ return &m_SockAddr; }
	char*				GetIPString()					{ return inet_ntoa(m_SockAddr.sin_addr); }
	DWORD				GetIP()							{ return m_SockAddr.sin_addr.S_un.S_addr; }
	int					GetPort()						{ return ntohs(m_SockAddr.sin_port); }
	WORD				GetRawPort()					{ return m_SockAddr.sin_port; }
	bool				IsValid()						{ return (m_sdSocket != INVALID_SOCKET); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class MSessionMap
{
protected:
	map<SOCKET, MSession*>	m_SessionMap;
	cml2::MCriticalSection		m_csLock;
public:
							MSessionMap();
							~MSessionMap();

	// Safe Methods ////////////////////////////////////////////////////////
	void					Add(MSession* pSession);
	void					RemoveAll();
	MSession*				GetSession(SOCKET sd);

	// Unsafe Methods /////////////////////////////////////////////////////////////////////
	// ~Unsafe() 용도 이외엔 사용금지
	void					Lock()		{ m_csLock.Lock(); }
	void					Unlock()	{ m_csLock.Unlock(); }

	MSession*				GetSessionUnsafe(SOCKET sd);		// 반드시 Lock과 Unlock을 동반해서 사용
	bool					IsExistUnsafe(MSession* pSession);
	bool					RemoveUnsafe(SOCKET sd);

friend MIocpNet;
};


} // namespace minet


#endif
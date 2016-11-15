#pragma once

#include "MiNetPrerequisites.h"
#include "MActor.h"
#include "MThread2.h"

namespace minet {

class MTcpListenSocket;
class MLinkMap;
class MServerTcpSocketPool;

class MAcceptListener
{
public:
	virtual void OnAccept(MServerTcpSocket& tcp_socket) {}
};


class MAcceptor : public MActor
{
public:
	MTcpListenSocket*		m_pTcpListenSocket;
	MAcceptListener*		m_pListener;
	MServerTcpSocketPool*	m_pSocketPool;

	map<int, MAct*>			m_Gabages;
	cml2::MCriticalSection	m_csGabageLock;
	void AddGabage(MAct* pAct);
	void DelGabage(MAct* pAct);
	void ClearGabages();
public:
	MAcceptor();
	~MAcceptor();

	virtual void ProcEvent(MAct* act, DWORD bytes_transferred);
	virtual void ProcError(MAct* act, DWORD bytes_transferred, DWORD error);
	void Init(MTcpListenSocket* pTcpListenSocket, MProactor* pProactor, MServerTcpSocketPool* pTcpSocketPool);
	bool Register(MServerTcpSocket* pAcceptSocket);
	void SetListener(MAcceptListener* pListner) { m_pListener = pListner; }
protected:
	void OnAccept(MServerTcpSocket& tcp_socket);
};


} // namespace minet {
#pragma once

#include "MActor.h"

namespace minet {

class MServerTcpSocketPool;

class MDisconnectListener
{
public:
	virtual void OnDisconnect(MUID uidLink) = 0;
};


class MDisconnector : public MActor
{
private:
	MDisconnectListener*	m_pListener;
	MServerTcpSocketPool*	m_pSocketPool;

	void OnDisconnect(MUID uidLink);
public:
	MDisconnector() : MActor(), m_pListener(NULL), m_pSocketPool(NULL) {}
	void Init( MProactor* proactor, MServerTcpSocketPool* pSocketPool );
	virtual void ProcEvent( MAct* act, DWORD bytes_transferred );
	virtual void ProcError( MAct* act, DWORD bytes_transferred, DWORD error );
	void SetListener(MDisconnectListener* pListener) { m_pListener = pListener; }
	void HardDisconnect(MServerTcpSocket* pTcpSocket, MUID uidLink);
};




} // namespace minet {
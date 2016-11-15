#pragma once

#include "MActor.h"

namespace minet {

class MRecvListener
{
public:
	virtual void OnDisconnect(MUID uidLink) = 0;
	virtual void OnHardDisconnect(MUID uidLink) = 0;
	virtual void OnRecv(MServerTcpSocket& tcpsocket, MUID uidLink, BYTE* pData, DWORD dwPacketLen) = 0;
};

class MReceiver : public MActor
{
protected:
	MRecvListener*	m_pListener;
	void OnRecv(MServerTcpSocket& tcpsocket, MUID uidLink, BYTE* pData, DWORD dwPacketLen);
public:
	MReceiver();
	virtual void ProcEvent( MAct* act, DWORD bytes_transferred );
	virtual void ProcError( MAct* act, DWORD bytes_transferred, DWORD error );
	void Init( MProactor* proactor );
	void SetListener(MRecvListener* pListener) { m_pListener = pListener; }
};


} // namespace minet {
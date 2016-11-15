#pragma once

#include "MActor.h"

namespace minet {

class MConnectListener
{
public:
	virtual void OnConnect(MServerTcpSocket& tcp_socket) = 0;
};

class MServerTcpSocketPool;

class MConnector  : public MActor {
public:
	MConnector(MProactor* const proactor,MServerTcpSocketPool* const socketPool);
	virtual ~MConnector();

	MServerTcpSocket* const Connect(const char* const addr,const char* const port);
	void SetListener(MConnectListener* const pListner);

	void ProcEvent(MAct* act, DWORD bytes_transferred) override;
	void ProcError(MAct* act, DWORD bytes_transferred, DWORD erorr) override;

private:
	MConnectListener*		m_pListener;
	MServerTcpSocketPool*	m_pSocketPool;
};


} // namespace minet {
#pragma once

#include "MConnector.h"

class XDummyMaster;

class XDummyConnectListener : public MConnectListener
{
public:
	XDummyConnectListener(XDummyMaster& dummyMaster);
	virtual ~XDummyConnectListener();

	void OnConnect(MServerTcpSocket& tcp_socket) override;

private:
	XDummyMaster&			m_dummyMaster;
	MUIDGenerator			m_uidGenerator;
	cml2::MCriticalSection	m_cs;
};


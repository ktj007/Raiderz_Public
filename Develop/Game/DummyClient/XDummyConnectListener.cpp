#include "stdafx.h"
#include "XDummyConnectListener.h"
#include "XDummyMaster.h"
#include "MServerTcpSocket.h"

XDummyConnectListener::XDummyConnectListener(XDummyMaster& dummyMaster)
: m_dummyMaster(dummyMaster)
{
}

XDummyConnectListener::~XDummyConnectListener()
{
}

void XDummyConnectListener::OnConnect(MServerTcpSocket& tcp_socket)
{
	{
		cml2::MCriticalSection::Guard guard(m_cs);
		tcp_socket.SetLink(m_uidGenerator.Generate());
	}
	
	m_dummyMaster.OnConnect(tcp_socket);
}

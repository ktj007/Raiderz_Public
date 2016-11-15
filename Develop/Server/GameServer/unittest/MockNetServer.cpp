#include "stdafx.h"
#include "MockNetServer.h"
#include "MockLink.h"
#include "GCmdHandlerGroup.h"

MMockLink* MockNetServer::NewMockLink( MUID uidLink )
{
	minet::MServerTcpSocket tcp_socket(NULL, NULL, NULL, NULL);
	MockLink* pNewLink = new MockLink(uidLink, this, tcp_socket, GetCommandManager());
	return pNewLink;
}

MockNetServer::MockNetServer( const MNetServerDesc& desc ) : MMockNetServer(desc)
{
	m_pCmdHandlerGroup = new GCmdHandlerGroup(this);
}

MockNetServer::~MockNetServer()
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}
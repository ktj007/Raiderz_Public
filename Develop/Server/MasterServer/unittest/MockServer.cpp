#include "stdafx.h"
#include "MockServer.h"
#include "MockNetServer.h"
#include "ZCommandCenter.h"
#include "ZGameServerObjectManager.h"

MockServer::MockServer()
{

}

MockServer::~MockServer()
{
}

bool MockServer::InitInfo()
{
	InitDB();


	return true;
}

bool MockServer::CreateNetwork()
{
	if (m_pCommandCenter) delete m_pCommandCenter;
	if (m_pNetServer) delete m_pNetServer;

	minet::MNetServerDesc server_desc;
	server_desc.bHeartBeat = false;

	m_pNetServer = new MockNetServer(server_desc);
	m_pCommandCenter = new ZCommandCenter(m_pNetServer);
	gsys.pCommandCenter = m_pCommandCenter;

	return true;
}

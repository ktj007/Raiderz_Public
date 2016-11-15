#include "stdafx.h"
#include "MockServer.h"
#include "MockNetServer.h"
#include "LCommandCenter.h"
#include "LCommObjectManager.h"

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
	m_pCommandCenter = new LCommandCenter(m_pNetServer, gmgr.pCommObjectManager);
	gsys.pCommandCenter = m_pCommandCenter;

	return true;
}


#include "stdafx.h"
#include "MockServer.h"
#include "MockNetServer.h"
#include "LCommandCenter.h"
#include "LPlayerObjectManager.h"
#include "MockMasterNetClient.h"
#include "LMasterClient.h"

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
	if (m_pMasterClient) delete m_pMasterClient;

	minet::MNetServerDesc server_desc;
	server_desc.bHeartBeat = false;

	m_pNetServer = new MockNetServer(server_desc);
	m_pCommandCenter = new LCommandCenter(m_pNetServer);

	minet::MNetClientDesc master_net_client_desc;
	master_net_client_desc.bKeepAlive = false;
	MockMasterNetClient* pNetClient = new MockMasterNetClient(master_net_client_desc);

	m_pMasterClient = new LMasterClient();
	if (m_pMasterClient->Create(pNetClient) == false)
	{
		mlog("Failed To MasterClient Creation\n");
		return false;
	}


	gsys.pCommandCenter = m_pCommandCenter;

	return true;
}


#include "stdafx.h"
#include "MockServer.h"
#include "MockNetServer.h"
#include "PCommandCenter.h"
#include "PGameServerObjectManager.h"
#include "PConfig.h"
#include "MNetClient.h"
#include "PMasterServerFacade.h"
#include "MockMasterNetClient.h"
#include "SProxyPlayerManager.h"

MockServer::MockServer() 
{
	SAFE_DELETE(m_pProxyPlayerManager);
	SAFE_DELETE(m_pMasterServerFacade);

	minet::MNetClientDesc master_net_client_desc;
	master_net_client_desc.bKeepAlive = false;

	MockMasterNetClient* pMasterMockNetClient = new MockMasterNetClient(master_net_client_desc);
	m_pMasterServerFacade = new PMasterServerFacade(pMasterMockNetClient);
	m_pProxyPlayerManager = new SProxyPlayerManager(pMasterMockNetClient);

	gsys.pMasterServerFacade = m_pMasterServerFacade;
}

MockServer::~MockServer()
{

}

bool MockServer::Create()
{
	CreateNetwork();

	m_bCreated = true;

	return true;
}

void MockServer::Destroy()
{
	DestroyNetwork();

	m_bCreated = false;
}

bool MockServer::InitInfo()
{
	return true;
}

bool MockServer::CreateNetwork()
{
	if (m_pCommandCenter) delete m_pCommandCenter;
	if (m_pNetServer) delete m_pNetServer;

	minet::MNetServerDesc server_desc;
	server_desc.bHeartBeat = false;

	m_pNetServer = new MockNetServer(server_desc);
	m_pCommandCenter = new PCommandCenter(m_pNetServer);
	gsys.pCommandCenter = m_pCommandCenter;

	if (m_pMasterServerFacade->CreateNetwork(PConfig::m_strMasterServerIP.c_str(), PConfig::m_nMasterServerPort) == false)
	{
		return false;
	}

	m_pProxyPlayerManager->AddListener(&m_proxyPlayerListener);

	return true;
}

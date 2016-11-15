#include "stdafx.h"
#include "FBaseAppServerFacade.h"
#include "MockAppNetClient.h"
#include "GConfig.h"

FBaseAppServerFacade::FBaseAppServerFacade()
{
	GConfig::m_nMyServerID = 1;

	minet::MNetClientDesc app_net_client_desc;
	app_net_client_desc.bKeepAlive = false;

	MockAppNetClient* pAppMockNetClient = new MockAppNetClient(app_net_client_desc);

	gsys.pAppServerFacade->SetNetClient(pAppMockNetClient);

	m_pAppServerFacade = gsys.pAppServerFacade;	
}

FBaseAppServerFacade::~FBaseAppServerFacade()
{
	// do nothing
}

#include "stdafx.h"
#include "FBaseAppNetClient.h"
#include "GGlobal.h"
#include "GAppServerFacade.h"

FBaseAppNetClient::FBaseAppNetClient() : FBaseAppServerFacade(), m_pAppNetClient(NULL), m_pAppNetAgent(NULL)
{
	if (gsys.pAppServerFacade)
	{
		m_pAppNetClient = gsys.pAppServerFacade->GetNetClient();
		m_pAppNetAgent = new minet::MTestNetAgent(m_pAppNetClient);
	}
}

FBaseAppNetClient::~FBaseAppNetClient()
{
	SAFE_DELETE(m_pAppNetAgent);
}

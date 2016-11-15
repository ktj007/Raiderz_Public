#include "stdafx.h"
#include "FBaseNetClient.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"

FBaseNetClient::FBaseNetClient() : FBaseMasterServerFacade(), m_pNetClient(NULL), m_pNetAgent(NULL)
{
	if (gsys.pMasterServerFacade)
	{
		m_pNetClient = gsys.pMasterServerFacade->GetNetClient();
		m_pNetAgent = new minet::MTestNetAgent(m_pNetClient);
	}
}

FBaseNetClient::~FBaseNetClient()
{
	SAFE_DELETE(m_pNetAgent);
}
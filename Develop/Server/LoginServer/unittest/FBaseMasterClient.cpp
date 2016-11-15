#include "StdAfx.h"
#include "FBaseMasterClient.h"
#include "LGlobal.h"
#include "LServer.h"
#include "LMasterClient.h"

FBaseMasterClient::FBaseMasterClient() : m_pNetAgent(NULL)
{
	minet::MNetClient* pNetClient = gsys.pServer->GetMasterClient()->GetNetClient();
	m_pNetAgent = new minet::MTestNetAgent(pNetClient);
}

FBaseMasterClient::~FBaseMasterClient()
{
	SAFE_DELETE(m_pNetAgent);
}

#pragma once

#include "MMockNetClient.h"
#include "FBaseMasterServerFacade.h"

struct FBaseNetClient : public FBaseMasterServerFacade
{
	FBaseNetClient();
	virtual ~FBaseNetClient();

	minet::MNetClient*		m_pNetClient;
	minet::MTestNetAgent*	m_pNetAgent;
};

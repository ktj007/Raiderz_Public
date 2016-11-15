#pragma once

#include "MMockNetClient.h"
#include "FBaseAppServerFacade.h"

struct FBaseAppNetClient : public FBaseAppServerFacade
{
	FBaseAppNetClient ();
	virtual ~FBaseAppNetClient ();

	minet::MNetClient*		m_pAppNetClient;
	minet::MTestNetAgent*	m_pAppNetAgent;
};

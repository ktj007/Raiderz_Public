#pragma once

#include "MockNetClient.h"

struct FBaseNetClient
{
	FBaseNetClient();
	virtual ~FBaseNetClient();

	minet::MNetClient*		m_pNetClient;
	minet::MTestNetAgent*	m_pNetAgent;
};
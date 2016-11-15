#pragma once

#include "MMockNetClient.h"

class FBaseMasterClient
{
public:
	FBaseMasterClient();
	virtual ~FBaseMasterClient();

	minet::MTestNetAgent*	m_pNetAgent;
};

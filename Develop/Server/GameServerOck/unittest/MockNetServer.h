#pragma once

#include "MMockNetServer.h"
#include "GNetServer.h"

class MockLink;

class MockNetServer : public minet::MMockNetServer
{
protected:
	GCmdHandlerGroup*	m_pCmdHandlerGroup;
	virtual MMockLink* NewMockLink(MUID uidLink);
public:
	MockNetServer(const MNetServerDesc& desc);
	virtual ~MockNetServer();
};

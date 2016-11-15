#pragma once

#include "MMockNetServer.h"
#include "PNetServer.h"

class PCmdHandlerGroup;

class MockNetServer : public minet::MMockNetServer
{
protected:
	PCmdHandlerGroup*	m_pCmdHandlerGroup;
	virtual MMockLink* NewMockLink(MUID uidLink);
public:
	MockNetServer(const MNetServerDesc& desc);
	virtual ~MockNetServer();
};

#pragma once

#include "MMockNetServer.h"
#include "LNetServer.h"

class LCmdHandlerGroup;

class MockNetServer : public minet::MMockNetServer
{
protected:
	LCmdHandlerGroup*	m_pCmdHandlerGroup;
	virtual MMockLink* NewMockLink(MUID uidLink);
public:
	MockNetServer(const MNetServerDesc& desc);
	virtual ~MockNetServer();
};

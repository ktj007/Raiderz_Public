#pragma once

#include "MMockNetServer.h"
#include "ZNetServer.h"

class ZCmdHandlerGroup;

class MockNetServer : public minet::MMockNetServer
{
protected:
	ZCmdHandlerGroup*	m_pCmdHandlerGroup;
	virtual MMockLink* NewMockLink(MUID uidLink);
public:
	MockNetServer(const MNetServerDesc& desc);
	virtual ~MockNetServer();
};

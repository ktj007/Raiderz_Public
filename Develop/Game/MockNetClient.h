#pragma once

#include "XNetClient.h"
#include "MMockNetClient.h"

class XCmdHandlerGroup;

class MockNetClient : public minet::MMockNetClient
{
protected:
	XCmdHandlerGroup*			m_pCmdHandlerGroup;
public:
	MockNetClient(const MNetClientDesc& desc);
	virtual ~MockNetClient();
};


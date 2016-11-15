#include "stdafx.h"
#include "MockNetClient.h"
#include "XCmdHandlerGroup.h"


MockNetClient::MockNetClient( const MNetClientDesc& desc ) : minet::MMockNetClient(desc)
{
	this->m_pCmdHandlerGroup = new XCmdHandlerGroup(this);
}

MockNetClient::~MockNetClient()
{
	SAFE_DELETE(m_pCmdHandlerGroup);
}
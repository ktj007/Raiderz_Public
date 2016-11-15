#pragma once

#include "MMockNetClient.h"

class GCmdHandlerGroup_AppNetClient;

class MockAppNetClient: public MMockNetClient
{
protected:
	GCmdHandlerGroup_AppNetClient*	m_pCmdHandlerGroup;
public:
	MockAppNetClient(const MNetClientDesc& desc);
	virtual ~MockAppNetClient();
};

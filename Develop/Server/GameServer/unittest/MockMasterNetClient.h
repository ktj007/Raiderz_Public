#pragma once

#include "MMockNetClient.h"
#include "GMasterNetClient.h"

class GCmdHandlerGroup_MasterNetClient;

class MockMasterNetClient : public MMockNetClient
{
protected:
	GCmdHandlerGroup_MasterNetClient*	m_pCmdHandlerGroup;
public:
	MockMasterNetClient(const MNetClientDesc& desc);
	virtual ~MockMasterNetClient();
};

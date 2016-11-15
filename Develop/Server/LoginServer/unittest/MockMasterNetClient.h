#pragma once

#include "MMockNetClient.h"
#include "LMasterNetClient.h"

class LCmdHandler_Master_Global;
class LCmdHandler_Master_Net;

class LCmdHandlerGroup_MasterNetClient;

class MockMasterNetClient : public MMockNetClient
{
protected:
	LCmdHandlerGroup_MasterNetClient*	m_pCmdHandlerGroup;
public:
	MockMasterNetClient(const MNetClientDesc& desc);
	~MockMasterNetClient(void);
};

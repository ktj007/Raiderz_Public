#pragma once

#include "MMockNetClient.h"

class PCmdHandler_Master_Global;

class MockMasterNetClient : public MMockNetClient
{
protected:
	PCmdHandler_Master_Global*		m_pGlobalCmdHandler;

public:
	MockMasterNetClient(const MNetClientDesc& desc);
	virtual ~MockMasterNetClient();
};

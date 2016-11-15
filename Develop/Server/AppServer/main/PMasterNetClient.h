#pragma once

#include "SNetClient.h"

class PCmdHandler_Master_Global;

class PMasterNetClient : public SNetClient
{
private:
protected:
	PCmdHandler_Master_Global*		m_pGlobalCmdHandler;

	virtual void			OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);

public:
	PMasterNetClient(const MNetClientDesc& desc);
	virtual ~PMasterNetClient();
};

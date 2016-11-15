#pragma once

#include "GNetClient.h"

class GCmdHandlerGroup_MasterNetClient;

class GMasterNetClient : public GNetClient, public MTestMemPool<GMasterNetClient>
{
private:
protected:
	GCmdHandlerGroup_MasterNetClient*	m_pCmdHandlerGroup;

	virtual void			OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);
public:
	GMasterNetClient(const MNetClientDesc& desc);
	virtual ~GMasterNetClient();
};
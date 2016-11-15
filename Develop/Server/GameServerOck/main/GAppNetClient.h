#pragma once

#include "GNetClient.h"

class GCmdHandlerGroup_AppNetClient;

class GAppNetClient : public GNetClient, public MTestMemPool<GAppNetClient>
{
private:
protected:
	GCmdHandlerGroup_AppNetClient*	m_pCmdHandlerGroup;

	virtual void			OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);
public:
	GAppNetClient(const MNetClientDesc& desc);
	virtual ~GAppNetClient();
};

#pragma once

#include "CSDef.h"
#include "SNetClient.h"
#include "STransData.h"

class LCmdHandler_Master_Global;
class LCmdHandler_Master_Net;
class LCmdHandler_Master_MoveServer;

class LCmdHandlerGroup_MasterNetClient;

class LMasterNetClient : public SNetClient
{
private:
protected:
	LCmdHandlerGroup_MasterNetClient*	m_pHandlerGroup;

	virtual void			OnReplyConnect(MUID& uidHost, MUID& uidAlloc, unsigned int nTimeStamp);
public:
	LMasterNetClient(const MNetClientDesc& desc);
	virtual ~LMasterNetClient();

};



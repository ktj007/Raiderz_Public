#pragma once

#include "MCommandCommunicator.h"
#include "MCommandHandler.h"

class LCmdHandler_Master;

class LCmdHandlerGroup
{
protected:
	LCmdHandler_Master*			m_pMasterCmdHandler;
public:
	LCmdHandlerGroup(minet::MCommandCommunicator* pCC);
	~LCmdHandlerGroup();
};

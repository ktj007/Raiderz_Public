#include "stdafx.h"
#include "LCmdHandlerGroup.h"
#include "LCmdHandler_Master.h"

LCmdHandlerGroup::LCmdHandlerGroup( minet::MCommandCommunicator* pCC )
{
	m_pMasterCmdHandler = new LCmdHandler_Master(pCC);
}

LCmdHandlerGroup::~LCmdHandlerGroup()
{
	SAFE_DELETE(m_pMasterCmdHandler);
}
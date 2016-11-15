#pragma once

#include "MCommandCommunicator.h"
#include "MCommandHandler.h"

class ZCmdHandler_Comm;
class ZCmdHandler_Field;
class ZCmdHandler_Loot;
class ZCmdHandler_Char;
class ZCmdHandler_Party;
class ZCmdHandler_Guild;
class ZCmdHandler_MoveServer;

class ZCmdHandlerGroup
{
protected:
	ZCmdHandler_Comm*			m_pMasterCmdHandler;
	ZCmdHandler_Field*			m_pFieldCmdHandler;
	ZCmdHandler_Loot*			m_pLootCmdHandler;
	ZCmdHandler_Char*			m_pCharCmdHandler;
	ZCmdHandler_Party*			m_pPartyCmdHandler;
	ZCmdHandler_Guild*			m_pGuildCmdHandler;
	ZCmdHandler_MoveServer*		m_pMoveServerCmdHandler;

public:
	ZCmdHandlerGroup(minet::MCommandCommunicator* pCC);
	~ZCmdHandlerGroup();
};

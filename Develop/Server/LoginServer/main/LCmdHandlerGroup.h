#pragma once

#include "MCommandCommunicator.h"
#include "MCommandHandler.h"

class LCmdHandler_NotServable;
class LCmdHandler_Login;
class LCmdHandler_MoveServer;

class LCmdHandlerGroup
{
private:
	minet::MCommandCommunicator* m_pCC;

	LCmdHandler_NotServable*	m_pCmdHandler_NotStartedServer;
	LCmdHandler_Login*			m_pCmdHandler_Login;
	LCmdHandler_MoveServer*		m_pCmdHandler_MoveServer;

public:
	LCmdHandlerGroup(minet::MCommandCommunicator* pCC);
	~LCmdHandlerGroup();

	void ChangeCmdHandler_ServerStart();

};

//////////////////////////////////////////////////////////////////////////

class LCmdHandler_Master_Global;
class LCmdHandler_Master_Net;
class LCmdHandler_Master_MoveServer;

class LCmdHandlerGroup_MasterNetClient
{
private:
	LCmdHandler_Master_Global*		m_pGlobalCmdHandler;
	LCmdHandler_Master_Net*			m_pNetCmdHandler;
	LCmdHandler_Master_MoveServer*	m_pMoveServerCmdHandler;

public:
	LCmdHandlerGroup_MasterNetClient(minet::MCommandCommunicator* pCC);
	~LCmdHandlerGroup_MasterNetClient(void);
};

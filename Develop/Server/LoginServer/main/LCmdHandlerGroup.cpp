#include "stdafx.h"
#include "LCmdHandlerGroup.h"
#include "LCmdHandler_NotServable.h"
#include "LCmdHandler_Login.h"
#include "LCmdHandler_MoveServer.h"

LCmdHandlerGroup::LCmdHandlerGroup( minet::MCommandCommunicator* pCC )
: m_pCC(pCC)
, m_pCmdHandler_Login(NULL)
{
	m_pCmdHandler_NotStartedServer = new LCmdHandler_NotServable(pCC);

	//////////////////////////////////////////////////////////////////////////
	m_pCmdHandler_MoveServer = new LCmdHandler_MoveServer(pCC);
}

LCmdHandlerGroup::~LCmdHandlerGroup()
{
	SAFE_DELETE(m_pCmdHandler_NotStartedServer);
	SAFE_DELETE(m_pCmdHandler_Login);
	SAFE_DELETE(m_pCmdHandler_MoveServer);
}

void LCmdHandlerGroup::ChangeCmdHandler_ServerStart()
{
	if (NULL != m_pCmdHandler_Login &&
		NULL == m_pCmdHandler_NotStartedServer) return;

	SAFE_DELETE(m_pCmdHandler_NotStartedServer);

	m_pCmdHandler_Login = new LCmdHandler_Login(m_pCC);
}

//////////////////////////////////////////////////////////////////////////

#include "LCmdHandler_Master_Global.h"
#include "LCmdHandler_Master_Net.h"
#include "LCmdHandler_Master_MoveServer.h"

LCmdHandlerGroup_MasterNetClient::LCmdHandlerGroup_MasterNetClient(minet::MCommandCommunicator* pCC)
{
	m_pGlobalCmdHandler = new LCmdHandler_Master_Global(pCC);
	m_pNetCmdHandler = new LCmdHandler_Master_Net(pCC);
	m_pMoveServerCmdHandler = new LCmdHandler_Master_MoveServer(pCC);

}

LCmdHandlerGroup_MasterNetClient::~LCmdHandlerGroup_MasterNetClient()
{
	SAFE_DELETE(m_pGlobalCmdHandler);
	SAFE_DELETE(m_pNetCmdHandler);
	SAFE_DELETE(m_pMoveServerCmdHandler);
}
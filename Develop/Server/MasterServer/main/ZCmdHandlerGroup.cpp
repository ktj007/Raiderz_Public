#include "stdafx.h"
#include "ZCmdHandlerGroup.h"
#include "ZCmdHandler_Comm.h"
#include "ZCmdHandler_Field.h"
#include "ZCmdHandler_Loot.h"
#include "ZCmdHandler_Char.h"
#include "ZCmdHandler_Party.h"
#include "ZCmdHandler_Guild.h"
#include "ZCmdHandler_MoveServer.h"

ZCmdHandlerGroup::ZCmdHandlerGroup( minet::MCommandCommunicator* pCC )
{
	m_pMasterCmdHandler = new ZCmdHandler_Comm(pCC);
	m_pFieldCmdHandler = new ZCmdHandler_Field(pCC);
	m_pLootCmdHandler = new ZCmdHandler_Loot(pCC);
	m_pCharCmdHandler = new ZCmdHandler_Char(pCC);
	m_pPartyCmdHandler = new ZCmdHandler_Party(pCC);
	m_pGuildCmdHandler = new ZCmdHandler_Guild(pCC);
	m_pMoveServerCmdHandler = new ZCmdHandler_MoveServer(pCC);
}

ZCmdHandlerGroup::~ZCmdHandlerGroup()
{
	SAFE_DELETE(m_pMasterCmdHandler);
	SAFE_DELETE(m_pFieldCmdHandler);
	SAFE_DELETE(m_pLootCmdHandler);
	SAFE_DELETE(m_pCharCmdHandler);
	SAFE_DELETE(m_pPartyCmdHandler);
	SAFE_DELETE(m_pGuildCmdHandler);
	SAFE_DELETE(m_pMoveServerCmdHandler);
}
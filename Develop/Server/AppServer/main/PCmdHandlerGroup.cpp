#include "stdafx.h"
#include "PCmdHandlerGroup.h"
#include "PCmdHandler_Master.h"
#include "Log/PCmdHandler_Log.h"
#include "PCmdHandler_Chat.h"
#include "PCmdHandler_Mail.h"
#include "PCmdHandler_TradeMarket.h"

PCmdHandlerGroup::PCmdHandlerGroup( minet::MCommandCommunicator* pCC )
{
	m_pMasterCmdHandler = new PCmdHandler_Master(pCC);
	m_pLogCmdHandler = new PCmdHandler_Log(pCC);
	m_pChatCmdHandler = new PCmdHandler_Chat(pCC);
	m_pMailCmdHandler = new PCmdHandler_Mail(pCC);
	m_pTradeMarketHandler = new PCmdHandler_TradeMarket(pCC);
}

PCmdHandlerGroup::~PCmdHandlerGroup()
{
	SAFE_DELETE(m_pMasterCmdHandler);
	SAFE_DELETE(m_pLogCmdHandler);
	SAFE_DELETE(m_pChatCmdHandler);
	SAFE_DELETE(m_pMailCmdHandler);
	SAFE_DELETE(m_pTradeMarketHandler);
}
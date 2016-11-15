#pragma once

#include "MCommandCommunicator.h"
#include "MCommandHandler.h"

class PCmdHandler_Master;
class PCmdHandler_Log;
class PCmdHandler_Chat;
class PCmdHandler_Mail;
class PCmdHandler_TradeMarket;

class PCmdHandlerGroup
{
protected:
	PCmdHandler_Master*			m_pMasterCmdHandler;	
	PCmdHandler_Log*			m_pLogCmdHandler;
	PCmdHandler_Chat*			m_pChatCmdHandler;
	PCmdHandler_Mail*			m_pMailCmdHandler;
	PCmdHandler_TradeMarket*	m_pTradeMarketHandler;

public:
	PCmdHandlerGroup(minet::MCommandCommunicator* pCC);
	~PCmdHandlerGroup();
};

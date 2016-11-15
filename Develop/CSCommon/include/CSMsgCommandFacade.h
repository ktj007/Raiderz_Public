#pragma once

#include "CSDef.h"
#include "MCommand.h"

class CSMsgHandler;

class CSMsgCommandFacade
{
public:
	typedef map<CSMsgType, CSMsgHandler*>		MsgHandlerMap;
	typedef	MsgHandlerMap::iterator			handler_iterator;

public:
	CSMsgCommandFacade();
	virtual ~CSMsgCommandFacade();

	CSMsgHandler* SetHandler(CSMsgType eType, CSMsgHandler* pNewHandler);

	bool RequestMessage(const minet::MCommand* pCmd);
	bool ResponseMessage(const minet::MCommand* pCmd);
	bool IsExistHandler(CSMsgType eType);

private:
	MsgHandlerMap m_mapHandler;
};

#pragma once

#include "XMsgHandler.h"

class XGuildMsgHandler: public XMsgHandler
{
public:
	XGuildMsgHandler();
	virtual ~XGuildMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_GUILD;	}
};

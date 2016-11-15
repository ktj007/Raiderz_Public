#pragma once

#include "PMsgHandler.h"

class PGuildMsgHandler: public PMsgHandler
{
public:
	PGuildMsgHandler();
	virtual ~PGuildMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_GUILD;	}
};

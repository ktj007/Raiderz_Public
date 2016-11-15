#pragma once

#include "PMsgHandler.h"

class PGlobalMsgHandler: public PMsgHandler
{
public:
	PGlobalMsgHandler();
	virtual ~PGlobalMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_GLOBAL;	}
};

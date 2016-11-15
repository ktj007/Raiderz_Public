#pragma once

#include "PMsgHandler.h"

class PFieldMsgHandler: public PMsgHandler
{
public:
	PFieldMsgHandler();
	virtual ~PFieldMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_FIELD;	}
};

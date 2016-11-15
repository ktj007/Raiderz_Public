#pragma once

#include "XMsgHandler.h"

class XGlobalMsgHandler: public XMsgHandler
{
public:
	XGlobalMsgHandler();
	virtual ~XGlobalMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_GLOBAL;	}
};

#pragma once

#include "XMsgHandler.h"

class XSystemMsgHandler: public XMsgHandler
{
public:
	XSystemMsgHandler();
	virtual ~XSystemMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_SYSTEM;	}
};

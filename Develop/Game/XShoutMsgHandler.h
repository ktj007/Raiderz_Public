#pragma once

#include "XMsgHandler.h"

class XShoutMsgHandler: public XMsgHandler
{
public:
	XShoutMsgHandler();
	virtual ~XShoutMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_SHOUT;	}
};

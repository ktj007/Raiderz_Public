#pragma once

#include "XMsgHandler.h"

class XShoutFakeMsgHandler: public XMsgHandler
{
public:
	XShoutFakeMsgHandler();
	virtual ~XShoutFakeMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_SHOUT_FAKE;	}
};

#pragma once

#include "XMsgHandler.h"

class XChannelMsgHandler: public XMsgHandler
{
public:
	XChannelMsgHandler();
	virtual ~XChannelMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_CHANNEL;	}
};

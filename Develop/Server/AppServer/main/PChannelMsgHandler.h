#pragma once

#include "PMsgHandler.h"

class PChannelMsgHandler: public PMsgHandler
{
public:
	PChannelMsgHandler();
	virtual ~PChannelMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_CHANNEL;	}
};

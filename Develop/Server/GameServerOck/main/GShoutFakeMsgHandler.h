#pragma once

#include "GMsgHandler.h"

class GShoutFakeMsgHandler: public GMsgHandler, public MTestMemPool<GShoutFakeMsgHandler>
{
public:
	GShoutFakeMsgHandler();
	virtual ~GShoutFakeMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual bool OnResponse(const minet::MCommand* pCmd);	

	virtual CSMsgType GetHandlerType(void)	{ return MT_SHOUT_FAKE;	}
};

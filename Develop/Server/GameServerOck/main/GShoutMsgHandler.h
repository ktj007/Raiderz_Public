#pragma once

#include "GMsgHandler.h"

class GShoutMsgHandler: public GMsgHandler, public MTestMemPool<GShoutMsgHandler>
{
public:
	GShoutMsgHandler();
	virtual ~GShoutMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual bool OnResponse(const minet::MCommand* pCmd);	
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_SHOUT;	}
};

#pragma once

#include "GMsgHandler.h"

class GChannelMsgHandler: public GMsgHandler, public MTestMemPool<GChannelMsgHandler>
{
public:
	GChannelMsgHandler();
	virtual ~GChannelMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_CHANNEL;	}
};

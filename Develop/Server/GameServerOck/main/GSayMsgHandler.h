#pragma once

#include "GMsgHandler.h"

class GSayMsgHandler: public GMsgHandler, public MTestMemPool<GSayMsgHandler>
{
public:
	GSayMsgHandler();
	virtual ~GSayMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual bool OnResponse(const minet::MCommand* pCmd);	
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_SAY;	}
};

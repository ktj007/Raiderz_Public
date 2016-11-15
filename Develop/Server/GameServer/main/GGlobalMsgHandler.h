#pragma once

#include "GMsgHandler.h"

class GGlobalMsgHandler: public GMsgHandler, public MTestMemPool<GGlobalMsgHandler>
{
public:
	GGlobalMsgHandler();
	virtual ~GGlobalMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_GLOBAL;	}
};

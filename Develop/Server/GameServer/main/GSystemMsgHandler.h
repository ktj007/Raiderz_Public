#pragma once

#include "GMsgHandler.h"

class GSystemMsgHandler: public GMsgHandler, public MTestMemPool<GSystemMsgHandler>
{
public:
	GSystemMsgHandler();
	virtual ~GSystemMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);

	virtual CSMsgType GetHandlerType(void)	{ return MT_SYSTEM;	}
};

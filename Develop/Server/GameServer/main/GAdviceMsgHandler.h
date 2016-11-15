#pragma once

#include "GMsgHandler.h"

class GAdviceMsgHandler: public GMsgHandler, public MTestMemPool<GAdviceMsgHandler>
{
public:
	GAdviceMsgHandler();
	virtual ~GAdviceMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);

	virtual CSMsgType GetHandlerType(void)	{ return MT_SYSTEM;	}
};

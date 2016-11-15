#pragma once

#include "GMsgHandler.h"

class GNarrationMsgHandler: public GMsgHandler, public MTestMemPool<GNarrationMsgHandler>
{
public:
	GNarrationMsgHandler();
	virtual ~GNarrationMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_NARRATION;	}
};

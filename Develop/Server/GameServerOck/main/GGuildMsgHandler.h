#pragma once

#include "GMsgHandler.h"

class GGuildMsgHandler: public GMsgHandler, public MTestMemPool<GGuildMsgHandler>
{
public:
	GGuildMsgHandler();
	virtual ~GGuildMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_GUILD;	}
};

#pragma once

#include "GMsgHandler.h"

class GPartyMsgHandler: public GMsgHandler, public MTestMemPool<GPartyMsgHandler>
{
public:
	GPartyMsgHandler();
	virtual ~GPartyMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual bool OnResponse(const minet::MCommand* pCmd);	
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_PARTY;	}
};

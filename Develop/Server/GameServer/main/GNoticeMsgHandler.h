#pragma once

#include "GMsgHandler.h"

class GNoticeMsgHandler: public GMsgHandler, public MTestMemPool<GNoticeMsgHandler>
{
public:
	GNoticeMsgHandler();
	virtual ~GNoticeMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);

	virtual CSMsgType GetHandlerType(void)	{ return MT_NOTICE;	}
};

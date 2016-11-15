#pragma once

#include "GMsgHandler.h"

class GAnnounceMsgHandler: public GMsgHandler, public MTestMemPool<GAnnounceMsgHandler>
{
public:
	GAnnounceMsgHandler();
	virtual ~GAnnounceMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_ANNOUNCE;	}
};

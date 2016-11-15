#pragma once

#include "GMsgHandler.h"

class GField;

class GFieldMsgHandler: public GMsgHandler, public MTestMemPool<GFieldMsgHandler>
{
public:
	GFieldMsgHandler();
	virtual ~GFieldMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_FIELD;	}
};

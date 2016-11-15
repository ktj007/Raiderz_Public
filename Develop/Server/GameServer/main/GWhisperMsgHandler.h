#pragma once

#include "GMsgHandler.h"

class GWhisperMsgHandler: public GMsgHandler, public MTestMemPool<GWhisperMsgHandler>
{
public:
	GWhisperMsgHandler();
	virtual ~GWhisperMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_WHISPER;	}
};

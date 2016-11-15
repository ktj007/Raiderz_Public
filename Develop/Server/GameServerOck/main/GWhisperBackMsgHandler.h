#pragma once

#include "GMsgHandler.h"

class GWhisperBackMsgHandler: public GMsgHandler, public MTestMemPool<GWhisperBackMsgHandler>
{
public:
	GWhisperBackMsgHandler();
	virtual ~GWhisperBackMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual bool OnResponse(const minet::MCommand* pCmd);	

	virtual CSMsgType GetHandlerType(void)	{ return MT_WHISPER_BACK;	}
};

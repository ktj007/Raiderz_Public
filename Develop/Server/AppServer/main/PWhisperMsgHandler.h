#pragma once

#include "PMsgHandler.h"

class PWhisperMsgHandler: public PMsgHandler
{
public:
	PWhisperMsgHandler();
	virtual ~PWhisperMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_WHISPER;	}
};

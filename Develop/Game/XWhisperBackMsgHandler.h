#pragma once

#include "XMsgHandler.h"

class XWhisperBackMsgHandler: public XMsgHandler
{
public:
	XWhisperBackMsgHandler();
	virtual ~XWhisperBackMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);	
	virtual CSMsgType GetHandlerType(void)	{ return MT_WHISPER_BACK;	}
};

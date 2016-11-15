#pragma once

#include "XMsgHandler.h"

class XWhisperMsgHandler: public XMsgHandler
{
public:
	XWhisperMsgHandler();
	virtual ~XWhisperMsgHandler();
	
	virtual bool OnResponse(const minet::MCommand* pCmd);	
	virtual CSMsgType GetHandlerType(void)	{ return MT_WHISPER;	}
};

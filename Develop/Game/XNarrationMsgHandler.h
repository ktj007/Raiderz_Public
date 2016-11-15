#pragma once

#include "XMsgHandler.h"

class XNarrationMsgHandler: public XMsgHandler
{
public:
	XNarrationMsgHandler();
	virtual ~XNarrationMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_NARRATION;	}
};

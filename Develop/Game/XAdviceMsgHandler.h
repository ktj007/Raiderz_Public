#pragma once

#include "XMsgHandler.h"

class XAdviceMsgHandler: public XMsgHandler
{
public:
	XAdviceMsgHandler();
	virtual ~XAdviceMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_ADVICE;	}
};

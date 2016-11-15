#pragma once

#include "XMsgHandler.h"

class XSayMsgHandler: public XMsgHandler
{
public:
	XSayMsgHandler();
	virtual ~XSayMsgHandler();
	
	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_SAY;	}
};

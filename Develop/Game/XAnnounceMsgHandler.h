#pragma once

#include "XMsgHandler.h"

class XAnnounceMsgHandler: public XMsgHandler
{
public:
	XAnnounceMsgHandler();
	virtual ~XAnnounceMsgHandler();
	
	virtual bool OnResponse(const minet::MCommand* pCmd);	
	virtual CSMsgType GetHandlerType(void)	{ return MT_ANNOUNCE;	}
};

#pragma once

#include "XMsgHandler.h"

class XNoticeMsgHandler: public XMsgHandler
{
public:
	XNoticeMsgHandler();
	virtual ~XNoticeMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_NOTICE;	}
};

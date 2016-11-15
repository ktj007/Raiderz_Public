#pragma once

#include "XMsgHandler.h"

class XFieldMsgHandler: public XMsgHandler
{
public:
	XFieldMsgHandler();
	virtual ~XFieldMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_FIELD;	}
};

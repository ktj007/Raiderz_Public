#pragma once

#include "XMsgHandler.h"

class XPartyMsgHandler: public XMsgHandler
{
public:
	XPartyMsgHandler();
	virtual ~XPartyMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_PARTY;	}
};

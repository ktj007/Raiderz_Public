#pragma once

#include "PMsgHandler.h"

class PNoticeMsgHandler: public PMsgHandler
{
public:
	PNoticeMsgHandler();
	virtual ~PNoticeMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_NOTICE;	}
};

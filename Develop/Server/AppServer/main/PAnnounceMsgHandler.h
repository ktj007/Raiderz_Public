#pragma once

#include "PMsgHandler.h"

class PAnnounceMsgHandler: public PMsgHandler
{
public:
	PAnnounceMsgHandler();
	virtual ~PAnnounceMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_ANNOUNCE;	}
};

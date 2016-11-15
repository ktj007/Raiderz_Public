#pragma once

#include "PMsgHandler.h"

class PPartyMsgHandler: public PMsgHandler
{
public:
	PPartyMsgHandler();
	virtual ~PPartyMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_PARTY;	}
};

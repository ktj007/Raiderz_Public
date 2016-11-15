#pragma once

#include "PMsgHandler.h"

class PAuctionMsgHandler: public PMsgHandler
{
public:
	PAuctionMsgHandler();
	virtual ~PAuctionMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_AUCTION;	}
};

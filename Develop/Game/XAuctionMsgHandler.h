#pragma once

#include "XMsgHandler.h"

class XAuctionMsgHandler: public XMsgHandler
{
public:
	XAuctionMsgHandler();
	virtual ~XAuctionMsgHandler();

	virtual bool OnResponse(const minet::MCommand* pCmd);
	virtual CSMsgType GetHandlerType(void)	{ return MT_AUCTION;	}
};

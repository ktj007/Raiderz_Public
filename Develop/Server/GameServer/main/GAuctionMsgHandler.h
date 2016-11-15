#pragma once

#include "GMsgHandler.h"

class GAuctionMsgHandler: public GMsgHandler, public MTestMemPool<GAuctionMsgHandler>
{
public:
	GAuctionMsgHandler();
	virtual ~GAuctionMsgHandler();

	virtual bool OnRequest(const MCommand* pCmd);
	virtual bool OnResponse(const MCommand* pCmd);
	
	virtual CSMsgType GetHandlerType(void)	{ return MT_AUCTION;	}

private:

};

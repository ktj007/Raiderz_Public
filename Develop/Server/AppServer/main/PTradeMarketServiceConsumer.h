#pragma once
#include <vector>
#include "STypes.h"
#include "MCommand.h"
#include "SServerApplication.h"

struct TD_TRADEMARKETITEM;
using namespace minet;

class PTradeMarketServiceConsumer : public SServerApplication::MsgHandler
{
public:
	PTradeMarketServiceConsumer();
	virtual ~PTradeMarketServiceConsumer();

	virtual void NotifyAddItem(MCommand* cmd);
	virtual void NotifyDelItem(MCommand* cmd);
	virtual void RequestSearch(MCommand* cmd);
	virtual void RequestSearchNext(MCommand* cmd);

private:
	void OnRecvCmd(MCommand* const cmd) override;
};
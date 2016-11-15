#ifndef _G_TRADE_VALIDATOR_H_
#define _G_TRADE_VALIDATOR_H_

#include "GBaseValidator.h"

class GTradeInfo;

class GTradeInfoValidator : public GBaseValidator
{
public:
	void CheckTrade(GTradeInfo* pTradeInfo);

public:
	virtual bool Check();
};

#endif//_G_TRADE_VALIDATOR_H_

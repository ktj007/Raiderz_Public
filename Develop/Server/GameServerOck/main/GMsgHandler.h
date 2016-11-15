#pragma once

#include "CSMsgHandler.h"

class GMsgHandler: public CSMsgHandler
{
public:
	GMsgHandler();
	virtual ~GMsgHandler();

	virtual bool OnPreRequest(const minet::MCommand* pCmd) override;
};

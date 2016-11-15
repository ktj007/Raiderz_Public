#pragma once

#include "CSMsgHandler.h"

class PMsgHandler: public CSMsgHandler
{
public:
	PMsgHandler();
	virtual ~PMsgHandler();
	
	virtual bool OnResponse(const minet::MCommand* pCmd) sealed;
};

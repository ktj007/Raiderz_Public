#pragma once

#include "CSMsgCommandFacade.h"
#include "STypes.h"

class PMsgCommandFacade: public CSMsgCommandFacade
{
public:
	PMsgCommandFacade();
	virtual ~PMsgCommandFacade();

	void RouteNoticeMsg(wstring strMsg);
	bool RouteSystemMsg(CCommandResultTable nResult, CID cidTargetPlayer);

private:
	bool RouteTargetPlayer(minet::MCommand* pCmd, CID cidTargetPlayer);
};

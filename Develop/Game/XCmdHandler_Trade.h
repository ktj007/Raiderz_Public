#ifndef _XCMD_HANDLER_TRADE_H_
#define _XCMD_HANDLER_TRADE_H_

#include "McommandHandler.h"
using namespace minet;

/// 트레이드에서 사용하는 커맨드 처리
class XCmdHandler_Trade : public MCommandHandler
{
protected:
	DECL_CMD_HANDLER(OnTradeStartReqRelay);
	DECL_CMD_HANDLER(OnTradeStart);	
	DECL_CMD_HANDLER(OnTradeStartReject);
	DECL_CMD_HANDLER(OnTradePutUpItem);
	DECL_CMD_HANDLER(OnTradePutDownItem);
	DECL_CMD_HANDLER(OnTradePutUpMoney);
	DECL_CMD_HANDLER(OnTradeCancel);
	DECL_CMD_HANDLER(OnTradeConfirm);
	DECL_CMD_HANDLER(OnTradeComplete);

public:
	XCmdHandler_Trade(MCommandCommunicator* pCC);
};

#endif //_XCMD_HANDLER_TRADE_H_

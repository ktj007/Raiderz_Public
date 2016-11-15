#pragma once

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_Mail : public MCommandHandler
{
public:
	XCmdHandler_Mail(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMailboxIsOpened);
	DECL_CMD_HANDLER(OnMailboxIsClosed);
	DECL_CMD_HANDLER(OnGetMailboxListRes);
	DECL_CMD_HANDLER(OnPostRes);
	DECL_CMD_HANDLER(OnReadRes);
	DECL_CMD_HANDLER(OnDeleteRes);
	DECL_CMD_HANDLER(OnTakeAppendedItemRes);
	DECL_CMD_HANDLER(OnTakeAppendedMoneyRes);
	DECL_CMD_HANDLER(OnAppendPutUpItemRes);
	DECL_CMD_HANDLER(OnAppendPutDownItemRes);
	DECL_CMD_HANDLER(OnNotifyMail);
};

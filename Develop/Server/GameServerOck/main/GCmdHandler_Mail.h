#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Mail : public MCommandHandler, public MTestMemPool<GCmdHandler_Mail>
{
public:
	GCmdHandler_Mail(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnGetMailboxListReq);
	DECL_CMD_HANDLER(OnPostReq);
	DECL_CMD_HANDLER(OnReadReq);
	DECL_CMD_HANDLER(OnDeleteReq);
	DECL_CMD_HANDLER(OnTakeAppendedItemReq);
	DECL_CMD_HANDLER(OnTakeAppendedMoneyReq);
	DECL_CMD_HANDLER(OnPutUpItemReq);
	DECL_CMD_HANDLER(OnPutDownItemReq);
	DECL_CMD_HANDLER(OnResetAppendedItem);
};

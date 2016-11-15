#pragma once

#include "MCommandHandler.h"
using namespace minet;

/// 통신관련 커맨드 핸들러
class LCmdHandler_Master : public MCommandHandler
{
private:

public:
	LCmdHandler_Master(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnNetClear);
	DECL_CMD_HANDLER(OnRequestLogin);
};


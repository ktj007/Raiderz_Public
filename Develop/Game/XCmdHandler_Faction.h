#pragma once

#include "MCommandHandler.h"
using namespace minet;

/// 팩션 커맨드 처리 핸들러
class XCmdHandler_Faction : public MCommandHandler
{
public:
	XCmdHandler_Faction(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnIncrease);
	DECL_CMD_HANDLER(OnDecrease);
};

#pragma once

#include "McommandHandler.h"
using namespace minet;

/// 게임 서버에 들어가기 위한 커맨드 핸들러
class XCmdHandler_GameStart : public MCommandHandler
{
protected:
	static bool CommonResponseLogin(MCommand* pCommand, MCommandHandler* pHandler);
	DECL_CMD_HANDLER(OnResponseLoginGameServer);
	DECL_CMD_HANDLER(OnResponseMoveGameServer);
public:
	XCmdHandler_GameStart(MCommandCommunicator* pCC);	
};


#pragma once

#include "McommandHandler.h"
using namespace minet;

class XCmdHandler_MoveServer : public MCommandHandler
{
public:
	XCmdHandler_MoveServer(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMoveGameServer);
	DECL_CMD_HANDLER(OnStartMoveToLoginServerRes);	///< 서버이동 시작.
	DECL_CMD_HANDLER(OnResponseMoveLoginServer);	///< 서버이동해서 로그인서버에 인증 요청.

};

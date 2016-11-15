#pragma once

#include "McommandHandler.h"
#include "XCmdParam_LoginServer.h"
using namespace minet;

class XCmdHandler_LoginServer : public MCommandHandler
{
protected:
	static MCommandResult _OnResponseLogin(XCmdParam__MC_COMM_RESPONSE_LOGIN& cmdParam);
protected:
	DECL_CMD_HANDLER(OnResponseLogin);
	DECL_CMD_HANDLER(OnResponseLoginOnPmang);
	DECL_CMD_HANDLER(OnResponseAccountCharList);
	DECL_CMD_HANDLER(OnResponseSelMyChar);
	DECL_CMD_HANDLER(OnResponseInsertChar);
	DECL_CMD_HANDLER(OnResponseDeleteChar);
	DECL_CMD_HANDLER(OnMoveToGameServer);
	DECL_CMD_HANDLER(OnEnterWorldMsg);
	DECL_CMD_HANDLER(OnEnterWorldMsgLocal);
	DECL_CMD_HANDLER(OnDuplicatedPlayerLogin);

public:
	XCmdHandler_LoginServer(MCommandCommunicator* pCC);	

private:
	static void CommEnterWorldMsg(MCommand* pCommand);
};


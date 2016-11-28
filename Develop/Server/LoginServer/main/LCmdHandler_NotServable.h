#pragma once

#include "MCommandHandler.h"
using namespace minet;

class LCmdHandler_NotServable : public MCommandHandler
{
public:
	LCmdHandler_NotServable(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnLogin);
	DECL_CMD_HANDLER(OnPmangLogin);
	DECL_CMD_HANDLER(OnPWELogin);

private:
	static bool	_Login(MUID uidPlayer, int nCommandVersion);
	static bool	_PmangLogin(MUID uidPlayer, int nCommandVersion);
	static bool	_PWELogin(MUID uidPlayer, int nCommandVersion);

};

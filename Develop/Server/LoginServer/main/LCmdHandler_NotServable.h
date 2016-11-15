#pragma once

#include "MCommandHandler.h"
using namespace minet;

class LCmdHandler_NotServable : public MCommandHandler
{
public:
	LCmdHandler_NotServable(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnLogin);
	DECL_CMD_HANDLER(OnPmangLogin);

private:
	static bool	_Login(MUID uidPlayer, int nCommandVersion);

};

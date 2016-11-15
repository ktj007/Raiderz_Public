#ifndef _GCMD_HANDLER_LOGIN_H
#define _GCMD_HANDLER_LOGIN_H

#include "MCommandHandler.h"
using namespace minet;

/// 통신관련 커맨드 핸들러
class LCmdHandler_Login : public MCommandHandler
{
public:
	LCmdHandler_Login(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnLogin);			
	DECL_CMD_HANDLER(OnPmangLogin);			
	DECL_CMD_HANDLER(OnNetClear);
	DECL_CMD_HANDLER(OnRequestAccountCharList);
	DECL_CMD_HANDLER(OnRequestInsertCharacter);
	DECL_CMD_HANDLER(OnRequestDeleteCharacter);
	DECL_CMD_HANDLER(OnRequestSelMyChar);
};

#endif

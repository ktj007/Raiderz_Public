#ifndef _XCMD_HANDLER_INN_H
#define _XCMD_HANDLER_INN_H

#include "MCommandHandler.h"
using namespace minet;

/// 여관 커맨드 처리 핸들러
class XCmdHandler_Inn : public MCommandHandler
{
public:
	XCmdHandler_Inn(MCommandCommunicator* pCC);
	
	DECL_CMD_HANDLER(OnBeginSleep);
	DECL_CMD_HANDLER(OnEndSleep);
};

#endif//_XCMD_HANDLER_INN_H

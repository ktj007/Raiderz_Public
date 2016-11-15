#ifndef _XCMD_HANDLER_ENCHANT_H
#define _XCMD_HANDLER_ENCHANT_H

#include "MCommandHandler.h"
using namespace minet;

/// 인체트 커맨드 처리 핸들러
class XCmdHandler_Enchant : public MCommandHandler
{
public:
	XCmdHandler_Enchant(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnEnchantDone);
	DECL_CMD_HANDLER(OnEnchantBroken);
	DECL_CMD_HANDLER(OnEnchantCheckResult);
};

#endif//_XCMD_HANDLER_ENCHANT_H

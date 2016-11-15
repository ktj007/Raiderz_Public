#ifndef _XCMD_HANDLER_CRAFT_H
#define _XCMD_HANDLER_CRAFT_H

#include "MCommandHandler.h"
using namespace minet;

/// 크래프트 커맨드 처리 핸들러
class XCmdHandler_Craft : public MCommandHandler
{
public:
	XCmdHandler_Craft(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnShow);
	DECL_CMD_HANDLER(OnMake);
	DECL_CMD_HANDLER(OnRecipeInsert);
	DECL_CMD_HANDLER(OnRecipeDelete);
};

#endif//_XCMD_HANDLER_CRAFT_H

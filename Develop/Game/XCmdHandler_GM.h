#ifndef _XCMD_HANDLER_GM_H
#define _XCMD_HANDLER_GM_H

#include "MCommandHandler.h"
using namespace minet;

/// 운영자가 사용하는 커맨드 처리
class XCmdHandler_GM : public MCommandHandler
{
public:
	XCmdHandler_GM(MCommandCommunicator* pCC);
	
	DECL_CMD_HANDLER(OnQuestResetDone);
	DECL_CMD_HANDLER(OnQuestVar);
	DECL_CMD_HANDLER(OnShowEnemyUID);
	DECL_CMD_HANDLER(OnLogCRTList);	
	DECL_CMD_HANDLER(OnResponseServerDump);	

	DECL_CMD_HANDLER(OnGMChangeServerMode);
};

#endif//_XCMD_HANDLER_ADMIN_H

#ifndef _XCMD_HANDLER_GUILD_H
#define _XCMD_HANDLER_GUILD_H

#include "MCommandHandler.h"
using namespace minet;

/// 크래프트 커맨드 처리 핸들러
class XCmdHandler_Guild : public MCommandHandler
{
public:
	XCmdHandler_Guild(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnShowCreateUI);
	DECL_CMD_HANDLER(OnShowDestroyUI);

	DECL_CMD_HANDLER(OnCreate);
	DECL_CMD_HANDLER(OnInvite_Notify);
	DECL_CMD_HANDLER(OnInvite_Reject);
	DECL_CMD_HANDLER(OnJoinMe);
	DECL_CMD_HANDLER(OnJoinOther);	
	DECL_CMD_HANDLER(OnLeave);
	DECL_CMD_HANDLER(OnKick);
	DECL_CMD_HANDLER(OnDestroy);
	DECL_CMD_HANDLER(OnOnLine);
	DECL_CMD_HANDLER(OnOffLine);
	DECL_CMD_HANDLER(OnMoveField);
	DECL_CMD_HANDLER(OnChangeMaster);
	DECL_CMD_HANDLER(OnGuildInfo);

	DECL_CMD_HANDLER(OnGuildJoinSector);
	DECL_CMD_HANDLER(OnGuildLeaveSector);

	DECL_CMD_HANDLER(OnShowStorageUI);
	DECL_CMD_HANDLER(OnDepositStorageMoney);
	DECL_CMD_HANDLER(OnWithdrawStorageMoney);
	DECL_CMD_HANDLER(OnMoveStorageItem);
};

#endif//_XCMD_HANDLER_CRAFT_H

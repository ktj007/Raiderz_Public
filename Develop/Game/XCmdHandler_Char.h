#ifndef _XCMD_HANDLER_CHAR_H
#define _XCMD_HANDLER_CHAR_H

#include "MCommandHandler.h"
using namespace minet;

class XCmdHandler_Char  : public MCommandHandler
{
public:
	XCmdHandler_Char(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnMyInfo);
	DECL_CMD_HANDLER(OnMyTimeInfo);
	DECL_CMD_HANDLER(OnRebirthMyPlayer);
	DECL_CMD_HANDLER(OnRebirthNetPlayer);
	DECL_CMD_HANDLER(OnUpdateSimpleStatus);
	DECL_CMD_HANDLER(OnRewardExp);
	DECL_CMD_HANDLER(OnRewardItemXP);
	DECL_CMD_HANDLER(OnUpdateMoney);
	DECL_CMD_HANDLER(OnUpdateTP);
	DECL_CMD_HANDLER(OnLevelUp);
	DECL_CMD_HANDLER(OnSaveSoulBinding);
	DECL_CMD_HANDLER(OnSaveCheckPoint);
	DECL_CMD_HANDLER(OnEnemyInfo);
	DECL_CMD_HANDLER(OnEnemyInfoClear);
	DECL_CMD_HANDLER(OnTip);
	DECL_CMD_HANDLER(OnAddEnemyNPC);
	DECL_CMD_HANDLER(OnDelEnemyNPC);
	DECL_CMD_HANDLER(OnTargetInfo);
};

#endif
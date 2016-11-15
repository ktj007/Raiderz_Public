#pragma once

#include "MCommandHandler.h"
using namespace minet;

/// NPC Action 관련 커맨드 핸들러
class XCmdHandler_NPCAction  : public MCommandHandler
{
public:
	XCmdHandler_NPCAction(MCommandCommunicator* pCC);


	DECL_CMD_HANDLER(OnNPCMove);
	DECL_CMD_HANDLER(OnNPCRun);
	DECL_CMD_HANDLER(OnNPCMoveStop);
	DECL_CMD_HANDLER(OnNPCRotate);
	DECL_CMD_HANDLER(OnNPCFaceTo);
	DECL_CMD_HANDLER(OnNPCChangeCapsule);
	DECL_CMD_HANDLER(OnChangeAttackable);
	DECL_CMD_HANDLER(OnChangetCombatTarget);
	DECL_CMD_HANDLER(OnSpewUp);
	DECL_CMD_HANDLER(OnSpewUpInterrupt);

};

#ifndef _XCMD_HANDLER_GLOBAL_H
#define _XCMD_HANDLER_GLOBAL_H

#include "MCommandHandler.h"
using namespace minet;

/// 게임 전체에 필요한 커맨드 처리
/// 일단 왠만하면 다 여기서 처리한 다음 주체할 수 없을정도로 커지면 분리하도록 합시다. - birdkr
class XCmdHandler_Global  : public MCommandHandler
{
public:
	XCmdHandler_Global(MCommandCommunicator* pCC);

	
	DECL_CMD_HANDLER(OnSectorEntityInfo);
	DECL_CMD_HANDLER(OnUpdateUsableSensor);
	DECL_CMD_HANDLER(OnSensorDestinationSelection);
	DECL_CMD_HANDLER(OnSensorErrorMsg);
	DECL_CMD_HANDLER(OnSensorFollowPartyReq);
		
	DECL_CMD_HANDLER(OnPlayerAction);
	DECL_CMD_HANDLER(OnEntityWarp);
	DECL_CMD_HANDLER(OnPlayBGM);
	
	DECL_CMD_HANDLER(OnGMGod);
	DECL_CMD_HANDLER(OnGMGhost);
	DECL_CMD_HANDLER(OnTestDebug);
	DECL_CMD_HANDLER(OnAIDebugMessage);
	DECL_CMD_HANDLER(OnTimeSynch);
	DECL_CMD_HANDLER(OnDebugCombatcalc);
	DECL_CMD_HANDLER(OnDebugNPCNetLog);	

	// Dialog
	DECL_CMD_HANDLER(OnDialogStart);
	DECL_CMD_HANDLER(OnDialogSelect);
	DECL_CMD_HANDLER(OnGMGetUID);

	// GameGuard
	DECL_CMD_HANDLER(OnGameGuardAuthReq);
};

#endif
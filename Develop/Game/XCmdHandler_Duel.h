#pragma once

#include "MCommandHandler.h"
using namespace minet;

/// 결투 커맨드 처리 핸들러
class XCmdHandler_Duel : public MCommandHandler
{
public:
	XCmdHandler_Duel(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnDuelQuestion);
	DECL_CMD_HANDLER(OnDuelCancel);
	DECL_CMD_HANDLER(OnDuelReady);
	DECL_CMD_HANDLER(OnDuelFight);
	DECL_CMD_HANDLER(OnDuelFinished);
	DECL_CMD_HANDLER(OnDuelDefeated);
	
	DECL_CMD_HANDLER(OnDuelPartyReady);
	DECL_CMD_HANDLER(OnDuelPartyFight);
	DECL_CMD_HANDLER(OnDuelPartyCancel);
	DECL_CMD_HANDLER(OnDuelPartyFinished);

	DECL_CMD_HANDLER(OnDuelReadyCount);
};

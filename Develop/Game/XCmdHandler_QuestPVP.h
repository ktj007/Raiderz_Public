#pragma once

#include "MCommandHandler.h"
using namespace minet;

/// 퀘스트PVP 관련 커맨드 처리
class XCmdHandler_QuestPVP : public MCommandHandler
{
public:
	XCmdHandler_QuestPVP(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnTeamCount);
	DECL_CMD_HANDLER(OnEnterPvpZone);
	DECL_CMD_HANDLER(OnBeginEvent);
	DECL_CMD_HANDLER(OnEndEvent);
	DECL_CMD_HANDLER(OnMarkNPCPos);
	DECL_CMD_HANDLER(OnUnmarkNPCPos);	
};

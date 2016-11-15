#ifndef _XCMD_HANDLER_QUEST_H
#define _XCMD_HANDLER_QUEST_H

#include "MCommandHandler.h"
using namespace minet;

/// 퀘스트에서 사용하는 커맨드 처리
class XCmdHandler_Quest  : public MCommandHandler
{
private:
protected:

public:
	XCmdHandler_Quest(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnShareReqRelay);
	DECL_CMD_HANDLER(OnShare);
	DECL_CMD_HANDLER(OnShowAcceptUI);
	DECL_CMD_HANDLER(OnShowRewardUI);
	DECL_CMD_HANDLER(OnGive);
	DECL_CMD_HANDLER(OnUpdateQObjective);
	DECL_CMD_HANDLER(OnComplete);
	DECL_CMD_HANDLER(OnIncomplete);
	DECL_CMD_HANDLER(OnFail);
	DECL_CMD_HANDLER(OnReward);
	DECL_CMD_HANDLER(OnCancel);
	DECL_CMD_HANDLER(OnInteractQObjective_Begin);
	DECL_CMD_HANDLER(OnInteractQObejctive_End);

	DECL_CMD_HANDLER(OnChallengerQuest);
	DECL_CMD_HANDLER(OnChallengerQuestComplete);
	DECL_CMD_HANDLER(OnChallengerQuestRefresh);
};



#endif
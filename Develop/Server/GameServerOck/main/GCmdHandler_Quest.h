#ifndef _GCMD_HANDLER_QUEST_H
#define _GCMD_HANDLER_QUEST_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Quest : public MCommandHandler, public MTestMemPool<GCmdHandler_Quest>
{
public:
	GCmdHandler_Quest(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestQuestGive);
	DECL_CMD_HANDLER(OnRequestQuestReward);
	DECL_CMD_HANDLER(OnRequestQuestCancel);
	DECL_CMD_HANDLER(OnRequestQuestShare);
	DECL_CMD_HANDLER(OnRequestQuestShareGive);
	DECL_CMD_HANDLER(OnInteractQObjectiveCancel);	
};

#endif//_GCMD_HANDLER_QUEST_H

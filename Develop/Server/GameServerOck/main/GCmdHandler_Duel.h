#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Duel : public MCommandHandler, public MTestMemPool<GCmdHandler_Duel>
{
public:
	GCmdHandler_Duel(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnDuelRequest);
	DECL_CMD_HANDLER(OnDuelPartyRequest);
	DECL_CMD_HANDLER(OnDuelQuestionRespond);
};

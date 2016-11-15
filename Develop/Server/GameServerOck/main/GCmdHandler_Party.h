#ifndef _GCMD_HANDLER_PARTY_H
#define _GCMD_HANDLER_PARTY_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Party : public MCommandHandler, public MTestMemPool<GCmdHandler_Party>
{
public:
	GCmdHandler_Party(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnRequestInviteParty);
	DECL_CMD_HANDLER(OnRequestInviteForMe);
	DECL_CMD_HANDLER(OnRequestInviteQuestionRespond);
	DECL_CMD_HANDLER(OnRequestLeaveParty);
	DECL_CMD_HANDLER(OnRequestKickParty);	
	DECL_CMD_HANDLER(OnRequestInviteForMeQuestionRespond);

	DECL_CMD_HANDLER(OnRequestChangeName);
	DECL_CMD_HANDLER(OnRequestChangeLeader);
	DECL_CMD_HANDLER(OnRequestChangeLootingRule);
	DECL_CMD_HANDLER(OnRequestQuestID);
	
	DECL_CMD_HANDLER(OnRequestAutoPartyEnqueue);
	DECL_CMD_HANDLER(OnRequestAutoPartyDequeue);
	DECL_CMD_HANDLER(OnRequestAutoPartyCheckSensor);
	DECL_CMD_HANDLER(OnRequestAutoPartyChangeState);
};

#endif//_GCMD_HANDLER_PARTY_H

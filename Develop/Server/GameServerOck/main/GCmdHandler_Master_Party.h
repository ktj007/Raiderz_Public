#pragma once

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Master_Party: public MCommandHandler, public MTestMemPool<GCmdHandler_Master_Party>
{
public:
	GCmdHandler_Master_Party(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnPartyInviteRes);
	DECL_CMD_HANDLER(OnPartyAcceptReq);
	DECL_CMD_HANDLER(OnPartyAcceptCancel);
	DECL_CMD_HANDLER(OnPartyMemberSync);
	DECL_CMD_HANDLER(OnPartySync);

	DECL_CMD_HANDLER(OnPartyAdd);
	DECL_CMD_HANDLER(OnPartyRemove);
	DECL_CMD_HANDLER(OnPartyAddMember);
	DECL_CMD_HANDLER(OnPartyRemoveMember);
	DECL_CMD_HANDLER(OnPartyQuestSync);
	DECL_CMD_HANDLER(OnPartyFieldSync);
	DECL_CMD_HANDLER(OnPartyQuestRunnerSync);	
	DECL_CMD_HANDLER(OnPartyLeaveRes);
	DECL_CMD_HANDLER(OnPartyEraseQuestRunnerSync);
	DECL_CMD_HANDLER(OnPartyKickRes);

	DECL_CMD_HANDLER(OnPartyMessage);

	DECL_CMD_HANDLER(OnPartyAddOfflineMember);
	DECL_CMD_HANDLER(OnPartyRemoveOfflineMember);

	DECL_CMD_HANDLER(OnPartyJoinAcceptReq);
	DECL_CMD_HANDLER(OnPartyJoinInviteRes);	
	DECL_CMD_HANDLER(OnPartyJoinAcceptCancel);

	DECL_CMD_HANDLER(OnPartyMoveServerSync);

	DECL_CMD_HANDLER(OnPartyInfoAllRes);

	DECL_CMD_HANDLER(OnPartyChangeNameRes);
	DECL_CMD_HANDLER(OnPartyChangeLeaderRes);
	DECL_CMD_HANDLER(OnPartyChangeLootingRuleRes);
	DECL_CMD_HANDLER(OnPartyChangeQuestIDRes);
	
	DECL_CMD_HANDLER(OnPartyFail);	
};

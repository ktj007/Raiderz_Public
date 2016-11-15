#pragma once

#include "MCommandHandler.h"
using namespace minet;

class ZServerAcceptor;

/// 통신관련 커맨드 핸들러
class ZCmdHandler_Party : public MCommandHandler
{
private:
	static bool CheckValidLoginReqCommand( ZServerAcceptor* pAcceptor, int nCommandVersion, int nWorldID );
public:
	ZCmdHandler_Party(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnPartyInviteReq);
	DECL_CMD_HANDLER(OnPartyAcceptRes);
	DECL_CMD_HANDLER(OnPartyUpdate);
	DECL_CMD_HANDLER(OnPartyUpdateMember);
	DECL_CMD_HANDLER(OnPartyAddQuest);
	DECL_CMD_HANDLER(OnPartyAddField);
	DECL_CMD_HANDLER(OnPartyUpdateQuestRunner);
	DECL_CMD_HANDLER(OnPartyLeaveReq);

	DECL_CMD_HANDLER(OnPartyRemoveQuest);
	DECL_CMD_HANDLER(OnPartyRemoveField);
	DECL_CMD_HANDLER(OnPartyEraseQuestRunner);

	DECL_CMD_HANDLER(OnPartyKickReq);	
	DECL_CMD_HANDLER(OnPartyDoOffline);
	DECL_CMD_HANDLER(OnPartyDoOnline);

	DECL_CMD_HANDLER(OnPartyJoinInviteReq);
	DECL_CMD_HANDLER(OnPartyJoinAcceptRes);

	DECL_CMD_HANDLER(OnPartyMoveServer);

	DECL_CMD_HANDLER(OnPartyCreateSingleReq);
	DECL_CMD_HANDLER(OnPartyInfoAllReq);
	DECL_CMD_HANDLER(OnPartyRemoveFieldSelf);

	DECL_CMD_HANDLER(OnPartyChangeNameReq);
	DECL_CMD_HANDLER(OnPartyChangeLeaderReq);
	DECL_CMD_HANDLER(OnPartyChangeLootingRuleReq);
	DECL_CMD_HANDLER(OnPartyChangeQuestIDReq);
	
	DECL_CMD_HANDLER(OnPartyFixedLogOn);
	DECL_CMD_HANDLER(OnPartyCreateAutoPartyReq);
	DECL_CMD_HANDLER(OnPartyJoinAutoPartyReq);
};

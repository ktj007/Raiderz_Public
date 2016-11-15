#ifndef _XCMD_HANDLER_PARTY_H
#define _XCMD_HANDLER_PARTY_H

#include "MCommandHandler.h"
using namespace minet;

/// 트레이닝 커맨드 처리 핸들러
class XCmdHandler_Party : public MCommandHandler
{
protected:
	DECL_CMD_HANDLER(OnInviteParty);
	DECL_CMD_HANDLER(OnInviteQuestion);
	DECL_CMD_HANDLER(OnInviteForMeQuestion);
	DECL_CMD_HANDLER(OnRefreshInfoAll);
	DECL_CMD_HANDLER(OnRefreshInfo);
	DECL_CMD_HANDLER(OnNotifyJoin);
	DECL_CMD_HANDLER(OnNotifyLeave);

	DECL_CMD_HANDLER(OnChangeLeader);
	DECL_CMD_HANDLER(OnChangeLootingRule);
	DECL_CMD_HANDLER(OnChangeLeaderNotify);	

	DECL_CMD_HANDLER(OnChangeState);	

public:
	XCmdHandler_Party(MCommandCommunicator* pCC);

};

#endif //_XCMD_HANDLER_TRAININIG_H

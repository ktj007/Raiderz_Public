#ifndef _GCMD_HANDLER_GUILD_H
#define _GCMD_HANDLER_GUILD_H

#include "MCommandHandler.h"
using namespace minet;

class GCmdHandler_Guild : public MCommandHandler, public MTestMemPool<GCmdHandler_Guild>
{
public:
	GCmdHandler_Guild(MCommandCommunicator* pCC);

	DECL_CMD_HANDLER(OnCreate);
	DECL_CMD_HANDLER(OnInvite_Req);
	DECL_CMD_HANDLER(OnInvite_Res);
	DECL_CMD_HANDLER(OnLeave);
	DECL_CMD_HANDLER(OnKick);
	DECL_CMD_HANDLER(OnDestroy);
	DECL_CMD_HANDLER(OnChangeMaster);
	DECL_CMD_HANDLER(OnDepositStorageMoney);
	DECL_CMD_HANDLER(OnWithdrawStorageMoney);
};

#endif//_GCMD_HANDLER_INN_H
